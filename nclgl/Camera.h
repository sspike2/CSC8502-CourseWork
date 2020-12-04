# pragma once
# include "Matrix4.h"
# include "Vector3.h"

class Camera
{
public:

	struct trackLocations
	{
		Vector3 pos;
		float pitchatPos;
		float yawatPos;
	};

	trackLocations Locations[4];

	Camera(void)
	{
		yaw = 0.0f;
		pitch = 0.0f;
	};

	Camera(float pitch, float yaw, Vector3 position, float speed)
	{
		this->pitch = pitch;
		this->yaw = yaw;
		this->position = position;
		this->speed = speed;

		Locations[0].pos = Vector3(-1000, 300, 0);
		Locations[0].pitchatPos = 0;
		Locations[0].yawatPos = 0;

		Locations[1].pos = Vector3(-1000, 300, -4000);
		Locations[1].pitchatPos = 0;
		Locations[1].yawatPos = 90;

		Locations[2].pos = Vector3(-2000, 300, -4000);
		Locations[2].pitchatPos = 0;
		Locations[2].yawatPos = 180;

		Locations[3].pos = Vector3(-2000, 300, 0);
		Locations[3].pitchatPos = 0;
		Locations[3].yawatPos = -90;
	}

	~Camera(void) {};

	void UpdateCamera(float dt = 1.0f);

	Matrix4 BuildViewMatrix();

	Vector3 GetPosition() const { return position; }
	void SetPosition(Vector3 val) { position = val; }

	float GetYaw() const { return yaw; }
	void SetYaw(float y) { yaw = y; }

	float GetPitch() const { return pitch; }
	void SetPitch(float p) { pitch = p; }


	void WarpTo(Vector3 pos, float pitch, float yaw);


protected:
	float yaw;
	float pitch;
	Vector3 position; // Set to 0,0,0 by Vector3 constructor ;)
	Vector3* GenerateDirection(Vector3 dir);
	float speed;
	bool isAutomated = true;

	int currentLocation = 0;
	float ammountToTravel = 0;
	float ammountTravelled = 0;
};