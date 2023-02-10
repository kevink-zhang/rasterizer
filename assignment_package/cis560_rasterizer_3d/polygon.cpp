#include "polygon.h"
#include <glm/gtx/transform.hpp>
#include <QDebug>

#define WIDTH 512.f
#define HEIGHT 512.f

float triArea(const Vertex& v1, const Vertex& v2, const Vertex& v3) {
     return std::abs(v1.m_pos[0]*(v2.m_pos[1]-v3.m_pos[1]) +
            v2.m_pos[0]*(v3.m_pos[1]-v1.m_pos[1]) +
            v3.m_pos[0]*(v1.m_pos[1]-v2.m_pos[1]))/2.f;
}
float triArea(glm::vec2 p1, glm::vec2 p2, glm::vec2 p3) {
    return std::abs(p1[0]*(p2[1]-p3[1]) +
                    p2[0]*(p3[1]-p1[1]) +
                    p3[0]*(p1[1]-p2[1]))/2.f;
}
void Polygon::computeBoundingBox(glm::vec2 &pt1,glm::vec2 &pt2,glm::vec2 &pt3, glm::vec2 &pos1, glm::vec2 &pos2) const{
    float minX = WIDTH;
    float minY = HEIGHT;
    float maxX = 0;
    float maxY = 0;
    for(glm::vec2 v : {pt1, pt2, pt3}){
        if(v[0] < minX){
            minX = v[0];
        }
        if(v[1] < minY){
            minY = v[1];
        }
        if(v[0] > maxX){
            maxX = v[0];
        }
        if(v[1] > maxY){
            maxY = v[1];
        }
    }
    pos1[0] = std::max(0.f, minX);
    pos1[1] = std::max(0.f, minY);
    pos2[0] = std::min(WIDTH, maxX);
    pos2[1] = std::min(HEIGHT, maxY);
}

glm::vec4 Polygon::computeColor(const Triangle &t, glm::vec2 pos) const {
    glm::vec3 color (0.f, 0.f, 0.f);
    float z = 0.f;
    for(int i = 0; i < 3; i++) {
        const Vertex& v1 = m_verts[t.m_indices[i]];
        const Vertex& v2 = m_verts[t.m_indices[(i+1)%3]];
        const Vertex& v3 = m_verts[t.m_indices[(i+2)%3]];
        float subarea = std::abs(v1.m_pos[0]*(v2.m_pos[1]-pos[1]) +
                v2.m_pos[0]*(pos[1]-v1.m_pos[1]) +
                pos[0]*(v1.m_pos[1]-v2.m_pos[1]))/2.f;

        color += v3.m_color*subarea/triArea(v1, v2, v3);
        z += subarea/triArea(v1, v2, v3)/v3.m_pos[2];
    }
    return glm::vec4 (color[0], color[1], color[2], 1.f/z);
}

glm::vec2 Polygon::computeUV(const Triangle &t, glm::vec2 p1, glm::vec2 p2, glm::vec2 p3, glm::vec2 pos) const {
    glm::vec2 uv (0.f, 0.f);
    glm::vec2 foo[3] = {p1, p2, p3};
    for(int i = 0; i < 3; i++) {
        glm::vec2 v1 = foo[i];
        glm::vec2 v2 = foo[(i+1)%3];
        glm::vec2 v3 = foo[(i+2)%3];
        float subarea = triArea(v1, v2, pos);

        uv += m_verts[t.m_indices[(i+2)%3]].m_uv*subarea/triArea(v1, v2, v3);
    }
    return uv;
}
glm::vec4 Polygon::computeNormal(const Triangle &t, glm::vec2 p1, glm::vec2 p2, glm::vec2 p3, glm::vec2 pos) const{
    glm::vec4 ret (0.f,0.f,0.f,0.f);
    glm::vec2 foo[3] = {p1, p2, p3};
    for(int i = 0; i < 3; i++) {
        glm::vec2 v1 = foo[i];
        glm::vec2 v2 = foo[(i+1)%3];
        glm::vec2 v3 = foo[(i+2)%3];
        float subarea = triArea(v1, v2, pos);

        ret += m_verts[t.m_indices[(i+2)%3]].m_normal*subarea/triArea(v1, v2, v3);
    }
    return ret;
}


