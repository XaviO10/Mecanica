

#include <imgui\imgui.h>
#include <imgui\imgui_impl_sdl_gl3.h>
#include <glm\gtc\matrix_transform.hpp>
#include <iostream>
#include <vector>
namespace Box {
	void drawCube();
}
namespace Axis {
	void drawAxis();
}
namespace Mesh {
	//18 filas
	extern const int numCols;
	extern const int numRows;
	extern void updateMesh(float* array_data);
	extern void drawMesh();
}
namespace Fiber {
	extern const int numVerts;
	extern void updateFiber(float* array_data);
	extern void drawFiber();
}

float RandomFloat(float a, float b) {
	float random = ((float)rand()) / (float)RAND_MAX;
	float diff = b - a;
	float r = random * diff;
	return a + r;
}

struct wave
{
	glm::vec3 pos;
	float posY;
	float A;
	float omega;
	glm::vec3 k;
	wave()
	{
		pos = { RandomFloat(-4,4), RandomFloat(1,9), RandomFloat(-4,4) };
		A = RandomFloat(0.1, 0.9);
		omega = RandomFloat(0.1, 0.9);
		k = { RandomFloat(-2,2), RandomFloat(-2,2), RandomFloat(-2,2) };
	}
};

struct FluidSystem
{
	const int NumOfWaves = 2;
	std::vector <wave> waves;

	void fill()
	{
		for (int i = 0; i < NumOfWaves; i++)
		{
			wave* temp = new wave;
			waves.push_back(*temp);
		}
	}


};

FluidSystem FLSys;



// Boolean variables allow to show/hide the primitives
bool renderSphere = false;
bool renderMesh = true;
bool renderFiber = false;

//You may have to change this code
void renderPrims() {
	Box::drawCube();
	Axis::drawAxis();

	if (renderMesh)
		Mesh::drawMesh();


	if (renderFiber)
		Fiber::drawFiber();
}

void GUI() {
	bool show = true;
	ImGui::Begin("Physics Parameters", &show, 0);

	// Do your GUI code here....
	{
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);//FrameRate

	}
	// .........................

	ImGui::End();

	// Example code -- ImGui test window. Most of the sample code is in ImGui::ShowTestWindow()
	bool show_test_window = false;
	if (show_test_window) {
		ImGui::SetNextWindowPos(ImVec2(650, 20), ImGuiSetCond_FirstUseEver);
		ImGui::ShowTestWindow(&show_test_window);
	}
}

void PhysicsInit() {
	// Do your initialization code here...
	FLSys.fill();




	// ...................................
}

void PhysicsUpdate(float dt) {
	// Do your update code here...
	// ...........................
}

void PhysicsCleanup() {
	// Do your cleanup code here...
	// ............................
}