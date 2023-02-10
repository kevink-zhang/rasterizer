#pragma once
#include <glm/glm.hpp>
#include <vector>
#include <QString>
#include <QImage>
#include <QColor>

class Camera{
public:
    glm::vec4 forward;
    glm::vec4 right;
    glm::vec4 up;
    float FOV;
    glm::vec4 pos;
    float near;
    float far;
    float aspect;

    Camera();
    glm::mat4 getViewMat();
    glm::mat4 getPerspectiveMat();
    void translateX(float dx);
    void translateY(float dy);
    void translateZ(float dz);
    void rotateRight(float dx);
    void rotateUp(float dy);
    void rotateForward(float dz);
};
