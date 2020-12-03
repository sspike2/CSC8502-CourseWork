#include "Light.h"
#include "Window.h"


void Light:: UpdateLight(float dt)
{
	Vector3 forward = Vector3(0, 0, -1);
	Vector3 right = Vector3(1, 0, 0);

	float speed = 20 * dt;

	if (Window::GetKeyboard()->KeyDown(KEYBOARD_I))
		position += forward * speed;
	if (Window::GetKeyboard()->KeyDown(KEYBOARD_K))
		position -= forward * speed;
	if (Window::GetKeyboard()->KeyDown(KEYBOARD_J))
		position -= right * speed;
	if (Window::GetKeyboard()->KeyDown(KEYBOARD_L))
		position += right * speed;
	if (Window::GetKeyboard()->KeyDown(KEYBOARD_O))
		position.y += speed;
	if (Window::GetKeyboard()->KeyDown(KEYBOARD_P))
		position.y -= speed;

}