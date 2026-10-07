#include "resource_runtime_3ds.h"

#include "o2r_archive_reader.hpp"

#include <fast/resource/type/Texture.h>
#include <libultraship/libultra/gbi.h>
#include <libultraship/bridge/resourcebridge.h>
#include <ship/resource/ResourceManager.h>

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <memory>
#include <new>
#include <string_view>
#include <vector>

#if !defined(__3DS__)
namespace {
bool sFailResourceAllocation = false;
size_t sFailNextResourceAllocations = 0;
size_t sLinearAllocationCalls = 0;
size_t sResourceAllocationCalls = 0;
size_t sLoadingAudioServiceCalls = 0;
}

extern "C" void* linearAlloc(size_t size) {
    ++sLinearAllocationCalls;
    return std::malloc(size);
}

extern "C" void linearFree(void* memory) {
    std::free(memory);
}

extern "C" void* Mk64ResourceHeapAllocate(size_t size) {
    ++sResourceAllocationCalls;
    if (sFailResourceAllocation || sFailNextResourceAllocations != 0) {
        if (sFailNextResourceAllocations != 0) --sFailNextResourceAllocations;
        return nullptr;
    }
    return std::malloc(size);
}

extern "C" void Mk64ResourceHeapFree(void* memory) {
    std::free(memory);
}

extern "C" uint64_t Mk64Perf3DSResourceStart(void) {
    return 0;
}

extern "C" void Mk64Perf3DSResourceRead(uint64_t, size_t) {
}

extern "C" void Mk64Diagnostics3DSSetResource(const char*, size_t) {
}

extern "C" void Mk64Diagnostics3DSSetArchiveEntryCount(size_t) {
}

extern "C" void Mk64Diagnostics3DSFailure(const char*, const char*) {
}

extern "C" void Mk64Diagnostics3DSCheckpoint(const char*) {
}

extern "C" void AudioDma_Register(const void*, size_t) {
}

extern "C" void AudioDma_Clear(void) {
}

extern "C" void Mk64Graphics3DSEvictSourceTexture(const void*) {
}
extern "C" void Mk64GameAudio3DSServiceLoading(void) {
    ++sLoadingAudioServiceCalls;
}
#endif

struct CtlEntry;
struct AudioSequenceData;

extern "C" CtlEntry* GameEngine_LoadBank(uint8_t bankId);
extern "C" AudioSequenceData* GameEngine_LoadSequence(uint8_t sequenceId);
extern "C" uint32_t GameEngine_GetSequenceCount(void);

