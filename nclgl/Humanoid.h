#pragma once
#include "MeshAnimation.h"
#include "MeshMaterial.h"
#include <glad\glad.h>


class Humanoid
{
public:
	Humanoid(MeshAnimation* anim, MeshMaterial* mat);
	~Humanoid();


	MeshAnimation* getMeshAnimation()const { return anim; }
	void setMeshAnimation(MeshAnimation* animation) { anim = animation; }


	MeshMaterial* getMeshMaterial()const { return material; }
	void setMeshMaterial(MeshMaterial* mat) { material = mat; }

	vector <GLuint > matTextures;

	int currentFrame;
	float frameTime;

	float speed;
	Vector3 direction;

protected:
	MeshAnimation* anim;
	MeshMaterial* material;

};

