#include "Camera.hpp"
#include "Input.hpp"
#include <spdlog/spdlog.h>

void Camera::ProcessInput()
{
    if (Input::MousePressed()) {
        glm::vec2 motion = Input::GetMouseMotion();
        yaw = glm::mod(yaw - motion.x * 0.1f, 360.0f);
        pitch = glm::clamp(pitch + motion.y * 0.1f, -89.9f, 89.9f);

        glm::mat4 rotation{ 1.0 };
        rotation *= glm::rotate(glm::radians(yaw), glm::vec3{ 0, 1, 0 });
        rotation *= glm::rotate(glm::radians(pitch), glm::vec3{ 1, 0, 0 });
        front = { rotation * glm::vec4{0, 0, -1, 1} };

        dirty = true;
    }

    glm::vec3 forward = glm::normalize(glm::vec3{ front.x, 0, front.z });
    glm::vec3 right = glm::normalize(glm::cross(-glm::vec3{ 0, 1, 0 }, forward));

    if (Input::KeyPressed(Key::W)) {
        position += forward * 0.2f;
        dirty = true;
    }
    if (Input::KeyPressed(Key::S)) {
        position -= forward * 0.2f;
        dirty = true;
    }
    if (Input::KeyPressed(Key::D)) {
        position += right * 0.1f;
        dirty = true;
    }
    if (Input::KeyPressed(Key::A)) {
        position -= right * 0.1f;
        dirty = true;
    }
    if (Input::KeyPressed(Key::Space)) {
        position.y -= 0.05f;
        dirty = true;
    }

    spdlog::info("pos: {}", glm::to_string(position));
    spdlog::info("yaw: {}", yaw);
}

glm::mat4 Camera::GetView() const
{
    return glm::lookAt(position, position + front, { 0.0f, 1.0f, 0.0f });
}

glm::mat4 Camera::GetProj() const
{
    return glm::perspective(glm::radians(45.0f), aspect, 0.01f, 10000.0f);
}

glm::vec3 Camera::GetRight() const
{
    glm::vec3 right = glm::normalize(glm::cross(-glm::vec3{ 0, 1, 0 }, front));
    return right;
}

void Camera::SetViewSize(int width, int height)
{
    aspect = float(width) / height;
}

void Camera::SetPosition(glm::vec3 pos)
{
    position = pos;
}

void Camera::SetYaw(float yaw)
{
    this->yaw = yaw;
    glm::mat4 rotation{ 1.0 };
    rotation *= glm::rotate(glm::radians(yaw), glm::vec3{ 0, 1, 0 });
    rotation *= glm::rotate(glm::radians(pitch), glm::vec3{ 1, 0, 0 });
    front = { rotation * glm::vec4{ 0, 0, -1, 1 } };
}

bool Camera::CheckDirtyAndClean()
{
    if (dirty) {
        dirty = false;
        return true;
    }
    return false;
}
