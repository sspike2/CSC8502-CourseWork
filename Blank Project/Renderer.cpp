#include"Renderer.h"
#include <algorithm>
const int LIGHT_NUM = 32;
Renderer::Renderer(Window& parent) : OGLRenderer(parent)
{

	sphere = Mesh::LoadFromMeshFile("Sphere.msh");
	quad = Mesh::GenerateQuad();
	cube = Mesh::LoadFromMeshFile("cube.msh");
	streetLightMesh = Mesh::LoadFromMeshFile("/Sush/StreetLight.msh");



	InitializeTextures();

	camera = new Camera(0.0f, 0.0f, (Vector3(-1000, 300, 0)), 600);



	pointLights = new Light[LIGHT_NUM];

	for (int i = 0; i < LIGHT_NUM; ++i)
	{
		Light& l = pointLights[i];
		l.SetPosition(Vector3(rand() % 5000,
			150.0f,
			rand() % 5000));

		l.SetColour(Vector4(0.5f + (float)(rand() / (float)RAND_MAX),
			0.5f + (float)(rand() / (float)RAND_MAX),
			0.5f + (float)(rand() / (float)RAND_MAX),
			1));
		l.SetRadius(250.0f + (rand() % 250));
	}

	sceneShader = new Shader("bumpvertex.glsl", // reused !
		"bufferFragment.glsl");
	//sceneShader = new Shader("TexturedVertex.glsl", // reused !
		//"TexturedFragment.glsl");

	pointlightShader = new Shader("pointlightvertex.glsl",
		"pointlightfragment.glsl");

	//pointlightShader = new Shader("TexturedVertex.glsl", // reused !
		//"TexturedFragment.glsl");

	combineShader = new Shader("combinevert.glsl",
		"combinefrag.glsl");

	//combineShader = new Shader("TexturedVertex.glsl",
		//"TexturedFragment.glsl");

	bool scene, point, combine;
	scene = sceneShader->LoadSuccess();
	point = pointlightShader->LoadSuccess();
	combine = combineShader->LoadSuccess();


	if (!scene || !point || !combine)
	{
		return;
	}

	glGenFramebuffers(1, &bufferFBO);
	glGenFramebuffers(1, &pointLightFBO);





	//for (int i = 0; i < 5; ++i)
	//{
	//	SceneNode* s = new SceneNode();
	//	s->SetColour(Vector4(1.0f, 1.0f, 1.0f, 0.5f));
	//	s->SetTransform(Matrix4::Translation(
	//		Vector3(0, 100.0f, -300.0f + 100.0f + 100 * i)));
	//	//s->SetTransform(Matrix4::Scale(Vector3(100, 100, 100)));
	//	s->SetModelScale(Vector3(100.0f, 100.0f, 100.0f));
	//	s->SetBoundingRadius(100.0f);
	//	s->SetMesh(quad);
	//	s->SetTexture(earthTex);
	//	root->AddChild(s);
	//}





	GenerateRoadSegments();
	GenerateBuildings();
	GenerateStreetLights();




	GLenum buffers[2] = {
		GL_COLOR_ATTACHMENT0 ,
		GL_COLOR_ATTACHMENT1
	};

	// Generate our scene depth texture...
	GenerateScreenTexture(bufferDepthTex, true);
	GenerateScreenTexture(bufferColourTex);
	GenerateScreenTexture(bufferNormalTex);
	GenerateScreenTexture(lightDiffuseTex);
	GenerateScreenTexture(lightSpecularTex);
	// And now attach them to our FBOs
	glBindFramebuffer(GL_FRAMEBUFFER, bufferFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
		GL_TEXTURE_2D, bufferColourTex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1,
		GL_TEXTURE_2D, bufferNormalTex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
		GL_TEXTURE_2D, bufferDepthTex, 0);
	glDrawBuffers(2, buffers);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) !=
		GL_FRAMEBUFFER_COMPLETE)
	{
		return;
	}
	glBindFramebuffer(GL_FRAMEBUFFER, pointLightFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
		GL_TEXTURE_2D, lightDiffuseTex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1,
		GL_TEXTURE_2D, lightSpecularTex, 0);
	glDrawBuffers(2, buffers);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) !=
		GL_FRAMEBUFFER_COMPLETE)
	{
		return;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND);

	init = true;
}
Renderer ::~Renderer(void)
{
	delete sceneShader;
	delete combineShader;
	delete pointlightShader;

	delete camera;
	delete sphere;
	delete quad;
	delete[] pointLights;
	glDeleteTextures(1, &bufferColourTex);
	glDeleteTextures(1, &bufferNormalTex);
	glDeleteTextures(1, &bufferDepthTex);
	glDeleteTextures(1, &lightDiffuseTex);
	glDeleteTextures(1, &lightSpecularTex);

	glDeleteFramebuffers(1, &bufferFBO);
	glDeleteFramebuffers(1, &pointLightFBO);
}
void Renderer::GenerateScreenTexture(GLuint& into, bool depth)
{
	glGenTextures(1, &into);
	glBindTexture(GL_TEXTURE_2D, into);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	GLuint format = depth ? GL_DEPTH_COMPONENT24 : GL_RGBA8;
	GLuint type = depth ? GL_DEPTH_COMPONENT : GL_RGBA;

	glTexImage2D(GL_TEXTURE_2D, 0,
		format, width, height, 0, type, GL_UNSIGNED_BYTE, NULL);

	glBindTexture(GL_TEXTURE_2D, 0);
}
void Renderer::UpdateScene(float dt)
{
	camera->UpdateCamera(dt);
	viewMatrix = camera->BuildViewMatrix();
	//frameFrustum.FromMatrix(projMatrix * viewMatrix);
	root->Update(dt);


}
void Renderer::RenderScene()
{



	BuildNodeLists(root);
	SortNodeLists();
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	FillBuffers();
	ClearNodeLists();
	DrawPointLights();
	CombineBuffers();



}
void Renderer::FillBuffers()
{
	glBindFramebuffer(GL_FRAMEBUFFER, bufferFBO);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	BindShader(sceneShader);
	glUniform1i(
		glGetUniformLocation(sceneShader->GetProgram(), "diffuseTex"), 0);
	glUniform1i(
		glGetUniformLocation(sceneShader->GetProgram(), "bumpTex"), 1);




	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, roadTex);


	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, roadBump);

	DrawNodes();


	modelMatrix.ToIdentity();
	viewMatrix = camera->BuildViewMatrix();
	projMatrix = Matrix4::Perspective(1.0f, 1000000.0f,
		(float)width / (float)height, 45.0f);

	UpdateShaderMatrices();

	//heightMap->Draw();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
