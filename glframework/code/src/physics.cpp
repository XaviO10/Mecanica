#include <imgui\imgui.h>
#include <imgui\imgui_impl_sdl_gl3.h>
#include <glm\gtc\matrix_transform.hpp>
#include <vector>
#include <iostream>

bool play = true;

namespace Box {
	void drawCube();
}
namespace Axis {
	void drawAxis();
}

namespace Sphere {
	extern void updateSphere(glm::vec3 pos, float radius = 1.f);
	extern void drawSphere();
}
namespace Capsule {
	extern void updateCapsule(glm::vec3 posA, glm::vec3 posB, float radius = 1.f);
	extern void drawCapsule();
}
namespace Particles {
	extern const int maxParticles;
	extern void updateParticles(int startIdx, int count, float* array_data);
	extern void drawParticles(int startIdx, int count);
}
namespace Mesh {
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
namespace Cube {
	extern void updateCube(const glm::mat4& transform);
	extern void drawCube();
}


// Boolean variables allow to show/hide the primitives
bool renderSphere = true;
bool renderCapsule = false;
bool renderParticles = false;
bool renderMesh = true;
bool renderFiber = false;
bool renderCube = false;

//You may have to change this code
void renderPrims() {

	Box::drawCube();
	Axis::drawAxis();

	if (renderSphere)
		Sphere::drawSphere();

	if (renderCapsule)
		Capsule::drawCapsule();

	if (renderParticles) {
		int startDrawingFromParticle = 0;
		int numParticlesToDraw = Particles::maxParticles;
		Particles::drawParticles(startDrawingFromParticle, numParticlesToDraw);
	}

	if (renderMesh)
		Mesh::drawMesh();
	if (renderFiber)
		Fiber::drawFiber();

	if (renderCube)
		Cube::drawCube();
}

struct FluidSystem
{
	std::vector<glm::vec3> fluidPoints;
	//glm::vec3 fluidPoints[14][18];
	std::vector<glm::vec3> firstPos;
	glm::vec3 y = glm::vec3(0.0f, 1.0f, 0.0f);

	glm::vec3 waveVec = glm::vec3(1.0f, 0.0f, 1.0f);

	float time, t, w, A, waveMod, density, gravity, vSub;
	float meshSize = 0.60;  //Cambiar tamany mesh

	float* fillPositions()
	{
		float* posArray = new float[fluidPoints.size() * 3];
		for (int i = 0; i < fluidPoints.size(); i++)
		{
			posArray[i * 3 + 0] = fluidPoints[i].x;
			posArray[i * 3 + 1] = fluidPoints[i].y;
			posArray[i * 3 + 2] = fluidPoints[i].z;
		}
		return posArray;
	}

};

struct BuoyantSphere {
	float radius = 1.f;
	glm::vec3 sphPos = { 0,5,0 };
};

FluidSystem *fluidSys = new FluidSystem();
BuoyantSphere *sphere = new BuoyantSphere();

void updateSphere(FluidSystem* FLSys, BuoyantSphere* BSph,
	float accum_time, float dt) {


}

/*glm::vec3 getInitPos(int i, int j, float initY = 3.f)
{
return;
}*/

glm::vec3 getGerstnerPos(FluidSystem* FLSys, int i, int j, float accum_time = 0.f)
{
	FLSys->t += accum_time;
	float yPoints = 0.0f;
	int countPoints = 0;

	glm::vec3 newPos;

	glm::vec3 v = (FLSys->waveVec / FLSys->waveMod)*FLSys->A*sin(dot(FLSys->waveVec, glm::vec3(FLSys->firstPos[i * 18 + j])) - FLSys->w * FLSys->t);
	newPos.x = FLSys->firstPos[i * 18 + j].x - v.x;
	newPos.z = FLSys->firstPos[i * 18 + j].z - v.z;
	newPos.y = ((glm::vec3(FLSys->A*cos(dot(FLSys->waveVec, glm::vec3(FLSys->firstPos[i * 18 + j])) - FLSys->w * FLSys->t))).y) + 3;


	return newPos;
}

void PhysicsInit() {
	fluidSys->t = 0.0f;
	if (fluidSys->w < 1.0f)fluidSys->w = 3.0f;
	fluidSys->A = 0.2f;//amplitud
	fluidSys->waveMod = fluidSys->waveVec.length();//
	fluidSys->gravity = -9.8f;//gravedad
	fluidSys->density = 15.0f;//densidad

	//Volum Desplasat
	fluidSys->vSub = 0.0f;


	float x;
	float z = -5.5f;
	float y = 3.0f;

	for (int i = 0; i < Mesh::numRows; i++)
	{
		z += fluidSys->meshSize;
		x = -5.5f;
		for (int j = 0; j < Mesh::numCols; ++j)
		{
			x += fluidSys->meshSize;
			glm::vec3 pos(x, y, z);
			fluidSys->fluidPoints.push_back(pos);
			fluidSys->firstPos.push_back(pos);
		}
	}
	Mesh::updateMesh(fluidSys->fillPositions());

	Sphere::updateSphere(sphere->sphPos, sphere->radius);
}
void PhysicsUpdate(float dt) {
	//TODO
	if (play)
	{
		fluidSys->time += dt;
		if (fluidSys->time >= 4)
		{
			PhysicsInit();
			fluidSys->time = 0;
		}
		else
		{
			//particles.Update(dt);
			Mesh::updateMesh(fluidSys->fillPositions());

			for (int i = 0; i < Mesh::numCols; ++i)
			{
				for (int j = 0; j < Mesh::numRows; ++j)
				{
					fluidSys->fluidPoints[i * 18 + j] = getGerstnerPos(fluidSys, i, j, dt);
				}
			}
		}
	}

}

void PhysicsCleanup() {
	//Mesh
	fluidSys->fluidPoints.clear();
	fluidSys->firstPos.clear();
}


void GUI() {
	bool show = true;
	ImGui::Begin("Physics Parameters", &show, 0);

	// Do your GUI code here....
	{
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);//FrameRate
		ImGui::Checkbox("Play simulation", &play);
		if (ImGui::Button("Reset", ImVec2(50, 20)))
		{
			PhysicsCleanup();
			PhysicsInit();
		}
		if (ImGui::TreeNode("Waves"))
		{
			ImGui::DragFloat("Amplitude_0", &fluidSys->A, 0.1f, 0.0f, 0.5f, "%.1f");
			ImGui::DragFloat("Frequency_0", &fluidSys->w, 0.1f, 1.0f, 10.0f, "%.1f");
			ImGui::DragFloat("WaveMod_0", &fluidSys->waveMod, 0.1f, 1.0f, 10.0f, "%.1f");
			ImGui::DragFloat3("WaveVec_0", &(fluidSys->waveVec.x, fluidSys->waveVec.y, fluidSys->waveVec.z), 0.1f, 1.0f, 10.0f, "%.1f");

			/*ImGui::DragFloat("Amplitude_1", &fluidSys->A, 0.1f, 1.0f, 10.0f, "%.1f");
			ImGui::DragFloat("Frequency_1", &fluidSys->w, 0.1f, 1.0f, 10.0f, "%.1f");
			ImGui::DragFloat("WaveMod_1", &fluidSys->kItalic, 0.1f, 1.0f, 10.0f, "%.1f");
			ImGui::DragFloat3("WaveVec_1", &(fluidSys->kBlood.x, fluidSys->kBlood.y, fluidSys->kBlood.z), 0.1f, 1.0f, 10.0f, "%.1f");*/

			ImGui::TreePop();
		}
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