#include "camera.hpp"
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>
// #include <glm/gtx/rotate_vector.hpp>

#include <iostream>

#define LOOK_VEC_THRESHHOLD 0.9

void Camera::reorient(double newX, double newY)
{
	static double x = newX;
	static double y = newY;

	float deltaX = (newX - x)*0.01;
	float deltaY = (newY - y)*0.01;

	glm::vec3 lookVec = glm::normalize(_lookingAt - _location);

	glm::vec3 perpendicular = glm::cross(lookVec, _upDirection);
	glm::mat3 parallelRotation = glm::mat3(glm::rotate(-deltaX, _upDirection));
	
	// Looking direction and up direction cannot be the same
	if (glm::dot(lookVec, _upDirection) > LOOK_VEC_THRESHHOLD)
		deltaY = deltaY > 0 ? deltaY : 0;
	else if (glm::dot(lookVec, _upDirection) < -LOOK_VEC_THRESHHOLD)
		deltaY = deltaY < 0 ? deltaY : 0;

	glm::mat3 perpendicularRotation = glm::mat3(glm::rotate(-deltaY, perpendicular));

	_lookingAt = parallelRotation*perpendicularRotation*lookVec + _location;

	x = newX;
	y = newY;
}

bool Camera::lookVecTooCloseToUpDirection(glm::vec3 lookVec)
{
	glm::vec3 nLookVec = glm::normalize(lookVec);
	float dotProduct = glm::dot(nLookVec, _upDirection);
	return dotProduct > 0.9 || dotProduct < -0.9;
}

void Camera::takeStep(glm::vec3 directionRelativeToCamera)
{
	static const float stepLength = 0.2f;

	glm::vec3 forward = _location - _lookingAt;
	forward = forward - glm::dot(forward, _upDirection)*_upDirection; // Limit movement to a plane

	glm::vec3 perpendicular = glm::cross(forward, _upDirection);

	glm::vec3 sideTranslation = -stepLength*directionRelativeToCamera.x*glm::normalize(perpendicular);
	glm::vec3 heightTranslation = stepLength*directionRelativeToCamera.y*_upDirection;
	glm::vec3 depthTranslation = -stepLength*directionRelativeToCamera.z*glm::normalize(forward);
	glm::vec3 translation = sideTranslation + depthTranslation + heightTranslation;

	_location = translation + _location;
	_lookingAt = translation + _lookingAt;
}


glm::mat4 Camera::WTVMatrix()
{
	return glm::lookAt(_location, _lookingAt, _upDirection);
}

void Camera::pollWindowForCameraMovement(GLFWwindow* window)
{
	// Keys
	static glm::vec3 step(0, 0, 0);
	char stepTaken = 0;
	if (glfwGetKey(window, GLFW_KEY_W))
	{
		stepTaken = 1;
		step.z = 1;
	} 
	else if (glfwGetKey(window, GLFW_KEY_S))
	{
		stepTaken = 1;
		step.z = -1;
	}	
	if (glfwGetKey(window, GLFW_KEY_D))
	{
		stepTaken = 1;
		step.x = 1;
	} 
	else if (glfwGetKey(window, GLFW_KEY_A))
	{
		stepTaken = 1;
		step.x = -1;
	}
	if (glfwGetKey(window, GLFW_KEY_SPACE))
	{
		stepTaken = 1;
		step.y = 1;
	} 
	else if (glfwGetKey(window, GLFW_KEY_C))
	{
		stepTaken = 1;
		step.y = -1;
	}
	if (stepTaken)
	{
		takeStep(step);
		stepTaken = 0;
		step.x = 0;
		step.y = 0;
		step.z = 0;
	}

	// Cursor
	double xPos;
	double yPos;
	glfwGetCursorPos(window, &xPos, &yPos);
	reorient(xPos, yPos);
}

glm::mat4 DragCamera::WTVMatrix()
{
	return glm::lookAt(_location, _lookingAt, _upDirection);
}


void DragCamera::anchor(glm::vec2 point)
{
	_anchor = point;
	_velocity = {0, 0};
}

void DragCamera::updatePosition(glm::vec2 toPoint)
{
	glm::vec2 direction = toPoint - _anchor;
	calculateNewVectorsForDirection(direction);
	_anchor = toPoint;
}

void DragCamera::updatePosition()
{
	_velocity /= 1.1;
	calculateNewVectorsForDirection(_velocity);
	_zoomVelocity /= 1.2;
	calculateNewVectorsForZoom(_zoomVelocity);
}

void DragCamera::calculateNewVectorsForDirection(glm::vec2 direction)
{
	_velocity = direction;
	if (glm::length(direction) == 0)
		return;

	glm::vec3 forward = glm::normalize(_location - _lookingAt);
	glm::vec3 perpendicular = glm::cross(forward, _upDirection);

	glm::vec3 movementVec = glm::normalize(direction.x*perpendicular + direction.y*_upDirection);
	glm::vec3 rotationAxis = glm::cross(movementVec, forward);

	static const float angle = 0.01;
	glm::mat3 rotationMatrix = glm::mat3(glm::rotate(-glm::length(direction)*angle, rotationAxis));

	_location = rotationMatrix*_location;
	_lookingAt = rotationMatrix*_lookingAt;
	_upDirection = rotationMatrix*_upDirection;
}

void DragCamera::calculateNewVectorsForZoom(float zoomVelocity)
{
	static const float stepLength = 0.01;
	glm::vec3 lookVec = _lookingAt - _location;

	if (glm::length(lookVec) < 1.0 && zoomVelocity < 0.0)
		return;

	lookVec = stepLength*zoomVelocity*lookVec + lookVec;
	_location = _lookingAt - lookVec;

}

void DragCamera::zoom(float direction)
{
	_zoomVelocity += direction;
}
