#include"Renderer.h"
#include <algorithm>
const int LIGHT_NUM = 32;
const int POST_PASSES = 1;
Renderer::Renderer(Window& parent) : OGLRenderer(parent)
{

	sphere = Mesh::LoadFromMeshFile("Sphere.msh");
	quad = Mesh::GenerateQuad();
	cube = Mesh::LoadFromMeshFile("cube.msh");
	streetLightMesh = Mesh::LoadFromMeshFile("/Sush/StreetLight.msh");

	humanoidMesh = Mesh::LoadFromMeshFile("Role_T.msh");
	anim = new MeshAnimation("Role_T.anm");
	mat = new MeshMaterial("Role_T.mat");







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

	pointlightShader = new Shader("pointlightvertex.glsl",
		"pointlightfragment.glsl");

	combineShader = new Shader("combinevert.glsl",
		"combinefrag.glsl");

	skyboxShader = new Shader(
		"skyboxVertex.glsl", "skyboxFragment.glsl");

	bloomShader = new Shader("BloomVert.glsl",
		"BloomFrag.glsl");

	FishEyeShader = new Shader("FishEyeVert.glsl",
		"FishEyeFrag.glsl");

	textureShader = new Shader("TexturedVertex.glsl",
		"TexturedFragment.glsl");

	skinningShader = new Shader("SkinningVertex.glsl",
		"TexturedFragment.glsl");




	//multiple bools to check exactly shader which is causing issue
	bool scene, point, combine, sky, post;
	scene = sceneShader->LoadSuccess();
	point = pointlightShader->LoadSuccess();
	combine = combineShader->LoadSuccess();
	sky = skyboxShader->LoadSuccess();
	post = bloomShader->LoadSuccess();
	bool skinning = skinningShader->LoadSuccess();

	if (!scene || !point || !combine || !sky || !post)
	{
		return;
	}


	glGenFramebuffers(1, &bufferFBO);
	glGenFramebuffers(1, &pointLightFBO);
	glGenFramebuffers(1, &combineFBO);
	glGenFramebuffers(1, &processFBO);






	GenerateRoadSegments();
	GenerateBuildings();
	GenerateStreetLights();
	GenerateHumanoids();




	GLenum buffers[4] = {
		GL_COLOR_ATTACHMENT0,
		GL_COLOR_ATTACHMENT1,
		GL_COLOR_ATTACHMENT2,
		GL_COLOR_ATTACHMENT3
	};

	// Generate our scene depth texture...
	GenerateScreenTexture(bufferDepthTex, true);
	GenerateScreenTexture(bufferColourTex);
	GenerateScreenTexture(bufferNormalTex);
	GenerateScreenTexture(bufferEmissionTex);
	GenerateScreenTexture(bufferFogTex);
	GenerateScreenTexture(lightDiffuseTex);
	GenerateScreenTexture(lightSpecularTex);
	GenerateScreenTexture(bufferCombineTex);
	GenerateScreenTexture(processTex[0]);
	GenerateScreenTexture(processTex[1]);


	// And now attach them to our FBOs
	glBindFramebuffer(GL_FRAMEBUFFER, bufferFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
		GL_TEXTURE_2D, bufferColourTex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1,
		GL_TEXTURE_2D, bufferNormalTex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2,
		GL_TEXTURE_2D, bufferEmissionTex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3,
		GL_TEXTURE_2D, bufferFogTex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
		GL_TEXTURE_2D, bufferDepthTex, 0);
	glDrawBuffers(4, buffers);

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


	glBindFramebuffer(GL_FRAMEBUFFER, combineFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
		GL_TEXTURE_2D, bufferCombineTex, 0);
	glDrawBuffers(1, buffers);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) !=
		GL_FRAMEBUFFER_COMPLETE)
	{
		return;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, processFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
		GL_TEXTURE_2D, processTex[0], 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
		GL_TEXTURE_2D, processTex[1], 0);

	glDrawBuffers(1, buffers);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) !=
		GL_FRAMEBUFFER_COMPLETE)
	{
		return;
	}








	glClipControl(GL_LOWER_LEFT, GL_ZERO_TO_ONE);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);


	BuildNodeLists(root);
	SortNodeLists();

	init = true;
}
Renderer ::~Renderer(void)
{
	delete sceneShader;
	delete combineShader;
	delete pointlightShader;
	delete skyboxShader;
	delete textureShader;
	delete bloomShader;
	delete FishEyeShader;
	delete skinningShader;


	delete camera;
	delete sphere;
	delete quad;
	delete[] pointLights;
	glDeleteTextures(1, &bufferColourTex);
	glDeleteTextures(1, &bufferNormalTex);
	glDeleteTextures(1, &bufferDepthTex);
	glDeleteTextures(1, &bufferEmissionTex);
	glDeleteTextures(1, &bufferFogTex);
	glDeleteTextures(1, &lightDiffuseTex);
	glDeleteTextures(1, &lightSpecularTex);
	glDeleteTextures(2, &processFBO);

	glDeleteFramebuffers(1, &bufferFBO);
	glDeleteFramebuffers(1, &pointLightFBO);
	glDeleteFramebuffers(1, &combineFBO);
	glDeleteFramebuffers(1, &processFBO);




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

	for (const auto& i : SkinnedMesh)
	{
		i->humanoid->frameTime -= dt;

		while (i->humanoid->frameTime < 0.0f)
		{
			i->humanoid->currentFrame = (i->humanoid->currentFrame + 1) %
				i->humanoid->getMeshAnimation()->GetFrameCount();

			i->humanoid->frameTime += 1.0f / i->humanoid->getMeshAnimation()->GetFrameRate();
		}

		Vector3 currpos = i->GetTransform().GetPositionVector();
		currpos += (i->humanoid->direction) * (i->humanoid->speed * dt);

		i->SetTransform(Matrix4::Translation(currpos) * Matrix4::Rotation(180, Vector3(0, 1, 0)));



	}



}
void Renderer::RenderScene()
{




	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	FillBuffers();
	DrawPointLights();
	CombineBuffers();
	if (shouldRenderPostProcess)
	{
		if (isBloomOn)
		{
			Bloom();
		}
		else
		{
			FishEyeEffect();
		}
	}

	PrintScene();


	//ClearNodeLists();
}
void Renderer::FillBuffers()
{
	glBindFramebuffer(GL_FRAMEBUFFER, bufferFBO);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	modelMatrix.ToIdentity();
	viewMatrix = camera->BuildViewMatrix();
	projMatrix = Matrix4::Perspective(1.0f, 100000.0f,
		(float)width / (float)height, 45.0f);

	DrawSkybox();

	//UpdateShaderMatrices();

	DrawNodes();
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
	glDepthMask(GL_TRUE);
	glDepthFunc(GL_GEQUAL);

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


	glBindFramebuffer(GL_FRAMEBUFFER, combineFBO);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);



	BindShader(combineShader);
	modelMatrix.ToIdentity();
	viewMatrix.ToIdentity();
	projMatrix.ToIdentity();
	UpdateShaderMatrices();



	SetTextureToShader(bufferColourTex, 0, "diffuseTex", combineShader);

	SetTextureToShader(lightDiffuseTex, 1, "diffuseLight", combineShader);

	SetTextureToShader(bufferEmissionTex, 2, "emissionTex", combineShader);

	SetTextureToShader(bufferFogTex, 3, "fogTex", combineShader);

	SetTextureToShader(lightSpecularTex, 4, "specularLight", combineShader);

	quad->Draw();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::FishEyeEffect()
{
	glBindFramebuffer(GL_FRAMEBUFFER, processFBO);

	BindShader(FishEyeShader);
	modelMatrix.ToIdentity();
	viewMatrix.ToIdentity();
	projMatrix.ToIdentity();
	UpdateShaderMatrices();


	glUniform1i(glGetUniformLocation(
		FishEyeShader->GetProgram(), "diffuseTex"), 1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, bufferCombineTex);

	quad->Draw();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


void Renderer::Bloom()
{
	glBindFramebuffer(GL_FRAMEBUFFER, processFBO);
	glClear(GL_COLOR_BUFFER_BIT);

	BindShader(bloomShader);
	modelMatrix.ToIdentity();
	viewMatrix.ToIdentity();
	projMatrix.ToIdentity();
	UpdateShaderMatrices();


	glUniform1i(glGetUniformLocation(
		bloomShader->GetProgram(), "emissionTex"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, bufferEmissionTex);

	glUniform1i(glGetUniformLocation(
		bloomShader->GetProgram(), "diffuseTex"), 1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, bufferCombineTex);

	glUniform1i(glGetUniformLocation(
		bloomShader->GetProgram(), "lightTex"), 2);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, lightDiffuseTex);

	quad->Draw();

	for (int i = 0; i < POST_PASSES; ++i)
	{

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
			GL_TEXTURE_2D, processTex[1], 0);
		glUniform1i(glGetUniformLocation(bloomShader->GetProgram(),
			"isVertical"), 0);

		glBindTexture(GL_TEXTURE_2D, processTex[0]);
		quad->Draw();
		// Now to swap the colour buffers , and do the second blur pass
		glUniform1i(glGetUniformLocation(bloomShader->GetProgram(),
			"isHorizontal"), 1);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
			GL_TEXTURE_2D, processTex[0], 0);
		glBindTexture(GL_TEXTURE_2D, processTex[1]);
		quad->Draw();
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::PrintScene()
{
	glClear(GL_COLOR_BUFFER_BIT);

	BindShader(textureShader);

	modelMatrix.ToIdentity();
	viewMatrix.ToIdentity();
	projMatrix.ToIdentity();
	UpdateShaderMatrices();



	if (shouldRenderPostProcess)
	{

		glUniform1i(glGetUniformLocation(
			textureShader->GetProgram(), "diffuseTex"), 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, processTex[0]);

		/*if (bloomShader)
		{*/
		/*glUniform1i(glGetUniformLocation(
			textureShader->GetProgram(), "diffuseTex2"), 1);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, processTex[1]);*/
		//}
	}
	else
	{
		glUniform1i(glGetUniformLocation(
			textureShader->GetProgram(), "diffuseTex"), 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, bufferCombineTex);
	}

	quad->Draw();
	//glBindFramebuffer(GL_FRAMEBUFFER, 0);
}









void Renderer::BuildNodeLists(SceneNode* from)
{
	//if (frameFrustum.InsideFrustum(*from))
	//{
	Vector3 dir = from->GetWorldTransform().GetPositionVector() -
		camera->GetPosition();
	from->SetCameraDistance(Vector3::Dot(dir, dir));


	switch (from->currentShader)
	{
	case shaderType::Skinning:
		SkinnedMesh.push_back(from);
		break;
	case shaderType::Standard:
		nodeList.push_back(from);
	default:
		break;
	}


	for (vector < SceneNode* >::const_iterator i =
		from->GetChildIteratorStart();
		i != from->GetChildIteratorEnd(); ++i)
	{
		BuildNodeLists((*i));
	}
}






void Renderer::SortNodeLists()
{
	std::sort(SkinnedMesh.rbegin(), // note the r!
		SkinnedMesh.rend(), // note the r!
		SceneNode::CompareByCameraDistance);
	std::sort(nodeList.begin(),
		nodeList.end(),
		SceneNode::CompareByCameraDistance);

}



void Renderer::DrawNodes()
{

	BindShader(sceneShader);
	UpdateShaderMatrices();
	for (const auto& i : nodeList)
	{
		DrawNode(i);
	}

	BindShader(skinningShader);
	UpdateShaderMatrices();
	for (const auto& i : SkinnedMesh)
	{
		DrawSkinnedNode(i);
	}

}


void Renderer::DrawNode(SceneNode* n)
{
	if (n->GetMesh())
	{
		nodeTex = n->GetTexture();
		SetTextureToShader(nodeTex, 0, "diffuseTex", sceneShader);

		nodeBumpTex = n->GetBumpTex();
		SetTextureToShader(nodeBumpTex, 1, "bumpTex", sceneShader);

		nodeEmissionTex = n->GetEmissionTex();
		SetTextureToShader(nodeEmissionTex, 2, "emiisionTex", sceneShader);

		glUniform4fv(glGetUniformLocation(sceneShader->GetProgram(),
			"emissionColor"), 1, (float*)&n->GetEmissionColour());


		glUniform4fv(glGetUniformLocation(sceneShader->GetProgram(),
			"fogcolor"), 1, (float*)&Vector4(.5f, 0, .2f, 1));

		glUniform1f(glGetUniformLocation(sceneShader->GetProgram(),
			"linearStart"), 1000.0f);

		glUniform1f(glGetUniformLocation(sceneShader->GetProgram(),
			"linearEnd"), 10000.0f);

		glUniform1f(glGetUniformLocation(sceneShader->GetProgram(),
			"density"), 0.0002f);

		glUniform1f(glGetUniformLocation(sceneShader->GetProgram(),
			"fogType"), fogType);


		Matrix4 model = n->GetWorldTransform() *
			Matrix4::Scale(n->GetModelScale());
		glUniformMatrix4fv(
			glGetUniformLocation(sceneShader->GetProgram(),
				"modelMatrix"), 1, false, model.values);

		glUniform4fv(glGetUniformLocation(sceneShader->GetProgram(),
			"nodeColour"), 1, (float*)&n->GetColour());

		glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram(), "textureMatrix"),
			1, false, n->GetTextureMatrix().values);

		n->Draw(*this);

	}
}
void Renderer::DrawSkinnedNode(SceneNode* n)
{
	if (n->GetMesh())
	{
		Matrix4 model = n->GetWorldTransform() *
			Matrix4::Scale(n->GetModelScale());
		glUniformMatrix4fv(
			glGetUniformLocation(sceneShader->GetProgram(),
				"modelMatrix"), 1, false, model.values);


		modelMatrix = model;
		//Matrix4::Translation(Vector3(-400, 200, -200)) *
		//Matrix4::Scale(Vector3(300, 300, 300));

		UpdateShaderMatrices();
		nodeTex = n->GetTexture();
		SetTextureToShader(nodeTex, 0, "diffuseTex", skinningShader);




		//glUniform3fv(glGetUniformLocation(pointlightShader->GetProgram(),
			//"position"), 1, (float*)&n->GetTransform().GetPositionVector());




		vector < Matrix4 > frameMatrices;
		const Matrix4* invBindPose = n->GetMesh()->GetInverseBindPose();
		const Matrix4* frameData = anim->GetJointData(n->humanoid->currentFrame);

		for (unsigned int i = 0; i < n->GetMesh()->GetJointCount(); ++i)
		{
			frameMatrices.emplace_back(frameData[i] * invBindPose[i]);
		}

		int j = glGetUniformLocation(skinningShader->GetProgram(), "joints");
		glUniformMatrix4fv(j, frameMatrices.size(), false,
			(float*)frameMatrices.data());
		for (int i = 0; i < n->GetMesh()->GetSubMeshCount(); ++i)
		{
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, n->humanoid->matTextures[i]);
			n->GetMesh()->DrawSubMesh(i);
		}



	}
}




