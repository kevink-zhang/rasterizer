#include "rasterizer.h"
#include <array>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>
#include <iostream>
#include <QDebug>
#include "camera.h"

#define WIDTH 512.f
#define HEIGHT 512.f

Rasterizer::Rasterizer(const std::vector<Polygon>& polygons)
    : m_polygons(polygons), CAMERA(Camera())
{}

QImage Rasterizer::RenderScene()
{
    QImage result(512, 512, QImage::Format_RGB32);
    // Fill the image with black pixels.
    // Note that qRgb creates a QColor,
    // and takes in values [0, 255] rather than [0, 1].
    result.fill(qRgb(0.f, 0.f, 0.f));
    // TODO: Complete the various components of code that make up this function.
    // It should return the rasterized image of the current scene.

    // Make liberal use of helper functions; writing your rasterizer as one
    // long RenderScene function will make it (a) hard to debug and
    // (b) hard to write without copy-pasting. Also, Adam will be sad when
    // he reads your code.

    // Also! As per the style requirements for this assignment, make sure you
    // use std::arrays to store things like your line segments, Triangles, and
    // vertex coordinates. This lets you easily use loops to perform operations
    // on your scene components, rather than copy-pasting operations three times
    // each!

    //z indexing
    std::array<float, (int) WIDTH* (int) HEIGHT>  zIndex;

    for(int i = 0; i < zIndex.size(); i++) zIndex[i] = 1000000;

    //store camera matrices
    glm::mat4 perspMat = CAMERA.getPerspectiveMat();
    glm::mat4 viewMat =  CAMERA.getViewMat();

    for(const Polygon &p: m_polygons) {
        //draws the triangles
        for(const Triangle &t: p.m_tris) {
            //space computations
            float z1 = (viewMat * p.VertAt(t.m_indices[0]).m_pos)[2];
            float z2 = (viewMat * p.VertAt(t.m_indices[1]).m_pos)[2];
            float z3 = (viewMat * p.VertAt(t.m_indices[2]).m_pos)[2];
            glm::vec4 pt1 = perspMat * viewMat * p.VertAt(t.m_indices[0]).m_pos;
            glm::vec4 pt2 = perspMat * viewMat * p.VertAt(t.m_indices[1]).m_pos;
            glm::vec4 pt3 = perspMat * viewMat * p.VertAt(t.m_indices[2]).m_pos;
            pt1 /= pt1[3];
            pt2 /= pt2[3];
            pt3 /= pt3[3];
            glm::vec2 p1 ((pt1[0]+1.f)*WIDTH/2.f, (1.f-pt1[1])*HEIGHT/2.f);
            glm::vec2 p2 ((pt2[0]+1.f)*WIDTH/2.f, (1.f-pt2[1])*HEIGHT/2.f);
            glm::vec2 p3 ((pt3[0]+1.f)*WIDTH/2.f, (1.f-pt3[1])*HEIGHT/2.f);
            std::array<Segment, 3> a = {Segment(p1, p2),
                                        Segment(p2, p3),
                                        Segment(p3, p1)};
            glm::vec2 pos1(0, 0);
            glm::vec2 pos2(0, 0);
            p.computeBoundingBox(p1, p2, p3, pos1, pos2);
            for(int y = (std::max(0.f, pos1[1])); y < std::ceil(std::min(HEIGHT, pos2[1])); y++){
                float xmin = 1000000;
                float xmax = -1;
                for(const Segment &s: a) {
                    float xat;
                    if(s.getIntersection(y, &xat)) {
                        xmin = std::min(xmin, xat);
                        xmax = std::max(xmax, xat);
                    }
                }
                xmin = std::max(xmin, 0.f);
                xmax = std::min(WIDTH, xmax);
                for(int x = std::ceil(xmin); x < (xmax); x++) {
                    //shade computations
                    float shade = glm::clamp(glm::dot(p.computeNormal(t, p1, p2, p3, glm::vec2(x, y)), -CAMERA.forward), 0.f, 1.f);
                    //drawing
                    glm::vec2 uv = p.computeUV(t, p1, p2, p3, glm::vec2(x,y));
                    glm::vec3 color = GetImageColor(uv, p.mp_texture) * shade;
                    float z = ZInterpolation(p1, p2, p3, z1, z2, z3, glm::vec2(x, y));
                    if(z < zIndex[x+y*WIDTH]) {
                         zIndex[x + y*WIDTH] = z;
                         result.setPixel(x, y, qRgb(glm::clamp(color[0], 0.f, 255.f), glm::clamp(color[1], 0.f, 255.f), glm::clamp(color[2], 0.f, 255.f)));
                    }
//                        glm::vec4 color = p.computeColor(t, glm::vec2(x, y));
//                        //if z-index is closer to camera, override the previous
//                        if(color[3] < zIndex[x + y*WIDTH]){
//                            zIndex[x + y*WIDTH] = color[3];
//                            result.setPixel(x, y, qRgb(glm::clamp(color[0], 0.f, 255.f), glm::clamp(color[1], 0.f, 255.f), glm::clamp(color[2], 0.f, 255.f)));
//                        }
                }

            }
        }
    }
    return result;
}

void Rasterizer::ClearScene()
{
    m_polygons.clear();
}
