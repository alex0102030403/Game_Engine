#include <gtc/matrix_transform.hpp>
#include "Camera.h"
#include "Input.h"
#include "Shader.h"

Camera::Camera()
{
    m_view = glm::mat4(1.0f);
    m_proj = glm::mat4(1.0f);

    m_position = glm::vec3(0.0f, 0.0f, 3.0f);
    m_direction = glm::vec3(0.0f, 0.0f, -1.0f);
    m_up = glm::vec3(0.0f, 1.0f, 0.0f);

    m_yaw = -90.0f; // Facing along the negative Z-axis
    m_pitch = 0.0f;
}

void Camera::Set3DView()
{
    GLfloat FOV = 45.0f;
    GLfloat aspectRatio = 1280.0f / 720.0f;

    m_proj = glm::perspective(glm::radians(FOV), aspectRatio, 0.001f, 1000.0f);
}

void Camera::Update()
{
    const float cameraSpeed = 0.05f; // Adjust as needed
    const float mouseSensitivity = 0.1f;

    // Handle keyboard input
    if (Input::Instance()->IsKeyPressed())
    {
        if (Input::Instance()->GetKeyDown() == 'a')
        {
            m_position -= glm::normalize(glm::cross(m_direction, m_up)) * cameraSpeed;
        }
        else if (Input::Instance()->GetKeyDown() == 'd')
        {
            m_position += glm::normalize(glm::cross(m_direction, m_up)) * cameraSpeed;
        }
        else if (Input::Instance()->GetKeyDown() == 'w')
        {
            m_position += m_direction * cameraSpeed;
        }
        else if (Input::Instance()->GetKeyDown() == 's')
        {
            m_position -= m_direction * cameraSpeed;
        }
        else if (Input::Instance()->GetKeyDown() == 'q')
        {
            m_position.y += cameraSpeed;
        }
        else if (Input::Instance()->GetKeyDown() == 'e')
        {
            m_position.y -= cameraSpeed;
        }
    }

    // Handle mouse movement
    int deltaX = Input::Instance()->GetMouseMotionX();
    int deltaY = Input::Instance()->GetMouseMotionY();

    m_yaw += deltaX * mouseSensitivity;
    m_pitch -= deltaY * mouseSensitivity; // Invert y-axis if necessary

    // Constrain the pitch angle
    if (m_pitch > 89.0f)
        m_pitch = 89.0f;
    if (m_pitch < -89.0f)
        m_pitch = -89.0f;

    // Update direction vector
    glm::vec3 front;
    front.x = cos(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
    front.y = sin(glm::radians(m_pitch));
    front.z = sin(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
    m_direction = glm::normalize(front);

    // Update the view matrix
    m_view = glm::lookAt(m_position, m_position + m_direction, m_up);

    // Reset mouse deltas
    
}

void Camera::SendToShader(const Shader& shader)
{
    shader.SendUniformData("proj", m_proj);
    shader.SendUniformData("view", m_view);
    shader.SendUniformData("cameraPosition", m_position.x, m_position.y, m_position.z);
}

glm::vec3 Camera::GetPosition() const
{
	return m_position;
}
