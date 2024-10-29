#include <iostream>
#include <SDL.h>
#include "Screen.h"
#include "Input.h"
#include "gl.h"
#include "Shader.h"
#include "Quad.h"
#include "Camera.h"
#include "Light.h"
#include "Cube.h"
#include "Model.h"

#include "Chunk.h"
#include "World.h"

#include <SDL.h>

// Global variables
Uint32 frameStartTime = 0;
float deltaTime = 0.0f;
float fps = 0.0f;

void StartFrame() {
	frameStartTime = SDL_GetTicks();
}

void EndFrame() {
	Uint32 frameEndTime = SDL_GetTicks();
	Uint32 frameTime = frameEndTime - frameStartTime;

	// Calculate delta time in seconds
	deltaTime = frameTime / 1000.0f;

	// Avoid division by zero
	if (deltaTime > 0.0f) {
		fps = 1.0f / deltaTime;
	}
	else {
		fps = 0.0f;
	}
}



bool isAppRunning = true;

int main(int argc, char* argv[])
{
	Screen::Instance()->Initialize();
	
	Shader lightShader;
	lightShader.Create("Shaders/Main.vert", "Shaders/Main.frag");
	

	Shader wireframeShader;
	wireframeShader.Create("wireframe_vertex.glsl", "wireframe_fragment.glsl");
	


	float xPos = 0.0f;
	float yPos = 0.0f;

	//================================================================

	Quad quad;
	Cube cube;
	Model model;
	model.Load("Models/Unreal_engineer_quali_1026155640_refine.obj");

	/*std::vector<Chunk> chunks;
	for (int x = -1; x <= 1; ++x) {
		for (int z = -1; z <= 1; ++z) {
			chunks.emplace_back(x, z);
		}
	}*/

	World world;

	Camera camera;
	camera.Set3DView();

	Light light;

	//================================================================

	while (isAppRunning)
	{
		StartFrame();

		Screen::Instance()->ClearScreen();

		Input::Instance()->Update();

		char keyPressed = Input::Instance()->GetKeyDown();

		isAppRunning = !Input::Instance()->IsXClicked();

		glm::vec3 playerPosition = camera.GetPosition();
		world.Update(playerPosition);

		camera.Update();
		lightShader.Use();
		camera.SendToShader(lightShader);

		light.Update();
		light.Render(lightShader);
		light.SendToShader(lightShader);

		//quad.Update();
		//quad.Render(lightShader);

		cube.Update();
		cube.Render(lightShader);

		/*model.Update();
		model.Render(lightShader);*/

		/*for (auto& chunk : chunks) {
			chunk.Update();
			chunk.Render(lightShader);
		}*/

		
		world.Render(lightShader);

		wireframeShader.Use();
		camera.SendToShader(wireframeShader);
		world.RenderWireframes(wireframeShader);
		

		Screen::Instance()->Present();

		EndFrame();

		std::cout << "FPS: " << fps << std::endl;

	}

	lightShader.Destroy();	
	wireframeShader.Destroy();

	Screen::Instance()->Shutdown();	

	return 0;
}