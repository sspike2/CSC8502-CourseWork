#include "../NCLGL/window.h"
#include "Renderer.h"


int main()
{

	srand(time(NULL));
	Window w("Blank Project!", 1280, 720, false); //This is all boring win32 window creation stuff!
	if (!w.HasInitialised())
	{
		return -1;
	}

	Renderer renderer(w); //This handles all the boring OGL 3.2 initialisation stuff, and sets up our tutorial!
	if (!renderer.HasInitialised())
	{
		return -1;
	}

	w.LockMouseToWindow(true);
	w.ShowOSPointer(false);

	while (w.UpdateWindow() && !Window::GetKeyboard()->KeyDown(KEYBOARD_ESCAPE))
	{


		/*if(Window::GetKeyboard()->KeyDown(KEYBOARD_0))
		{
			renderer.toggleFiltering();
		}*/

		if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_P))
		{
			renderer.ToggleFogType();
		}

		if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_O))
		{
			renderer.TogglePostProcessing();
		}

		if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_L))
		{
			renderer.ToggleBloomFishEye();
		}



		// starting position
		if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_1))
		{
			//renderer.camera->SetPosition

			renderer.camera->WarpTo(Vector3(-1000, 300, 0), 0, 0);
		}

		// warp to end of road
		if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_2))
		{
			renderer.camera->WarpTo(Vector3(-1000, 300, -80000), 0, 180);
		}

		//Bird's eye view of city
		if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_3))
		{
			renderer.camera->WarpTo(Vector3(-1000, 25000, 0), -50, 0);
		}





		renderer.UpdateScene(w.GetTimer()->GetTimeDeltaSeconds());
		renderer.RenderScene();
		renderer.SwapBuffers();
		if (Window::GetKeyboard()->KeyDown(KEYBOARD_F5))
		{
			Shader::ReloadAllShaders();
		}
	}


	return 0;
}





