#include "Humanoid.h"

Humanoid::Humanoid(MeshAnimation* anim, MeshMaterial* mat)
{

	setMeshAnimation(anim);
	setMeshMaterial(mat);
	currentFrame = 0;
	frameTime = 0.0f;
}