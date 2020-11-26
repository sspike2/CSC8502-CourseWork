#pragma once
# include "../nclgl/OGLRenderer.h"
# include "../nclgl/Camera.h"
# include "../nclgl/HeightMap.h"
# include "../nclgl/SceneNode.h"
# include "../nclgl/Frustum.h"

class Renderer : public OGLRenderer
{
public:
	Renderer(Window& parent);
	~Renderer(void);

	void RenderScene() override;
	void UpdateScene(float dt) override;

	SceneNode* root = new SceneNode();


	const int numOfBuildingsOnEachSide = 15;
	const int numOfRoadSegments = 7;
	const int numofStreetLightsEachSide = 50;


	const int lightRadius = 2000;



protected:
	void FillBuffers(); //G- Buffer Fill Render Pass
	void DrawPointLights(); // Lighting Render Pass
	void CombineBuffers(); // Combination Render Pass
						   // Make a new texture ...
	void GenerateScreenTexture(GLuint& into, bool depth = false);

	void InitializeTextures();

	void GenerateRoadSegments();
	void GenerateBuildings();
	void GenerateStreetLights();






	Shader* sceneShader; // Shader to fill our GBuffers
	Shader* pointlightShader; // Shader to calculate lighting
	Shader* combineShader; // shader to stick it all together

	GLuint bufferFBO; // FBO for our G- Buffer pass
	GLuint bufferColourTex; // Albedo goes here
	GLuint bufferNormalTex; // Normals go here
	GLuint bufferDepthTex; // Depth goes here




	GLuint pointLightFBO; // FBO for our lighting pass
	GLuint lightDiffuseTex; // Store diffuse lighting
	GLuint lightSpecularTex; // Store specular lighting
	Light* pointLights; // Array of lighting data
	
	Mesh* sphere; // Light volume
	Mesh* quad; // To draw a full - screen quad
	Mesh* cube;
	Mesh* streetLightMesh;

	Camera* camera; // Our usual camera

	GLuint roadTex;
	GLuint roadBump;
	GLuint buildingTex;
	GLuint buildingTex2;
	GLuint streetLightTex;


	Light* streetLightLights;




	void BuildNodeLists(SceneNode* from);
	void SortNodeLists();
	void ClearNodeLists();
	void DrawNodes();
	void DrawNode(SceneNode* n);




	//Frustum frameFrustum;

	vector < SceneNode*> transparentNodeList;
	vector < SceneNode*> nodeList;



};