void Renderer::DrawPointLights()
{
	glBindFramebuffer(GL_FRAMEBUFFER, pointLightFBO);
	BindShader(pointlightShader);

	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT);
	glBlendFunc(GL_ONE, GL_ONE);
	glCullFace(GL_FRONT);
	glDepthFunc(GL_ALWAYS);
	glDepthMask(GL_FALSE);

	glUniform1i(glGetUniformLocation(
		pointlightShader->GetProgram(), "depthTex"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, bufferDepthTex);

	glUniform1i(glGetUniformLocation(
		pointlightShader->GetProgram(), "normTex"), 1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, bufferNormalTex);

	glUniform3fv(glGetUniformLocation(pointlightShader->GetProgram(),
		"cameraPos"), 1, (float*)&camera->GetPosition());

	glUniform2f(glGetUniformLocation(pointlightShader->GetProgram(),
		"pixelSize"), 1.0f / width, 1.0f / height);

	Matrix4 invViewProj = (projMatrix * viewMatrix).Inverse();
	glUniformMatrix4fv(glGetUniformLocation(
		pointlightShader->GetProgram(), "inverseProjView"),
		1, false, invViewProj.values);
	UpdateShaderMatrices();
	for (int i = 0; i < LIGHT_NUM; ++i)
	{
		Light& l = pointLights[i];
		SetShaderLight(l);
		sphere->Draw();
	}


	for (int i = 0; i < numofStreetLightsEachSide * 2; ++i)
	{
		Light& l = streetLightLights[i];
		SetShaderLight(l);
		sphere->Draw();
	}




	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glCullFace(GL_BACK);
	glDepthFunc(GL_LEQUAL);

	glDepthMask(GL_TRUE);

	glClearColor(0.2f, 0.2f, 0.2f, 1);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
void Renderer::CombineBuffers()
{
	BindShader(combineShader);
	modelMatrix.ToIdentity();
	viewMatrix.ToIdentity();
	projMatrix.ToIdentity();
	UpdateShaderMatrices();

	glUniform1i(glGetUniformLocation(
		combineShader->GetProgram(), "diffuseTex"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, bufferColourTex);

	glUniform1i(glGetUniformLocation(
		combineShader->GetProgram(), "diffuseLight"), 1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, lightDiffuseTex);

	glUniform1i(glGetUniformLocation(
		combineShader->GetProgram(), "specularLight"), 2);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, lightSpecularTex);

	quad->Draw();
}

void Renderer::BuildNodeLists(SceneNode* from)
{
	//if (frameFrustum.InsideFrustum(*from))
	//{
	Vector3 dir = from->GetWorldTransform().GetPositionVector() -
		camera->GetPosition();
	from->SetCameraDistance(Vector3::Dot(dir, dir));

	if (from->GetColour().w < 1.0f)
	{
		transparentNodeList.push_back(from);

	}
	else
	{
		nodeList.push_back(from);

	}

	//}

	for (vector < SceneNode* >::const_iterator i =
		from->GetChildIteratorStart();
		i != from->GetChildIteratorEnd(); ++i)
	{
		BuildNodeLists((*i));

	}
}


void Renderer::SortNodeLists()
{
	std::sort(transparentNodeList.rbegin(), // note the r!
		transparentNodeList.rend(), // note the r!
		SceneNode::CompareByCameraDistance);
	std::sort(nodeList.begin(),
		nodeList.end(),
		SceneNode::CompareByCameraDistance);

}

void Renderer::DrawNodes()
{
	for (const auto& i : nodeList)
	{
		DrawNode(i);

	}
	for (const auto& i : transparentNodeList)
	{
		DrawNode(i);

	}

}


void Renderer::DrawNode(SceneNode* n)
{
	if (n->GetMesh())
	{
		Matrix4 model = n->GetWorldTransform() *
			Matrix4::Scale(n->GetModelScale());
		glUniformMatrix4fv(
			glGetUniformLocation(sceneShader->GetProgram(),
				"modelMatrix"), 1, false, model.values);

		glUniform4fv(glGetUniformLocation(sceneShader->GetProgram(),
			"nodeColour"), 1, (float*)&n->GetColour());

		glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram(), "textureMatrix"),
			1, false, n->GetTextureMatrix().values);


		roadTex = n->GetTexture();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, roadTex);

		glUniform1i(glGetUniformLocation(sceneShader->GetProgram(),
			"useTexture"), roadTex);

		n->Draw(*this);

	}
}


