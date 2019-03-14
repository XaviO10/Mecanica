#include <imgui\imgui.h>
#include <imgui\imgui_impl_sdl_gl3.h>
#include <glm\gtc\matrix_transform.hpp>
#include <iostream>
#include <vector>
struct Particle 
{
	std::vector<glm::vec3> positions;
	std::vector<glm::vec3> velocities;
	std::vector<float> mass;
};

struct FiberStraw 
{
	float elasticity;
	float Damping;
	float OriginaLength;
	Particle particles;
};

std::vector<FiberStraw> fibers;
float* data;
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
bool renderSphere = false;
bool renderCapsule = false;
bool renderParticles = true;
bool renderMesh = false;
bool renderFiber = true;
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
		int numParticlesToDraw = 500;
		Particles::drawParticles(startDrawingFromParticle, numParticlesToDraw);
	}

	if (renderMesh)
		Mesh::drawMesh();
	
	if (renderFiber) 
	{
		int startDrawingFromFiber = 0;
		int numFibersToDraw = 100;
		for (int i = 0; i < 99; i++)
		{
			data = &fibers[i].particles.positions[0].x;
			Fiber::updateFiber(data);
			Fiber::drawFiber();
		}
		
	}
		

	if (renderCube)
		Cube::drawCube();
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
	if(show_test_window) {
		ImGui::SetNextWindowPos(ImVec2(650, 20), ImGuiSetCond_FirstUseEver);
		ImGui::ShowTestWindow(&show_test_window);
	}
}

float randomFloat(float a, float b) {
	float random = ((float)rand()) / (float)RAND_MAX;
	float diff = b - a;
	float r = random * diff;
	return a + r;
}

void PhysicsInit() {
	// Do your initialization code here...
	for (int i = 0; i < 99; i++)
	{
		FiberStraw tempFiber;
		tempFiber.Damping = randomFloat(0.1, 1);
		tempFiber.elasticity = randomFloat(0.1, 1);
		tempFiber.OriginaLength = randomFloat(0.5, 2);
		tempFiber.particles.positions.push_back({randomFloat(-5,5),randomFloat(0,5), randomFloat(-5,5)});
		tempFiber.particles.velocities.push_back({randomFloat(-1,1),randomFloat(-1,1), randomFloat(-1,1) });
		tempFiber.particles.mass.push_back(randomFloat(0.1,1));
		
		for (int j = 1; j < 5; j++)
		{
			tempFiber.particles.positions.push_back({ tempFiber.particles.positions[0].x,	tempFiber.particles.positions[j-1].y+ tempFiber.OriginaLength, tempFiber.particles.positions[0].z });
			tempFiber.particles.velocities.push_back({ randomFloat(-1,1),randomFloat(-1,1), randomFloat(-1,1) });
			tempFiber.particles.mass.push_back(randomFloat(0.1, 1));
		}
		fibers.push_back(tempFiber);
	}
	
		data = &fibers[0].particles.positions[0].x;
		Fiber::updateFiber(data);
	
	
	
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