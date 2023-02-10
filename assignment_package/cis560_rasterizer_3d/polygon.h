#pragma once
#include <glm/glm.hpp>
#include <vector>
#include <QString>
#include <QImage>
#include <QColor>

// A Vertex is a point in space that defines one corner of a polygon.
// Each Vertex has several attributes that determine how they contribute to the
// appearance of their Polygon, such as coloration.
struct Vertex
{
    glm::vec4 m_pos;    // The position of the vertex. In hw02, this is in pixel space.
    glm::vec3 m_color;  // The color of the vertex. X corresponds to Red, Y corresponds to Green, and Z corresponds to Blue.
    glm::vec4 m_normal; // The surface normal of the vertex (not yet used)
    glm::vec2 m_uv;     // The texture coordinates of the vertex (not yet used)

    Vertex(glm::vec4 p, glm::vec3 c, glm::vec4 n, glm::vec2 u)
        : m_pos(p), m_color(c), m_normal(n), m_uv(u)
    {}
};

// Each Polygon can be decomposed into triangles that fill its area.
struct Triangle
{
    // The indices of the Vertices that make up this triangle.
    // The indices correspond to the std::vector of Vertices stored in the Polygon
    // which stores this Triangle
    unsigned int m_indices[3];
};

struct Segment{
    glm::vec2 pt1;
    glm::vec2 pt2;

    Segment(glm::vec2 e1, glm::vec2 e2)
        : pt1(e1), pt2(e2){

    }
    bool getIntersection(int y, float* x) const {
        if(pt1[1] < pt2[1]){
            if(y >= pt1[1] && y <= pt2[1]){
                if (pt2[1]-pt1[1] == 0) *x = pt1[0];
                else *x = pt1[0] + (pt2[0]-pt1[0]) * (y-pt1[1]) / (pt2[1]-pt1[1]);
                return true;
            }
            else {
                return false;
            }
        }
        else {
            if(y >= pt2[1] && y <= pt1[1]){
                if (pt2[1]-pt1[1] == 0) *x = pt1[0];
                else *x = pt2[0] + (pt1[0]-pt2[0]) * (y-pt2[1]) / (pt1[1]-pt2[1]);
                return true;
            }
            else {
                return false;
            }
        }
    }
};

class Polygon
{
public:
    // TODO: Populate this list of triangles in Triangulate()
    std::vector<Triangle> m_tris;
    // The list of Vertices that define this polygon. This is already filled by the Polygon constructor.
    std::vector<Vertex> m_verts;
    // The name of this polygon, primarily to help you debug
    QString m_name;
    // The image that can be read to determine pixel color when used in conjunction with UV coordinates
    // Not used until homework 3.
    QImage* mp_texture;
    // The image that can be read to determine surface normal offset when used in conjunction with UV coordinates
    // Not used until homework 3
    QImage* mp_normalMap;

    // Polygon class constructors
    Polygon(const QString& name, const std::vector<glm::vec4>& pos, const std::vector<glm::vec3> &col);
    Polygon(const QString& name, int sides, glm::vec3 color, glm::vec4 pos, float rot, glm::vec4 scale);
    Polygon(const QString& name);
    Polygon();
    Polygon(const Polygon& p);
    ~Polygon();

    // TODO: Complete the body of Triangulate() in polygon.cpp
    // Creates a set of triangles that, when combined, fill the area of this convex polygon.
    void Triangulate();

    // Copies the input QImage into this Polygon's texture
    void SetTexture(QImage*);

    // Copies the input QImage into this Polygon's normal map
    void SetNormalMap(QImage*);

    // Various getter, setter, and adder functions
    void AddVertex(const Vertex&);
    void AddTriangle(const Triangle&);
    void ClearTriangles();

    Triangle& TriAt(unsigned int);
    Triangle TriAt(unsigned int) const;

    Vertex& VertAt(unsigned int);
    Vertex VertAt(unsigned int) const;

    //for bounding box
    void computeBoundingBox(glm::vec2 &pt1,glm::vec2 &pt2,glm::vec2 &pt3, glm::vec2 &pos, glm::vec2 &dim) const;
    //for color
    glm::vec4 computeColor(const Triangle &t, glm::vec2 pos) const;
    glm::vec2 computeUV(const Triangle &t, glm::vec2 p1, glm::vec2 p2, glm::vec2 p3, glm::vec2 pos) const;
    glm::vec4 computeNormal(const Triangle &t, glm::vec2 p1, glm::vec2 p2, glm::vec2 p3, glm::vec2 pos) const;
};

// Returns the color of the pixel in the image at the specified texture coordinates.
// Returns white if the image is a null pointer
glm::vec3 GetImageColor(const glm::vec2 &uv_coord, const QImage* const image);


//does z-interpolation
float ZInterpolation(glm::vec2 p1, glm::vec2 p2, glm::vec2 p3, float z1, float z2, float z3, glm::vec2 pos);