void Renderer::ClearNodeLists()
{
	transparentNodeList.clear();
	nodeList.clear();

}

void Renderer::InitializeTextures()
{
	roadTex = SOIL_load_OGL_texture(
		TEXTUREDIR"/Sush/ground_asphalt_synth_11.png", SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);

	roadBump = SOIL_load_OGL_texture(
		TEXTUREDIR"/Sush/ground_asphalt_synth_11_Bump.png", SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);

	buildingTex = SOIL_load_OGL_texture(
		TEXTUREDIR"/Sush/window1.png", SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);

	buildingTex2 = SOIL_load_OGL_texture(
		TEXTUREDIR"/Sush/window3.png", SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);


	streetLightTex = SOIL_load_OGL_texture(
		TEXTUREDIR"/Sush/mat24.png", SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);





	SetTextureFiltering(roadTex, true);
	SetTextureFiltering(buildingTex, true);
	SetTextureFiltering(buildingTex2, true);

	SetTextureRepeating(roadTex, true, false);
	SetTextureRepeating(roadBump, true, false);

	SetTextureRepeating(buildingTex, true);
	SetTextureRepeating(buildingTex2, true);

}


void Renderer::GenerateRoadSegments()
{
	for (int i = 0; i < numOfRoadSegments; i++)
	{


		SceneNode* road = new SceneNode();
		road->SetColour(Vector4(1.0f, 1.0f, 1.0f, 1.0f));
		road->SetTransform(Matrix4::Translation(Vector3(00, 00, -(i * 10000.0f) - 10000))
			* Matrix4::Rotation(-90, Vector3(1, 0, 0))
		);
		//s->SetTransform(Matrix4::Rotation(-90, Vector3(0, 0, 1)));
		road->SetModelScale(Vector3(10000.0f, 10000.0f, 10000.0f));
		//s->SetBoundingRadius(100.0f);
		road->SetMesh(quad);
		road->SetTexture(roadTex);

		road->SetTextureMatrix(Matrix4::Scale(Vector3(10, 10, 1))
			* Matrix4::Translation(Vector3(0.5f, 0.5f, 0.0f))
			* Matrix4::Rotation(180, Vector3(0, 0, 1))
		);

		root->name = "road";
		root->AddChild(road);
	}
}

