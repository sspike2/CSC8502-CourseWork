#pragma once
#include "SceneNode.h"
#include "MeshAnimation.h"
#include "MeshMaterial.h"
#include "Camera.h"
class SceneNodeSkinned: public SceneNode
{

public:

	MeshAnimation* getMeshAnimation()const { return anim; }
	void setMeshAnimation(MeshAnimation* animation) { anim = animation; }


	MeshMaterial* getMeshMaterial()const { return material; }
	void setMeshMaterial(MeshMaterial* mat) { material = mat; }


	vector <GLuint > matTextures;

	int currentFrame;
	float frameTime;

protected:
	MeshAnimation* anim;
	MeshMaterial* material;
};