void Renderer::ClearNodeLists()
{
	SkinnedMesh.clear();
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
		TEXTUREDIR"/Sush/window4.png", SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);

	buildingTex2 = SOIL_load_OGL_texture(
		TEXTUREDIR"/Sush/window5.png", SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);


	streetLightTex = SOIL_load_OGL_texture(
		TEXTUREDIR"/Sush/mat24.png", SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);


	streetLightEmiss = SOIL_load_OGL_texture(
		TEXTUREDIR"/Sush/mat24-Emmis.png", SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);



	Skybox = SOIL_load_OGL_cubemap(
		TEXTUREDIR"/Sush/skybox/S.png", TEXTUREDIR"/Sush/skybox/N.png",
		TEXTUREDIR"/Sush/skybox/U.png", TEXTUREDIR"/Sush/skybox/D.png",
		TEXTUREDIR"/Sush/skybox/E.png", TEXTUREDIR"/Sush/skybox/W.png",
		SOIL_LOAD_RGB, SOIL_CREATE_NEW_ID, 0);


	//emissTex =





	SetTextureFiltering(roadTex, true);
	//SetTextureFiltering(buildingTex, true);
	//SetTextureFiltering(buildingTex2, true);

	SetTextureFilteringMipMaps(buildingTex);
	SetTextureFilteringMipMaps(buildingTex2);

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

		road->name = "road";
		root->AddChild(road);
	}
}

