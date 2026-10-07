#pragma once
#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>

namespace mk64_3ds {
struct OptionCursorVertex {
    int16_t x, y, z; uint16_t flag; int16_t u, v; uint8_t r, g, b, a;
};
static_assert(sizeof(OptionCursorVertex) == 16);
struct OptionCursorPoint { float x, y, z; uint8_t r, g, b, a; };
struct OptionCursorFace { std::array<OptionCursorPoint,3> points; float depth; };
// D_0D003090 uses common_vtx_itembox[8..31]: eight triangular faces,
// the very same colourful rotating solid used by the native retry cursor.
inline std::array<OptionCursorFace,8> OptionCursorFaces(
    const std::array<OptionCursorVertex,24>& vertices, double seconds,
    float scale, float x, float y) {
    constexpr double rad = 3.14159265358979323846/180;
    const float ax = std::fmod(seconds*135,360)*rad;
    const float ay = std::fmod(seconds*180,360)*rad;
    const float az = std::fmod(seconds*90,360)*rad;
    std::array<OptionCursorFace,8> faces{};
    for (unsigned i=0;i<vertices.size();++i) {
        const auto& v=vertices[i];
        float px=v.x*scale, py=v.y*scale, pz=v.z*scale;
        float nx=px*std::cos(ay)+pz*std::sin(ay), nz=-px*std::sin(ay)+pz*std::cos(ay);
        px=nx;pz=nz;
        nx=px*std::cos(az)-py*std::sin(az);float ny=px*std::sin(az)+py*std::cos(az);
        px=nx;py=ny;
        ny=py*std::cos(ax)-pz*std::sin(ax);nz=py*std::sin(ax)+pz*std::cos(ax);
        faces[i/3].points[i%3]={x+px,y-ny,nz,v.r,v.g,v.b,v.a};
        faces[i/3].depth+=nz/3;
    }
    std::stable_sort(faces.begin(),faces.end(),[](const auto& a,const auto& b){return a.depth<b.depth;});
    return faces;
}
} // namespace mk64_3ds
