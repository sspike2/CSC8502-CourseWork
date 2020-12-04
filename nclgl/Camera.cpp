# include "Camera.h"
# include "Window.h"
#include <algorithm>

using namespace std;

void Camera::UpdateCamera(float dt)
{
	if (!isAutomated)
	{
		pitch -= (Window::GetMouse()->GetRelativePosition().y);
		yaw -= (Window::GetMouse()->GetRelativePosition().x);
	}

	pitch = min(pitch, 90.0f);
	pitch = max(pitch, -90.0f);

	if (yaw < 0)
	{
		yaw += 360.0f;
	}
	if (yaw > 360.0f)
	{
		yaw -= 360.0f;
	}
	Matrix4 rotation = Matrix4::Rotation(yaw, Vector3(0, 1, 0));

	Vector3 forward = rotation * Vector3(0, 0, -1);
	Vector3 right = rotation * Vector3(1, 0, 0);

	if (isAutomated)
	{
		if (currentLocation < 3)
		{

			Vector3 dir = Locations[currentLocation + 1].pos - Locations[currentLocation].pos;


			int xDir = 0, yDir = 0, zDir = 0;
			if (dir.x > 0)// target to right
			{
				xDir = 1;
				ammountToTravel = dir.x;
			}
			else if (dir.x < 0) // target to left
			{
				xDir = -1;
				ammountToTravel = -dir.x;

			}// else 0
			if (dir.y > 0)// target to up
			{
				yDir = 1;
				ammountToTravel = dir.y;
			}
			else if (dir.y < 0) // target to down
			{
				yDir = -1;
				ammountToTravel = -dir.y;
			}// else 0
			if (dir.z > 0)// target to back
			{
				zDir = 1;
				ammountToTravel = dir.z;
			}
			else if (dir.z < 0) // target to forward
			{
				zDir = -1;
				ammountToTravel = -dir.z;
			}// else 0


			Vector3 finalDirection = Vector3(xDir, yDir, zDir);

			position += finalDirection * speed * .5f * dt;
			ammountTravelled += speed * .5f * dt;

			if (ammountTravelled > ammountToTravel)
			{
				pitch = Locations[currentLocation + 1].pitchatPos;
				yaw = Locations[currentLocation + 1].yawatPos;
				currentLocation++;
				ammountTravelled = 0;
			}
		}
		else
		{
			Vector3 dir = Locations[0].pos - Locations[currentLocation].pos;
			int xDir = 0, yDir = 0, zDir = 0;
			if (dir.x > 0)// target to right
			{
				xDir = 1;
				ammountToTravel = dir.x;
			}
			else if (dir.x < 0) // target to left
			{
				xDir = -1;
				ammountToTravel = -dir.x;

			}// else 0
			if (dir.y > 0)// target to up
			{
				yDir = 1;
				ammountToTravel = dir.y;
			}
			else if (dir.y < 0) // target to down
			{
				yDir = -1;
				ammountToTravel = -dir.y;
			}// else 0
			if (dir.z > 0)// target to back
			{
				zDir = 1;
				ammountToTravel = dir.z;
			}
			else if (dir.z < 0) // target to forward
			{
				zDir = -1;
				ammountToTravel = -dir.z;
			}// else 0


			Vector3 finalDirection = Vector3(xDir, yDir, zDir);

			position += finalDirection * speed * .5f * dt;
			ammountTravelled += speed * .5f * dt;

			if (ammountTravelled > ammountToTravel)
			{
				pitch = Locations[0].pitchatPos;
				yaw = Locations[0].yawatPos;
				currentLocation = 0;
				ammountTravelled = 0;
			}
		}
	}




	if (Window::GetKeyboard()->KeyDown(KEYBOARD_W))
	{
		position += forward * (speed * dt);
	}
	if (Window::GetKeyboard()->KeyDown(KEYBOARD_S))
	{
		position -= forward * (speed * dt);
	}
	if (Window::GetKeyboard()->KeyDown(KEYBOARD_A))
	{
		position -= right * (speed * dt);
	}
	if (Window::GetKeyboard()->KeyDown(KEYBOARD_D))
	{
		position += right * (speed * dt);
	}

	if (Window::GetKeyboard()->KeyDown(KEYBOARD_SPACE))
	{
		if (isAutomated)
		{
			isAutomated = false;
		}
		position.y += (speed * dt);
	}

	if (Window::GetKeyboard()->KeyDown(KEYBOARD_SHIFT))
	{
		position.y -= (speed * dt);
	}
}

void Camera::WarpTo(Vector3 position, float pitch, float yaw)
{
	if (isAutomated) isAutomated = false;
	this->position = position;
	this->pitch = pitch;
	this->yaw = yaw;

}


Vector3* Camera::GenerateDirection(Vector3 dir)
{
	int xDir = 0, yDir = 0, zDir = 0;

	if (dir.x > 0)// target to right
	{
		xDir = 1;
	}
	else if (dir.x < 0) // target to left
	{
		xDir = -1;
	}// else 0



	if (dir.y > 0)// target to up
	{
		yDir = 1;
	}
	else if (dir.y < 0) // target to down
	{
		yDir = -1;
	}// else 0



	if (dir.z > 0)// target to back
	{
		xDir = 1;
	}
	else if (dir.z < 0) // target to forward
	{
		xDir = -1;
	}// else 0


	return new Vector3(xDir, yDir, zDir);

}






Matrix4 Camera::BuildViewMatrix()
{
	return Matrix4::Rotation(-pitch, Vector3(1, 0, 0)) *
		Matrix4::Rotation(-yaw, Vector3(0, 1, 0)) *
		Matrix4::Translation(-position);
};