void Renderer::GenerateHumanoids()
{
	for (int i = 0; i < 2; i++)
	{


		SceneNode* node = new SceneNode(humanoidMesh);
		node->humanoid = new Humanoid(anim, mat);

		node->humanoid->speed = 300 + rand() % 50;
		node->humanoid->direction = Vector3(0, 0, -1);

		node->currentShader = shaderType::Skinning;

		Vector3 pos = Vector3(i == 0 ? -2500 : 1000, 00, -200);

		node->SetTransform(Matrix4::Translation(pos) * Matrix4::Rotation(180, Vector3(0, 1, 0)));// *Matrix4::Scale(Vector3(30, 30, 30)));
		node->SetModelScale(Vector3(200, 200, 200));
		for (int i = 0; i < node->GetMesh()->GetSubMeshCount(); ++i)
		{
			const MeshMaterialEntry* matEntry =
				node->humanoid->getMeshMaterial()->GetMaterialForLayer(i);

			const string* filename = nullptr;
			matEntry->GetEntry("Diffuse", &filename);
			string path = TEXTUREDIR + *filename;
			GLuint texID = SOIL_load_OGL_texture(path.c_str(), SOIL_LOAD_AUTO,
				SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y);
			node->humanoid->matTextures.emplace_back(texID);
		}



		root->AddChild(node);


		//node->SetMesh()
		//H

	}
}

