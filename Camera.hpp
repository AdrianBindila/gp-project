#ifndef Camera_hpp
#define Camera_hpp

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

#include <string>

namespace gps {

    enum MOVE_DIRECTION {
        MOVE_FORWARD, MOVE_BACKWARD, MOVE_RIGHT, MOVE_LEFT
    };

    class Camera {
    public:
        //Camera constructor
        Camera(glm::vec3 cameraPosition, glm::vec3 cameraTarget, glm::vec3 cameraUp);

        //return the view matrix, using the glm::lookAt() function
        glm::mat4 getViewMatrix();

        //update the camera internal parameters following a camera move event
        void move(MOVE_DIRECTION direction, float speed);

        //update the camera internal parameters following a camera rotate event
        //yaw - camera rotation around the y axis
        //pitch - camera rotation around the x axis
        void rotate(float pitch, float yaw);

    private:
        glm::vec3 cameraPosition; //coordinates of the camera
        glm::vec3 cameraTarget; //coordinates camera looks at
        glm::vec3 cameraFrontDirection; //coordinates of z-axis
        glm::vec3 cameraRightDirection; //x-axis
        glm::vec3 cameraUpDirection;    //y-axis
        float pitch = -90;
        float yaw;
    };

}

#endif /* Camera_hpp */
