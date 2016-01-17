#ifndef _CAMERA_
#define _CAMERA_

#include <glm/glm.hpp>

#include <GLFW/glfw3.h> // Including this here to make it possible to implement more helper functions.

class Camera
{
	public:
		Camera(glm::vec3 location, glm::vec3 lookingAt, glm::vec3 upDirection) :
			_location(location), _lookingAt(lookingAt), _upDirection(upDirection) {};
		~Camera() = default;

		void reorient(double newX, double newY);
		void takeStep(glm::vec3 directionRelativeToCamera);
		void pollWindowForCameraMovement(GLFWwindow* window);
		glm::mat4 WTVMatrix();
		glm::vec3 at() { return _location; }



	private:
		glm::vec3 _location;
		glm::vec3 _lookingAt;
		glm::vec3 _upDirection;

		bool lookVecTooCloseToUpDirection(glm::vec3 lookVec);
};



class DragCamera // These should maybe have a common main class
{
	public:
		DragCamera(glm::vec3 location, glm::vec3 lookingAt, glm::vec3 upDirection) :
			_location{location}, _lookingAt{lookingAt}, _upDirection{upDirection} {};
		~DragCamera() = default;

		void anchor(glm::vec2 point); // Relative to screen
		void updatePosition(glm::vec2 toPoint); // Called if the mousebutton is pressed
		void updatePosition(); // Called otherwise
		void zoom(float direction); // + in, -
		void print();
		glm::mat4 WTVMatrix();
		glm::vec3 at() { return _location; }

	private: 
		glm::vec2 _anchor{0, 0};
		glm::vec3 _location{0, 0, 5};
		glm::vec3 _lookingAt{0, 0, 0};
		glm::vec3 _upDirection{0, 1, 0};

		glm::vec2 _velocity{0, 0}; // Using this to have some feeling of momentum
		float _zoomVelocity{0};

		void calculateNewVectorsForDirection(glm::vec2 direction);
		void calculateNewVectorsForZoom(float zoomVelocity);
};


#endif