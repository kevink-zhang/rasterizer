#include "camera.h"
#include <array>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>
#include <iostream>
#include <QDebug>
#include <glm/gtx/rotate_vector.hpp>

Camera::Camera()
    : forward(glm::vec4(0.f, 0.f, -1.f, 0.f)),
      right(glm::vec4(1.f, 0.f, 0.f, 0.f)),
      up(glm::vec4(0.f, 1.f, 0.f, 0.f)),
      FOV(45.f),
      pos(glm::vec4(0.f, 0.f, 10.f, 1.f)),
      near(0.01f),
      far(100.f),
      aspect(1.f){
}

glm::mat4 Camera::getViewMat() {
    return   glm::mat4(glm::vec4(right[0], up[0], forward[0], 0),
                glm::vec4(right[1], up[1], forward[1], 0),
                glm::vec4(right[2], up[2], forward[2], 0),
                glm::vec4(0, 0, 0, 1)) *
                glm::mat4(glm::vec4(1.f, 0, 0, 0),
                                 glm::vec4(0, 1.f, 0, 0),
                                 glm::vec4(0, 0, 1.f, 0),
                                 glm::vec4(-pos[0], -pos[1], -pos[2], 1.f))
   ;
}
glm::mat4 Camera::getPerspectiveMat() {
    float itan = 1/(tan(glm::radians(FOV/2.f)));
    return glm::mat4(glm::vec4(itan/aspect, 0, 0, 0),
                    glm::vec4(0, itan, 0, 0),
                    glm::vec4(0, 0, far/(far-near), 1.f),
                    glm::vec4(0, 0, -far*near/(far-near), 0));
}

void Camera::translateX(float dx) {
    pos[0] += dx;
}

void Camera::translateY(float dy) {
    pos[1] += dy;
}

void Camera::translateZ(float dz) {
    pos[2] += dz;
}

void Camera::rotateRight(float a) {
    up = glm::rotate(glm::radians(a), glm::vec3(right)) * up;
    forward = glm::rotate(glm::radians(a), glm::vec3(right)) * forward;
}
void Camera::rotateUp(float a) {
    right = glm::rotate(glm::radians(a), glm::vec3(up)) * right;
    forward = glm::rotate(glm::radians(a), glm::vec3(up)) * forward;
}
void Camera::rotateForward(float a) {
    up = glm::rotate(glm::radians(a), glm::vec3(forward)) * up;
    right = glm::rotate(glm::radians(a), glm::vec3(forward)) * right;
}
