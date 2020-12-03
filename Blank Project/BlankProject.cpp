#include "../NCLGL/window.h"
#include "Renderer.h"


int main() {

	srand(time(NULL));
	Window w("Blank Project!", 1280,720,false); //This is all boring win32 window creation stuff!
	if(!w.HasInitialised()) {
		return -1;
	}

	Renderer renderer(w); //This handles all the boring OGL 3.2 initialisation stuff, and sets up our tutorial!
	if(!renderer.HasInitialised()) {
		return -1;
	}

	w.LockMouseToWindow(true);
	w.ShowOSPointer(false);

	while(w.UpdateWindow() && !Window::GetKeyboard()->KeyDown(KEYBOARD_ESCAPE)){


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




		renderer.UpdateScene(w.GetTimer()->GetTimeDeltaSeconds());
		renderer.RenderScene();
		renderer.SwapBuffers();
		if (Window::GetKeyboard()->KeyDown(KEYBOARD_F5)) {
			Shader::ReloadAllShaders();
		}
	}

	return 0;
}