void Renderer::GenerateBuildings()
{
	// right side of road
	for (int i = 0; i < numOfBuildingsOnEachSide; i++)
	{
		SceneNode* building = new SceneNode();
		building->SetMesh(cube);

		int texid = rand() % 2;


		GLuint currentTex = texid == 0 ? buildingTex : buildingTex2;
		building->SetTexture(currentTex);

		building->SetEmissionTexture(currentTex);
		building->SetEmissionColour(GetRandomBuildingColor());

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

		building->SetTransform(Matrix4::Translation(Vector3(4000 + rand() % 200,
			(yScale / 2),
			zpos))
		);

		float xTexScale = 0, yTexScale = 0;

		if (texid == 0)
		{
			xTexScale = 10 + rand() % 30;
			yTexScale = 50 + rand() % 50;
		}
		else
		{
			xTexScale = 10 + rand() % 15;
			yTexScale = 50 + rand() % 10;
		}

		building->SetTextureMatrix(Matrix4::Scale(Vector3(xTexScale, yTexScale, 1))

			//* Matrix4::Translation(Vector3(0.5f, 0.5f, 0.0f))
			//* Matrix4::Rotation(180, Vector3(0, 0, 1))
		);
		building->name = "building";
		root->AddChild(building);
	}

	//left side of road
	for (int i = 0; i < numOfBuildingsOnEachSide; i++)
	{
		SceneNode* building = new SceneNode();
		building->SetMesh(cube);
		int texid = rand() % 2;

		GLuint currentTex = texid == 0 ? buildingTex : buildingTex2;

		building->SetTexture(currentTex);
		building->SetEmissionTexture(currentTex);




		building->SetEmissionColour(GetRandomBuildingColor());

		float yScale = 10000 + rand() % 20000;
		float zpos = 0;
		if (i != 0)
		{
			zpos = -7000 * i;
			float offset = (rand() % 1000) + 1000;
			zpos = zpos + offset;
		}

		building->SetModelScale(Vector3(5000, yScale, 5000));
		building->SetTransform(Matrix4::Translation(Vector3(-6200,
			(yScale / 2),
			zpos))
		);

		float xTexScale = 0, yTexScale = 0;

		if (texid == 0)
		{
			xTexScale = 10 + rand() % 30;
			yTexScale = 50 + rand() % 50;
		}
		else
		{
			xTexScale = 10 + rand() % 15;
			yTexScale = 50 + rand() % 10;
		}

		building->SetTextureMatrix(Matrix4::Scale(Vector3(xTexScale, yTexScale, 1))
			//* Matrix4::Translation(Vector3(0.5f, 0.5f, 0.0f))
			//* Matrix4::Rotation(180, Vector3(0, 0, 1))
		);
		building->name = "building";
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

		streetlight->SetModelScale(Vector3(500.0f, 500.0f, 500.0f));
		streetlight->SetMesh(streetLightMesh);
		streetlight->SetTexture(streetLightTex);
		streetlight->SetEmissionTexture(streetLightEmiss);
		streetlight->SetEmissionColour(Color::cyan());

		streetlight->SetTextureMatrix(
			//Matrix4::Scale(Vector3(10, 10, 1))
			Matrix4::Translation(Vector3(0, 0.5f, 0.0f))
			//Matrix4::Rotation(180, Vector3(0, 0, 1))
		);

		streetlight->name = "streetLight";
		root->AddChild(streetlight);
		pos.y += 500.0f;

		Light& l = streetLightLights[i];
		l.SetPosition(pos);
		l.SetColour(Vector4(.23f, .74f, .74f, 1)); // light blue
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
		streetlight->SetEmissionTexture(streetLightEmiss);
		streetlight->SetEmissionColour(Color::cyan());


		streetlight->SetTextureMatrix(
			//Matrix4::Scale(Vector3(10, 10, 1))
			Matrix4::Translation(Vector3(0, .5f, 0.0f))
			//Matrix4::Rotation(180, Vector3(0, 0, 1))
		);

		streetlight->name = "streetLight";
		root->AddChild(streetlight);


		Light& l = streetLightLights[i + numofStreetLightsEachSide];
		l.SetPosition(Vector3(0, 500.0f, (-2000 * i) - 750));
		l.SetColour(Vector4(.23f, .74f, .74f, 1)); // light blue
		l.SetRadius(lightRadius);
	}

}

Vector4 Renderer::GetRandomBuildingColor()
{
	int index = rand() % 5;

	switch (index)
	{
	case 0:
		return Vector4(.9f, 1, .556f, 1); // light yellow
		break;
	case 1:
		return Color::cyan();// cyan
		break;
	case 2:
		return Vector4(1, 0.2962264f, 0.2962264f, 1);//Light pink
		break;
	case 3:
		return Vector4(.5f, 1, .5f, 1);//light green
		break;
	default:
		return Color::white(); // white
		break;
	}



}



void Renderer::DrawSkybox()
{
	glDepthMask(GL_FALSE);

	BindShader(skyboxShader);
	UpdateShaderMatrices();

	quad->Draw();

	glDepthMask(GL_TRUE);
}


void Renderer::ToggleFogType()
{
	if (fogType == 0)
	{
		fogType = 1;
	}
	else if (fogType == 1)
	{
		fogType = 2;
	}
	else
	{
		fogType = 0;
	}
}

void Renderer::TogglePostProcessing()
{
	shouldRenderPostProcess = !shouldRenderPostProcess;
}

void Renderer::ToggleBloomFishEye()
{
	isBloomOn = !isBloomOn;
}






