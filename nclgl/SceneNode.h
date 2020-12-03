# pragma once
# include "Matrix4.h"
# include "Vector3.h"
# include "Vector4.h"
# include "Mesh.h"
# include <vector >

enum shaderType
{
	Standard,
	Skinning
};



class SceneNode
{
public:
	SceneNode(Mesh* m = NULL, Vector4 colour = Vector4(1, 1, 1, 1));
	~SceneNode(void);

	char* name;
	shaderType currentShader;
	






	void SetTransform(const Matrix4& matrix) { transform = matrix; }

	const Matrix4& GetTransform() const { return transform; }
	Matrix4 GetWorldTransform() const { return worldTransform; }

	Vector4 GetColour() const { return colour; }
	void SetColour(Vector4 c) { colour = c; }

	Vector4 GetEmissionColour() const { return emissonColor; }
	void SetEmissionColour(Vector4 c) { emissonColor = c; }




	

	Vector3 GetModelScale() const { return modelScale; }
	void SetModelScale(Vector3 s) { modelScale = s; }

	Mesh* GetMesh() const { return mesh; }
	void SetMesh(Mesh* m) { mesh = m; }

	void AddChild(SceneNode* s);

	virtual void Update(float dt);
	virtual void Draw(const OGLRenderer& r);


	std::vector < SceneNode* >::const_iterator GetChildIteratorStart()
	{
		return children.begin();
	}

	std::vector < SceneNode* >::const_iterator GetChildIteratorEnd()
	{
		return children.end();
	}


	float GetBoundingRadius() const { return boundingRadius; }
	void SetBoundingRadius(float f) { boundingRadius = f; }

	float GetCameraDistance() const { return distanceFromCamera; }
	void SetCameraDistance(float f) { distanceFromCamera = f; }

	void SetTexture(GLuint tex) { texture = tex; }
	GLuint GetTexture() const { return texture; }

	void   SetBumpTex(GLuint bump) { bumpTex = bump; }
	GLuint GetBumpTex() const { return bumpTex; }

	void SetEmissionTexture(GLuint tex) { emissionTex = tex; }
	GLuint GetEmissionTex() const { return emissionTex; }

	//void SetTextureTiling(Vector3 dfsdf){TextureMatrix}

	static bool CompareByCameraDistance(SceneNode* a, SceneNode* b)
	{
		return (a->distanceFromCamera < b->distanceFromCamera) ? true : false;
	}





	const Matrix4& GetTextureMatrix() const { return textureMatrix; }
	void SetTextureMatrix(const Matrix4& textureMatrix) { this->textureMatrix = textureMatrix; }


protected:
	SceneNode* parent;
	Mesh* mesh;
	Matrix4 worldTransform;
	Matrix4 transform;
	Vector3 modelScale;
	Vector4 colour;
	Vector4 emissonColor;
	std::vector < SceneNode*> children;
	float distanceFromCamera;
	float boundingRadius;
	GLuint texture;
	GLuint bumpTex;
	//bool has emm/
	GLuint emissionTex;

	Matrix4 textureMatrix;
};