#pragma once
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

class Camera
{
public:
    Camera() = default;
    Camera(uint32_t width, uint32_t height);
    void ProcessInput();
    bool CheckDirtyAndClean();
    glm::mat4 GetView() const;
    glm::mat4 GetProj() const;
    glm::mat4 GetInvView() const { return glm::inverse(GetView()); }
    glm::mat4 GetInvProj() const { return glm::inverse(GetProj()); }

private:
    glm::vec3 position = { 0.0f, 0.0f, 5.0f };
    glm::vec3 front = { 0.0f, 0.0f, -1.0f };
    float aspect = 1.0f;
    float yaw = 0.0f;
    float pitch = 0.0f;
    bool dirty = false;
};
