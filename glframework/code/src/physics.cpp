
#include <imgui\imgui.h>
#include <imgui\imgui_impl_sdl_gl3.h>
#include <glm\gtc\matrix_transform.hpp>
#include <iostream>
#include <vector>
bool play;
glm::vec3 gravity = { 0,1,0 };
namespace Sphere {
	extern void updateSphere(glm::vec3 pos, float radius = 1.f);
	extern void drawSphere();
	glm::vec3 Sphereposition(1.f, 8.f, 1.f);
	float mass = 50.f;
	float rad = 1.f;
}

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
	std::vector<glm::vec3> position;
	float posY;
	float A;
	float omega;
	glm::vec3 k;
	wave()
	{
		A = RandomFloat(0.1, 0.9);
		omega = RandomFloat(0.1, 0.9);
		k = { RandomFloat(-2,2), RandomFloat(-2,2), RandomFloat(-2,2) };
	}
};

struct FluidSystem
{
	const int NumOfWaves = 4;
	wave waves;

	/*void fill()
	{
		for (int i = 0; i < NumOfWaves; i++)
		{
			wave* temp = new wave;
			waves.push_back(*temp);
		}
	}*/


};
float *data;
std::vector<FluidSystem> FLSys;

/*glm::vec3 getInitPos(int i, int j, float initY = 3.f)
{

}
glm::vec3 getGerstnerPos(FluidSystem* FLSys, int i, int j, float accum_time = 0.f) {
	i= i - (FLSys->initPos / FLSys->k)*FLSys->A*sin(FLSys->k* FLSys->initPos.x - FLSys->w * accum_time + FLSys->fase);
	j = FLSys->A * cos(FLSys->k*FLSys->initPos.x - FLSys->w * accum_time + FLSys->fase);
	return FLSys; //devuelvela nueva posicion
}*/

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
	for (int i = 0; i < 2; i++)
	{
		data = &FLSys[i].waves.position[0].x;
		Mesh::updateMesh(data);
		Fiber::drawFiber();
	}


	if (renderFiber)
		Fiber::drawFiber();
}



void PhysicsInit() {
	// Do your initialization code here...
	for (int i = 0; i < 99; i++)
	{
		FluidSystem tempFiber;

		glm::vec3 pos = { RandomFloat(-4,4),3, RandomFloat(-4,4) };
		tempFiber.waves.position.push_back(pos);


		for (int j = 1; j < 5; j++)
		{
			tempFiber.waves.position.push_back({ tempFiber.waves.position[0].x, tempFiber.waves.position[0].y,tempFiber.waves.position[0].z });


		}
		FLSys.push_back(tempFiber);
	}
	data = &FLSys[0].waves.position[0].x;
	Fiber::updateFiber(data);
	// ...................................
}




// ...................................


void PhysicsUpdate(float dt) {
	if (play) {

	}
}

void PhysicsCleanup() {
	// Do your cleanup code here...
	// ............................
}
void GUI() {
	ImGuiWindowFlags window_flags = 0;
	bool show = true;

	bool pause = true;
	ImGui::Begin("Physics Parameters", &show, 0);

	// Do your GUI code here....
	{

		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);//FrameRate
		ImGui::Checkbox("Play/pause", &play);

		if (ImGui::Button("ResetSimulation", ImVec2(650, 20))) {
			PhysicsCleanup();
			PhysicsInit();
		}


		ImGui::DragFloat("SphereMass_0", &Sphere::mass);

		if (ImGui::TreeNode("Waves")) {
			/*ImGui::DragFloat("Amplitude_0", static_cast<float*>(&fluid.waves));
			ImGui::DragFloat("Frequency_0", static_cast<float*>(&fluid.w));
		//	ImGui::DragFloat("WaveMod_0", &Spheres[1]->mass);
			//ImGui::DragFloat3("WaveVec_0", &Spheres[1]->radius, 0);
			ImGui::DragFloat("Amplitude_1", static_cast<float*>(&fluid.A));
			ImGui::DragFloat("Frequency_1", static_cast<float*>(&fluid.w));*/
			//ImGui::DragFloat("WaveMod_1", static_cast<float*>(&fluid.waveVector);
			//ImGui::DragFloat3("WaveVec_1", fluid.waveVector);

			ImGui::TreePop();
		}

	}

	ImGui::End();

	// Example code -- ImGui test window. Most of the sample code is in ImGui::ShowTestWindow()
	bool show_test_window = false;
	if (show_test_window) {
		ImGui::SetNextWindowPos(ImVec2(650, 20), ImGuiSetCond_FirstUseEver);
		ImGui::ShowTestWindow(&show_test_window);
	}
}