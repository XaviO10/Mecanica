#include <imgui\imgui.h>
#include <imgui\imgui_impl_sdl_gl3.h>
#include <glm\gtc\matrix_transform.hpp>
#include <iostream>

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

float RandomFloat(float a, float b) {
	float random = ((float)rand()) / (float)RAND_MAX;
	float diff = b - a;
	float r = random * diff;
	return a + r;
}

struct Collider {
	virtual bool checkCollision(const glm::vec3& next_pos, float radius) = 0;
};

struct RigidSphere : Collider {

	void initShphere()
	{
		glm::vec3 linearMomentum = {RandomFloat(0.1,2),RandomFloat(0.1,2) ,RandomFloat(0.1,2) };
		glm::vec3 angularMomentum = { RandomFloat(0.1,2),RandomFloat(0.1,2) ,RandomFloat(0.1,2) };
		glm::vec3 initPos = { RandomFloat(-5,5),RandomFloat(0,10) ,RandomFloat(-5,5) };
		float mass =RandomFloat(0.1,2);
	
	}
	bool checkCollision(const glm::vec3& next_pos, float radius) override {
		//...
	}
};

// Boolean variables allow to show/hide the primitives
bool renderSphere = true;
bool renderCapsule = false;
bool renderParticles = false;
bool renderMesh = false;
bool renderFiber = false;
bool renderCube = false;

float chrono=0;


//You may have to change this code
void renderPrims() {
	Box::drawCube();
	Axis::drawAxis();

	if (renderSphere) 
	{		
		for (int i = 0; i < 3; i++)
		{
			glm::vec3 initPos = { RandomFloat(-4,4), RandomFloat(1,9) ,RandomFloat(-4,4) };
			Sphere::updateSphere(initPos, 1);
			Sphere::drawSphere();
		}		
	}

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

void PhysicsInit() {
	
	
	
}

void euler(float dt, RigidSphere& sph) 
{
	
}


void PhysicsUpdate(float dt) 
{
	chrono += dt;
	std::cout << chrono<< std::endl;
	if (chrono >= 15) 
	{
		for (int i = 0; i < 3; i++)
		{
			glm::vec3 initPos = { RandomFloat(-4,4), RandomFloat(1,9) ,RandomFloat(-4,4) };
			Sphere::updateSphere(initPos, 1);
			Sphere::drawSphere();
		}
		chrono = 0;
	}

}

void PhysicsCleanup() {
	


}