namespace {
bool ValidateStartupDisplayList(Gfx* commands, size_t commandCount) {
    if (commands == nullptr || commandCount < 3 ||
        static_cast<uint8_t>(commands[0].words.w0 >> 24) != 0x33 ||
        static_cast<uint8_t>(commands[commandCount - 1].words.w0 >> 24) != 0xB8) {
        return false;
    }
    for (size_t i = 0; i < commandCount; ++i) {
        const uint8_t opcode = static_cast<uint8_t>(commands[i].words.w0 >> 24);
        if (opcode != 0x32 && opcode != 0x33) {
            continue;
        }
        if (++i >= commandCount) {
            return false;
        }
        const uint64_t hash = (static_cast<uint64_t>(commands[i].words.w0) << 32) |
                              static_cast<uint32_t>(commands[i].words.w1);
        if (ResourceGetDataByCrc(hash) == nullptr) {
            return false;
        }
    }
    return true;
}

bool ValidateCrcIndex(const char* archivePath) {
    if (!Mk64Resource3DSValidateCrcIndex()) {
        return false;
    }
    mk64_3ds::O2rArchiveReader archive(archivePath);
    if (archive.Open() != mk64_3ds::O2rReadResult::Ok ||
        archive.Entries().size() != Mk64Resource3DSArchiveEntryCount()) {
        return false;
    }
    for (const std::string& path : archive.Entries()) {
        const uint64_t crc = ResourceGetCrcByName(path.c_str());
        const char* resolved = ResourceGetNameByCrc(crc);
        size_t nameSize = 0;
        size_t crcSize = 0;
        if (resolved == nullptr || std::strcmp(resolved, path.c_str()) != 0 ||
            !Mk64Resource3DSGetArchiveEntrySizeByName(path.c_str(), &nameSize) ||
            !Mk64Resource3DSGetArchiveEntrySizeByCrc(crc, &crcSize) || nameSize != crcSize) {
            return false;
        }
    }
    return true;
}

bool ValidateTransientTexture(Ship::ResourceManager& resourceManager, const char* texture) {
    const size_t baseline = Mk64Resource3DSLoadedCount();
    auto first = std::static_pointer_cast<Fast::Texture>(resourceManager.LoadResourceProcess(texture));
    if (first == nullptr || first->ImageData == nullptr || first->ImageDataSize == 0 ||
        Mk64Resource3DSLoadedCount() != baseline + 1U) {
        return false;
    }
    const uint8_t firstByte = first->ImageData[0];
    auto second = std::static_pointer_cast<Fast::Texture>(resourceManager.LoadResourceProcess(texture));
    std::weak_ptr<Ship::IResource> firstLifetime = first;
    if (second.get() != first.get() || Mk64Resource3DSLoadedCount() != baseline + 1U) {
        return false;
    }

    ResourceUnloadDirectory("textures/common");
    if (Mk64Resource3DSLoadedCount() != baseline + 1U) {
        return false;
    }
    ResourceUnloadDirectory("__OTR__textures/common_data");
    if (Mk64Resource3DSLoadedCount() != baseline + 1U || first->ImageData[0] != firstByte) {
        return false;
    }
    second.reset();
    if (firstLifetime.expired()) {
        return false;
    }
    first.reset();
    if (!firstLifetime.expired() || Mk64Resource3DSLoadedCount() != baseline) {
        return false;
    }

    auto reloaded = std::static_pointer_cast<Fast::Texture>(resourceManager.LoadResourceProcess(texture));
    std::weak_ptr<Ship::IResource> reloadedLifetime = reloaded;
    if (reloaded == nullptr || reloaded->ImageData == nullptr ||
        Mk64Resource3DSLoadedCount() != baseline + 1U) {
        return false;
    }
    ResourceDirtyDirectory("textures/common_data/");
    if (Mk64Resource3DSLoadedCount() != baseline + 1U) {
        return false;
    }
    reloaded.reset();
    return reloadedLifetime.expired() && Mk64Resource3DSLoadedCount() == baseline;
}

bool ValidateTextureOwnership(Ship::ResourceManager& resourceManager,
                              const char* texture) {
    const size_t baseline = Mk64Resource3DSLoadedCount();
    auto resource = std::static_pointer_cast<Fast::Texture>(
        resourceManager.LoadResourceProcess(texture));
    if (resource == nullptr || resource->ImageData == nullptr ||
        Mk64Resource3DSLoadedCount() != baseline + 1U) {
        return false;
    }
    resource.reset();
    // Owner-only texture sources follow the bounded Fast3D cache lifetime.
    if (Mk64Resource3DSLoadedCount() != baseline) {
        return false;
    }

    // A legacy raw borrow has no shared lifetime token. It must pin the source
    // even if Fast3D later acquires and releases the same texture.
    void* raw = ResourceGetDataByName(texture);
    if (raw == nullptr || Mk64Resource3DSLoadedCount() != baseline + 1U) return false;
    resource = std::static_pointer_cast<Fast::Texture>(
        resourceManager.LoadResourceProcess(texture));
    if (resource == nullptr || resource->ImageData != raw) return false;
    resource.reset();
    if (Mk64Resource3DSLoadedCount() != baseline + 1U) return false;
    ResourceUnloadDirectory("textures/tracks/luigi_raceway/");
    return Mk64Resource3DSLoadedCount() == baseline;
}

bool ValidateOutOfMemoryPropagation(Ship::ResourceManager& resourceManager,
                                    const char* texture) {
#if defined(__3DS__)
    (void)resourceManager;
    (void)texture;
    return true;
#else
    const size_t baseline = Mk64Resource3DSLoadedCount();
    sFailResourceAllocation = true;
    bool propagated = false;
    try {
        (void)resourceManager.LoadResourceProcess(texture);
    } catch (const std::bad_alloc&) {
        propagated = true;
    }
    sFailResourceAllocation = false;
    if (!propagated || Mk64Resource3DSLoadedCount() != baseline) {
        return false;
    }
    auto recovered = std::static_pointer_cast<Fast::Texture>(
        resourceManager.LoadResourceProcess(texture));
    return recovered != nullptr && recovered->ImageData != nullptr;
#endif
}

bool ValidateResidentPressureRecovery(Ship::ResourceManager& resourceManager,
                                      const char* texture) {
#if defined(__3DS__)
    (void)resourceManager;
    (void)texture;
    return true;
#else
    if (!Mk64Resource3DSMakeResident(SIZE_MAX, nullptr) ||
        !Mk64Resource3DSIsResident()) {
        return false;
    }
    const size_t baseline = Mk64Resource3DSLoadedCount();
    sFailNextResourceAllocations = 1;
    auto recovered = std::static_pointer_cast<Fast::Texture>(
        resourceManager.LoadResourceProcess(texture));
    const bool valid = sFailNextResourceAllocations == 0 && recovered != nullptr &&
        recovered->ImageData != nullptr && !Mk64Resource3DSIsResident() &&
        std::strcmp(Mk64Resource3DSResidencyStatus(), "released-for-memory") == 0 &&
        Mk64Resource3DSLoadedCount() == baseline + 1U;
    recovered.reset();
    return valid && Mk64Resource3DSLoadedCount() == baseline;
#endif
}

bool ValidateHotCacheAcrossTextureEviction(Ship::ResourceManager& manager, const char* texture) {
    if (Mk64Resource3DSMakeResident(1024, nullptr) ||
        !Mk64Resource3DSResidencyMemoryLimited()) return false;
    const size_t baseline = Mk64Resource3DSLoadedCount();
    auto first = std::static_pointer_cast<Fast::Texture>(manager.LoadResourceProcess(texture));
    if (first == nullptr || first->ImageDataSize == 0 ||
        Mk64Resource3DSLoadedCount() != baseline + 1) return false;
    const std::vector<uint8_t> expected(first->ImageData, first->ImageData + first->ImageDataSize);
    first.reset();
    if (Mk64Resource3DSLoadedCount() != baseline) return false;
    const auto reads = Mk64Resource3DSPhysicalReadCalls();
    for (unsigned pass = 0; pass < 1000; ++pass) {
        auto reloaded = std::static_pointer_cast<Fast::Texture>(manager.LoadResourceProcess(texture));
        if (reloaded == nullptr || reloaded->ImageDataSize != expected.size() ||
            std::memcmp(reloaded->ImageData, expected.data(), expected.size()) != 0) return false;
        reloaded.reset();
        if (Mk64Resource3DSLoadedCount() != baseline ||
            Mk64Resource3DSPhysicalReadCalls() != reads ||
            Mk64Resource3DSStreamingCachedBytes() > 2u*1024u*1024u) return false;
    }
    std::puts("PASS: 1000 actual ResourceManager texture evict/reload cycles, identical bytes, no decoded leaks and zero warm SD reads");
    return true;
}

bool ValidateBoundedKartWorkingSet(Ship::ResourceManager& resourceManager,
                                   const char* archivePath) {
    constexpr size_t kWindow = 64;
    constexpr size_t kLoads = 512;
    mk64_3ds::O2rArchiveReader archive(archivePath);
    if (archive.Open() != mk64_3ds::O2rReadResult::Ok) return false;

    const size_t baseline = Mk64Resource3DSLoadedCount();
    std::vector<std::shared_ptr<Ship::IResource>> owners;
    owners.reserve(kWindow + 1U);
    size_t loaded = 0;
    for (const std::string& path : archive.Entries()) {
        const std::string_view view(path);
        if (view.rfind("textures/karts/", 0) != 0 ||
            view.find("_frame") == std::string_view::npos) {
            continue;
        }
        auto texture = resourceManager.LoadResourceProcess(path.c_str());
        if (texture == nullptr) return false;
        owners.push_back(std::move(texture));
        if (owners.size() > kWindow) owners.erase(owners.begin());
        if (Mk64Resource3DSLoadedCount() > baseline + kWindow) return false;
        if (++loaded == kLoads) break;
    }
    if (loaded != kLoads) return false;
    owners.clear();
    return Mk64Resource3DSLoadedCount() == baseline;
}

bool ValidateDirectoryEviction(const char* vertex, const char* displayList,
                               const char* unrelated) {
    const size_t baseline = Mk64Resource3DSLoadedCount();
    const uint64_t vertexCrc = ResourceGetCrcByName(vertex);
    const uint64_t displayListCrc = ResourceGetCrcByName(displayList);
    void* vertexData = ResourceGetDataByName(vertex);
    void* displayListData = ResourceGetDataByName(displayList);
    void* unrelatedData = ResourceGetDataByName(unrelated);
    if (vertexData == nullptr || displayListData == nullptr || unrelatedData == nullptr ||
        Mk64Resource3DSLoadedCount() != baseline + 3U) {
        return false;
    }

    ResourceUnloadDirectory("models/ceremony");
    if (Mk64Resource3DSLoadedCount() != baseline + 3U ||
        ResourceGetDataByName(unrelated) != unrelatedData) {
        return false;
    }
    ResourceUnloadDirectory("__OTR__models/ceremony_data");
    if (Mk64Resource3DSLoadedCount() != baseline + 1U ||
        ResourceGetDataByName(unrelated) != unrelatedData) {
        return false;
    }
    if (ResourceGetDataByCrc(vertexCrc) == nullptr || ResourceGetDataByName(displayList) == nullptr ||
        Mk64Resource3DSLoadedCount() != baseline + 3U) {
        return false;
    }

    ResourceDirtyDirectory("models/ceremony_data/");
    if (Mk64Resource3DSLoadedCount() != baseline + 1U ||
        ResourceGetDataByName(vertex) == nullptr || ResourceGetDataByCrc(displayListCrc) == nullptr ||
        Mk64Resource3DSLoadedCount() != baseline + 3U) {
        return false;
    }

    CtlEntry* bank = GameEngine_LoadBank(0);
    AudioSequenceData* sequence = GameEngine_LoadSequence(0);
    void* sample = ResourceGetDataByName("sound/samples/sample_0");
    const size_t beforeSoundUnload = Mk64Resource3DSLoadedCount();
    ResourceUnloadDirectory("sound/");
    return bank != nullptr && sequence != nullptr && sample != nullptr &&
           GameEngine_LoadBank(0) == bank && GameEngine_LoadSequence(0) == sequence &&
           ResourceGetDataByName("sound/samples/sample_0") == sample &&
           Mk64Resource3DSLoadedCount() == beforeSoundUnload;
}

bool ValidateActiveTrackEviction(Ship::ResourceManager& resourceManager) {
    constexpr const char* luigiModel =
        "models/tracks/luigi_raceway/luigi_raceway_data/d_course_luigi_raceway_dl_0";
    constexpr const char* luigiTexture =
        "textures/tracks/luigi_raceway/luigi_raceway_data/d_course_luigi_raceway_sign_left";
    constexpr const char* mooModel =
        "models/tracks/moo_moo_farm/moo_moo_farm_data/d_course_moo_moo_farm_dl_0";
    constexpr const char* mooTexture =
        "textures/tracks/moo_moo_farm/moo_moo_farm_data/gTextureMole1";
    constexpr const char* royalModel =
        "models/tracks/royal_raceway/royal_raceway_data/d_course_royal_raceway_dl_0";
    constexpr const char* ceremonyModel = "models/ceremony_data/silver_trophy_dl";
    constexpr const char* commonModel = "models/startup_logo/dl1";

    Mk64Resource3DSSetActiveTrack("luigi_raceway");
    const size_t baseline = Mk64Resource3DSLoadedCount();
    void* common = ResourceGetDataByName(commonModel);
    if (common == nullptr) return false;

    for (size_t pass = 0; pass < 32; ++pass) {
        if (ResourceGetDataByName(luigiModel) == nullptr ||
            ResourceGetDataByName(luigiTexture) == nullptr ||
            ResourceGetDataByName(mooModel) == nullptr ||
            ResourceGetDataByName(mooTexture) == nullptr) {
            return false;
        }
        Mk64Resource3DSSetActiveTrack((pass & 1U) == 0 ? "luigi_raceway" : "moo_moo_farm");
        if (ResourceGetDataByName(commonModel) != common ||
            Mk64Resource3DSLoadedCount() > baseline + 3U) {
            return false;
        }
    }

    if (ResourceGetDataByName(luigiModel) == nullptr ||
        ResourceGetDataByName(royalModel) == nullptr ||
        ResourceGetDataByName(ceremonyModel) == nullptr) {
        return false;
    }
    Mk64Resource3DSSetActiveTrack("podium_ceremony");
    if (ResourceGetDataByName(royalModel) == nullptr ||
        ResourceGetDataByName(ceremonyModel) == nullptr ||
        ResourceGetDataByName(commonModel) != common) {
        return false;
    }
    const size_t podiumCount = Mk64Resource3DSLoadedCount();
    Mk64Resource3DSSetActiveTrack("luigi_raceway");
    if (Mk64Resource3DSLoadedCount() + 2U != podiumCount) {
        return false;
    }

    const size_t beforeOwnedTexture = Mk64Resource3DSLoadedCount();
    auto ownedMooTexture = std::static_pointer_cast<Fast::Texture>(
        resourceManager.LoadResourceProcess(mooTexture));
    if (ownedMooTexture == nullptr || ownedMooTexture->ImageData == nullptr ||
        Mk64Resource3DSLoadedCount() != beforeOwnedTexture + 1U) {
        return false;
    }
    const uint8_t firstByte = ownedMooTexture->ImageData[0];
    Mk64Resource3DSSetActiveTrack("luigi_raceway");
    if (Mk64Resource3DSLoadedCount() != beforeOwnedTexture + 1U ||
        ownedMooTexture->ImageData[0] != firstByte) {
        return false;
    }
    ownedMooTexture.reset();
    return Mk64Resource3DSLoadedCount() == beforeOwnedTexture;
}

bool ValidateRaceResourceReset(Ship::ResourceManager& manager, const char* archivePath) {
    Mk64Resource3DSReleaseRaceResources();
    Mk64RaceResourceStats3DS stats = {};
    Mk64Resource3DSGetRaceStats(&stats);
    if (stats.entries != 0 || stats.bytes != 0) return false;
    constexpr const char* logo = "models/startup_logo/dl1";
    constexpr const char* sample = "sound/samples/sample_0";
    const void* logoData = ResourceGetDataByName(logo);
    const void* sampleData = ResourceGetDataByName(sample);
    if (logoData == nullptr || sampleData == nullptr) return false;
    const size_t baseline = Mk64Resource3DSLoadedCount();
    mk64_3ds::O2rArchiveReader archive(archivePath);
    if (archive.Open() != mk64_3ds::O2rReadResult::Ok) return false;
    std::vector<std::string> kartFrames;
    std::string otherTexture;
    for (const auto& entry : archive.Entries()) {
        if (entry.starts_with("textures/karts/") && kartFrames.size() < 128)
            kartFrames.push_back(entry);
        if (otherTexture.empty() && entry.starts_with("textures/other_textures/"))
            otherTexture = entry;
    }
    if (kartFrames.size() != 128 || otherTexture.empty()) return false;
    constexpr const char* common = "textures/common_data/common_texture_item_box_question_mark";
    constexpr const char* model = "models/tracks/luigi_raceway/luigi_raceway_data/d_course_luigi_raceway_dl_0";
    for (size_t race = 0; race < 64; ++race) {
        for (const auto& frame : kartFrames)
            if (ResourceGetDataByName(frame.c_str()) == nullptr) return false;
        if (ResourceGetDataByName(common) == nullptr || ResourceGetDataByName(model) == nullptr ||
            ResourceGetDataByName(otherTexture.c_str()) == nullptr) return false;
        auto held = std::static_pointer_cast<Fast::Texture>(manager.LoadResourceProcess(kartFrames[0].c_str()));
        if (held == nullptr || held->ImageDataSize == 0) return false;
        const std::vector<uint8_t> original(held->ImageData, held->ImageData + held->ImageDataSize);
        Mk64Resource3DSGetRaceStats(&stats);
        if (stats.kartEntries != 128 || stats.rawEntries < 130 || stats.bytes == 0) return false;
        // Even a raw-borrowed kart is retired at this explicit scene boundary.
        // A shared owner still alive delays its free, never a dangling pointer.
        Mk64Resource3DSReleaseRaceResources();
        Mk64Resource3DSGetRaceStats(&stats);
        if (stats.entries != 1 || stats.kartEntries != 1 || stats.ownedEntries != 1 ||
            std::memcmp(original.data(), held->ImageData, original.size()) != 0) return false;
        held.reset();
        Mk64Resource3DSGetRaceStats(&stats);
        if (stats.entries != 0 || stats.bytes != 0 || stats.kartEntries != 0 ||
            Mk64Resource3DSLoadedCount() != baseline || ResourceGetDataByName(logo) != logoData ||
            ResourceGetDataByName(sample) != sampleData) return false;
        auto reloaded = std::static_pointer_cast<Fast::Texture>(manager.LoadResourceProcess(kartFrames[0].c_str()));
        if (reloaded == nullptr || reloaded->ImageDataSize != original.size() ||
            std::memcmp(original.data(), reloaded->ImageData, original.size()) != 0) return false;
        reloaded.reset();
        if (Mk64Resource3DSLoadedCount() != baseline) return false;
    }
    std::puts("PASS: 64 complete race resets, 8192 raw kart frames, deferred owners, identical reloads and pinned audio/menu");
    return true;
}
}

