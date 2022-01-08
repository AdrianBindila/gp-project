#include <GL/glew.h>
#include "Camera.hpp"

namespace gps {

    //Camera constructor

    Camera::Camera(glm::vec3 cameraPosition, glm::vec3 cameraTarget, glm::vec3 cameraUp) {
        this->cameraPosition = cameraPosition;
        this->cameraTarget = cameraTarget;
        this->cameraFrontDirection = glm::normalize(this->cameraPosition - this->cameraTarget);
        this->cameraRightDirection = glm::normalize(glm::cross(cameraUp, cameraFrontDirection));
        this->cameraUpDirection = glm::cross(this->cameraFrontDirection, this->cameraRightDirection);
        this->pitch = 0;
        this->yaw = 0;
        this->cameraUp = cameraUp;
        rotate(pitch, yaw);
    }

    //return the view matrix, using the glm::lookAt() function
    glm::mat4 Camera::getViewMatrix() {
        return glm::lookAt(cameraPosition, cameraPosition + cameraFrontDirection, cameraUpDirection);
    }

    //update the camera internal parameters following a camera move event
    void Camera::move(MOVE_DIRECTION direction, float speed) {
        switch (direction) {
            case MOVE_FORWARD:
                cameraPosition += speed * cameraFrontDirection;
                break;
            case MOVE_BACKWARD:
                cameraPosition -= speed * cameraFrontDirection;
                break;
            case MOVE_LEFT:
                cameraPosition -= speed * cameraRightDirection;
                break;
            case MOVE_RIGHT:
                cameraPosition += speed * cameraRightDirection;
                break;
            case MOVE_UPWARD:
                cameraPosition += speed * cameraUp;
                break;
            case MOVE_DOWNWARD:
                cameraPosition -= speed * cameraUp;
                break;
        }
    }

    //update the camera internal parameters following a camera rotate event
    //yaw - camera rotation around the y axis
    //pitch - camera rotation around the x axis
    void Camera::rotate(float pitch, float yaw) {//pitch and yaw should be in radians
        this->pitch += pitch;
        this->yaw += yaw;

        if (this->pitch > 89.0f) {
            this->pitch = 89.0f;
        }
        if (this->pitch < -89.0f) {
            this->pitch = -89.0f;
        }
//        printf("%f %f\n", this->pitch, this->yaw);
        cameraFrontDirection.x = cos(glm::radians(this->yaw)) * cos(glm::radians(this->pitch));
        cameraFrontDirection.y = sin(glm::radians(this->pitch));
        cameraFrontDirection.z = sin(glm::radians(this->yaw)) * cos(glm::radians(this->pitch));

        cameraFrontDirection = glm::normalize(cameraFrontDirection);
//        printf("x:%f ; y:%f ; z:%f\n", cameraFrontDirection.x, cameraFrontDirection.y, cameraFrontDirection.z);
        cameraRightDirection = glm::normalize(glm::cross(cameraFrontDirection, glm::vec3(0, 1, 0)));
        cameraUpDirection = glm::normalize(glm::cross(cameraRightDirection, cameraFrontDirection));

    }
}