void Renderer::GenerateBuildings()
{
	// right side of road
	for (int i = 0; i < numOfBuildingsOnEachSide; i++)
	{
		SceneNode* building = new SceneNode();
		building->SetMesh(cube);
		building->SetTexture(rand() % 2 == 0 ? buildingTex : buildingTex2);

		//building->SetTexture(buildingTex2);

		float yScale = 10000 + rand() % 20000;

		float zpos = 0;
		if (i != 0)
		{
			zpos = -7000 * i;
			float offset = (rand() % 1000) + 1000;
			zpos = zpos + offset;
		}

		building->SetModelScale(Vector3(5000, yScale, 5000));
		building->SetTransform(Matrix4::Translation(Vector3(2800 + rand() % 200,
			(yScale / 2),
			zpos))
		);

		building->SetTextureMatrix(Matrix4::Scale(Vector3(10 + rand() % 30, 50 + rand() % 500, 1))
			//* Matrix4::Translation(Vector3(0.5f, 0.5f, 0.0f))
			//* Matrix4::Rotation(180, Vector3(0, 0, 1))
		);
		root->name = "building";
		root->AddChild(building);
	}

	//left side of road
	for (int i = 0; i < numOfBuildingsOnEachSide; i++)
	{
		SceneNode* building = new SceneNode();
		building->SetMesh(cube);
		building->SetTexture(rand() % 2 == 0 ? buildingTex : buildingTex2);

		float yScale = 10000 + rand() % 20000;
		float zpos = 0;
		if (i != 0)
		{
			zpos = -7000 * i;
			float offset = (rand() % 1000) + 1000;
			zpos = zpos + offset;
		}

		building->SetModelScale(Vector3(5000, yScale, 5000));
		building->SetTransform(Matrix4::Translation(Vector3(-5000,
			(yScale / 2),
			zpos))
		);

		building->SetTextureMatrix(Matrix4::Scale(Vector3(10 + rand() % 30, 50 + rand() % 500, 1))
			//* Matrix4::Translation(Vector3(0.5f, 0.5f, 0.0f))
			//* Matrix4::Rotation(180, Vector3(0, 0, 1))
		);
		root->name = "building";
		root->AddChild(building);
	}

}


void Renderer::GenerateStreetLights()
{



	streetLightLights = new Light[numofStreetLightsEachSide * 2];

	// left streetlights
	for (int i = 0; i < numofStreetLightsEachSide; i++)
	{

		SceneNode* streetlight = new SceneNode();
		streetlight->SetColour(Vector4(1.0f, 1.0f, 1.0f, 1.0f));
		Vector3 pos = Vector3(-2000, 00, -2000 * i);
		streetlight->SetTransform(Matrix4::Translation(pos)
			//* Matrix4::Rotation(-90, Vector3(1, 0, 0))
		);
		//s->SetTransform(Matrix4::Rotation(-90, Vector3(0, 0, 1)));
		streetlight->SetModelScale(Vector3(500.0f, 500.0f, 500.0f));
		//s->SetBoundingRadius(100.0f);
		streetlight->SetMesh(streetLightMesh);
		streetlight->SetTexture(streetLightTex);

		streetlight->SetTextureMatrix(
			//Matrix4::Scale(Vector3(10, 10, 1))
			Matrix4::Translation(Vector3(0.5f, 0.5f, 0.0f))
			//Matrix4::Rotation(180, Vector3(0, 0, 1))
		);

		root->name = "streetLight";
		root->AddChild(streetlight);
		pos.y += 500.0f;

		Light& l = streetLightLights[i];
		l.SetPosition(pos);
		l.SetColour(Vector4(.23f, .74f, .74f, 1));
		l.SetRadius(lightRadius);

	}



	// right streetlights
	for (int i = 0; i < numofStreetLightsEachSide; i++)
	{

		SceneNode* streetlight = new SceneNode();
		streetlight->SetColour(Vector4(1.0f, 1.0f, 1.0f, 1.0f));


		streetlight->SetTransform(Matrix4::Translation((Vector3(0, 00, (-2000 * i) - 750)))
			* Matrix4::Rotation(180, Vector3(0, 1, 0))
		);
		//s->SetTransform(Matrix4::Rotation(-90, Vector3(0, 0, 1)));
		streetlight->SetModelScale(Vector3(500.0f, 500.0f, 500.0f));
		//s->SetBoundingRadius(100.0f);
		streetlight->SetMesh(streetLightMesh);
		streetlight->SetTexture(streetLightTex);

		streetlight->SetTextureMatrix(
			//Matrix4::Scale(Vector3(10, 10, 1))
			Matrix4::Translation(Vector3(0.5f, 0.5f, 0.0f))
			//Matrix4::Rotation(180, Vector3(0, 0, 1))
		);

		root->name = "streetLight";
		root->AddChild(streetlight);


		Light& l = streetLightLights[i + numofStreetLightsEachSide];
		l.SetPosition(Vector3(0, 500.0f, (-2000 * i) - 750));
		l.SetColour(Vector4(.23f, .74f, .74f, 1));
		l.SetRadius(lightRadius);
	}











}