int main(int argc, char** argv) {
    if (argc != 2) {
        std::fprintf(stderr, "usage: %s <mk64.o2r>\n", argv[0]);
        return 2;
    }
    if (!Mk64Resource3DSInit(argv[1])) {
        std::fprintf(stderr, "failed to initialize resource runtime\n");
        return 1;
    }
    const bool resourceHeapValid = sResourceAllocationCalls != 0 && sLinearAllocationCalls == 0;

    constexpr const char* texture = "textures/common_data/common_texture_item_box_question_mark";
    constexpr const char* streamingTexture =
        "textures/karts/bowser_kart/bowser_kart_frame000_wheel0";
    constexpr const char* pressureRecoveryTexture =
        "textures/karts/mario_kart/mario_kart_frame088_wheel1";
    constexpr const char* trackTexture =
        "textures/tracks/luigi_raceway/luigi_raceway_data/d_course_luigi_raceway_sign_left";
    constexpr const char* vertex = "models/ceremony_data/ceremony_data_seg11_vtx_340";
    constexpr const char* displayList = "models/ceremony_data/silver_trophy_dl";
    constexpr const char* startupDisplayList = "models/startup_logo/dl1";
    constexpr const char* audioSample = "sound/samples/sample_0";
    constexpr uint32_t expectedSequenceCount = 36;
    Ship::ResourceManager resourceManager;
    const bool crcIndexValid = ValidateCrcIndex(argv[1]);
    const bool hotTextureCacheValid = ValidateHotCacheAcrossTextureEviction(resourceManager, texture);
    const bool residentPressureRecoveryValid =
        ValidateResidentPressureRecovery(resourceManager, pressureRecoveryTexture);
    const bool boundedKartWorkingSetValid =
        ValidateBoundedKartWorkingSet(resourceManager, argv[1]);
    const bool transientTextureValid = ValidateTransientTexture(resourceManager, streamingTexture);
    const bool textureOwnershipValid =
        ValidateTextureOwnership(resourceManager, trackTexture);
    const bool outOfMemoryValid =
        ValidateOutOfMemoryPropagation(resourceManager, streamingTexture);
    const bool directoryEvictionValid =
        ValidateDirectoryEviction(vertex, displayList, startupDisplayList);
    const bool activeTrackEvictionValid = ValidateActiveTrackEviction(resourceManager);
    const bool raceResourceResetValid = ValidateRaceResourceReset(resourceManager, argv[1]);
    std::printf("checks: heap=%d crc=%d resident=%d bounded=%d transient=%d ownership=%d oom=%d directory=%d track=%d race-reset=%d hot-cache=%d\n",
        resourceHeapValid, crcIndexValid, residentPressureRecoveryValid, boundedKartWorkingSetValid,
        transientTextureValid, textureOwnershipValid, outOfMemoryValid, directoryEvictionValid,
        activeTrackEvictionValid, raceResourceResetValid, hotTextureCacheValid);
    const uint64_t displayListCrc = ResourceGetCrcByName(displayList);
    auto* startupCommands = static_cast<Gfx*>(ResourceGetDataByName(startupDisplayList));
    const size_t startupCommandCount = ResourceGetSizeByName(startupDisplayList) / sizeof(Gfx);
    const bool startupDisplayListValid = ValidateStartupDisplayList(startupCommands, startupCommandCount);
    const bool ok = resourceHeapValid && crcIndexValid && hotTextureCacheValid && residentPressureRecoveryValid &&
                    boundedKartWorkingSetValid && transientTextureValid && textureOwnershipValid &&
                    outOfMemoryValid && directoryEvictionValid &&
                    activeTrackEvictionValid && raceResourceResetValid &&
                    resourceManager.GetArchiveManager()->HashToCString(displayListCrc) != nullptr &&
                    resourceManager.GetResourceRawPointer(displayListCrc) != nullptr &&
                    ResourceGetDataByName(texture) != nullptr && ResourceGetTexWidthByName(texture) > 0 &&
                    ResourceGetTexHeightByName(texture) > 0 && ResourceGetDataByName(vertex) != nullptr &&
                    ResourceGetSizeByName(vertex) > 0 && ResourceGetDataByName(displayList) != nullptr &&
                    ResourceGetSizeByName(displayList) > 0 && startupDisplayListValid &&
                    ResourceGetDataByName(audioSample) != nullptr &&
                    ResourceGetDataByCrc(displayListCrc) != nullptr &&
                    GameEngine_LoadBank(0) != nullptr && GameEngine_LoadSequence(0) != nullptr &&
                    GameEngine_GetSequenceCount() == expectedSequenceCount &&
                    sLoadingAudioServiceCalls != 0;

    std::printf("entries=%zu loaded=%zu result=%s\n", Mk64Resource3DSArchiveEntryCount(),
                Mk64Resource3DSLoadedCount(), ok ? "ok" : "failed");
    Mk64Resource3DSShutdown();
    return ok ? 0 : 1;
}