float ZInterpolation(glm::vec2 p1, glm::vec2 p2, glm::vec2 p3, float z1, float z2, float z3, glm::vec2 pos) {
    float ret = 0.f;
    glm::vec2 foo[3] = {p1, p2, p3};
    float foo2[3] = {z1, z2, z3};
    for(int i = 0; i < 3; i++) {
        glm::vec2 v1 = foo[i];
        glm::vec2 v2 = foo[(i+1)%3];
        glm::vec2 v3 = foo[(i+2)%3];
        float subarea = triArea(v1, v2, pos);

        ret += subarea/triArea(v1, v2, v3)/foo2[(i+2)%3];
    }
    return 100.f/ret;
}


void Polygon::Triangulate()
{
    for(int i = 1; i < m_verts.size()-1; i++) {
        Triangle t;
        t.m_indices[0] = 0;
        t.m_indices[1] = i;
        t.m_indices[2] = i+1;

        AddTriangle(t);
    }
}

glm::vec3 GetImageColor(const glm::vec2 &uv_coord, const QImage* const image)
{
    if(image)
    {
        int X = glm::min(image->width() * uv_coord.x, image->width() - 1.0f);
        int Y = glm::min(image->height() * (1.0f - uv_coord.y), image->height() - 1.0f);
        QColor color = image->pixel(X, Y);
        return glm::vec3(color.red(), color.green(), color.blue());
    }
    return glm::vec3(255.f, 255.f, 255.f);
}


// Creates a polygon from the input list of vertex positions and colors
Polygon::Polygon(const QString& name, const std::vector<glm::vec4>& pos, const std::vector<glm::vec3>& col)
    : m_tris(), m_verts(), m_name(name), mp_texture(nullptr), mp_normalMap(nullptr)
{
    for(unsigned int i = 0; i < pos.size(); i++)
    {
        m_verts.push_back(Vertex(pos[i], col[i], glm::vec4(), glm::vec2()));
    }
    Triangulate();
}

// Creates a regular polygon with a number of sides indicated by the "sides" input integer.
// All of its vertices are of color "color", and the polygon is centered at "pos".
// It is rotated about its center by "rot" degrees, and is scaled from its center by "scale" units
Polygon::Polygon(const QString& name, int sides, glm::vec3 color, glm::vec4 pos, float rot, glm::vec4 scale)
    : m_tris(), m_verts(), m_name(name), mp_texture(nullptr), mp_normalMap(nullptr)
{
    glm::vec4 v(0.f, 1.f, 0.f, 1.f);
    float angle = 360.f / sides;
    for(int i = 0; i < sides; i++)
    {
        glm::vec4 vert_pos = glm::translate(glm::vec3(pos.x, pos.y, pos.z))
                           * glm::rotate(rot, glm::vec3(0.f, 0.f, 1.f))
                           * glm::scale(glm::vec3(scale.x, scale.y, scale.z))
                           * glm::rotate(i * angle, glm::vec3(0.f, 0.f, 1.f))
                           * v;
        m_verts.push_back(Vertex(vert_pos, color, glm::vec4(), glm::vec2()));
    }

    Triangulate();
}

Polygon::Polygon(const QString &name)
    : m_tris(), m_verts(), m_name(name), mp_texture(nullptr), mp_normalMap(nullptr)
{}

Polygon::Polygon()
    : m_tris(), m_verts(), m_name("Polygon"), mp_texture(nullptr), mp_normalMap(nullptr)
{}

Polygon::Polygon(const Polygon& p)
    : m_tris(p.m_tris), m_verts(p.m_verts), m_name(p.m_name), mp_texture(nullptr), mp_normalMap(nullptr)
{
    if(p.mp_texture != nullptr)
    {
        mp_texture = new QImage(*p.mp_texture);
    }
    if(p.mp_normalMap != nullptr)
    {
        mp_normalMap = new QImage(*p.mp_normalMap);
    }
}

Polygon::~Polygon()
{
    delete mp_texture;
}

void Polygon::SetTexture(QImage* i)
{
    mp_texture = i;
}

void Polygon::SetNormalMap(QImage* i)
{
    mp_normalMap = i;
}

void Polygon::AddTriangle(const Triangle& t)
{
    m_tris.push_back(t);
}

void Polygon::AddVertex(const Vertex& v)
{
    m_verts.push_back(v);
}

void Polygon::ClearTriangles()
{
    m_tris.clear();
}

Triangle& Polygon::TriAt(unsigned int i)
{
    return m_tris[i];
}

Triangle Polygon::TriAt(unsigned int i) const
{
    return m_tris[i];
}

Vertex &Polygon::VertAt(unsigned int i)
{
    return m_verts[i];
}

Vertex Polygon::VertAt(unsigned int i) const
{
    return m_verts[i];
}
