#include <imgui\imgui.h>
#include <imgui\imgui_impl_sdl_gl3.h>
#include <glm\gtc\matrix_transform.hpp>
#include <vector>
#include <windows.h>
#include <iostream>
#include <glm/gtx/intersect.hpp>

static bool play = true;
glm::vec3 gravity = { 0,1,0 };
float elasticity = 0.5;
#define MAX_SPHERES 3

namespace Box {
	void drawCube();
}
namespace Axis {
	void drawAxis();
}

namespace Sphere {
	extern void updateSphere(glm::vec3 pos, float radius);
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

	
	float inertiaTensor = 1;
	glm::vec3 velocity;
	glm::vec3 linearMomentum = {RandomFloat(0.1,2),RandomFloat(0.1,2) ,RandomFloat(0.1,2) };
	glm::vec3 angularMomentum = { RandomFloat(0.1,2),RandomFloat(0.1,2) ,RandomFloat(0.1,2) };
	glm::vec3 initPos = { RandomFloat(-5,5),RandomFloat(0,10) ,RandomFloat(-5,5) };
	float mass =RandomFloat(0.1,2);
	float radius = RandomFloat(0.1, 2);
	
	void restart() 
	{
		linearMomentum = { RandomFloat(0.1,2),RandomFloat(0.1,2) ,RandomFloat(0.1,2) };
		angularMomentum = { RandomFloat(0.1,2),RandomFloat(0.1,2) ,RandomFloat(0.1,2) };
		initPos = { RandomFloat(-5,5),RandomFloat(0,10) ,RandomFloat(-5,5) };
		mass = RandomFloat(0.1, 2);
		radius = RandomFloat(0.1, 2);
	}

	bool checkCollision(const glm::vec3& next_pos, float radius) override 
	{
		



		return false;
	}
};
struct PlaneCol : Collider 
{
	float p_d;
	glm::vec3 p_normal;

	PlaneCol(glm::vec3 _nrmal, float _d)
	{
		p_normal = _nrmal;
		p_d = _d;
	}

	bool checkCollision(const glm::vec3& next_pos, float radius) override 
	{
		float distanceNext = ((glm::dot(next_pos, p_normal)) + p_d) / glm::length(p_normal);
		if (distanceNext < radius) 
		{
			
			return true;
		}
		else return false;
		
	}
};

std::vector<RigidSphere*> Spheres;
Collider *planeColliderDown = new PlaneCol(glm::vec3{ 0,1,0 }, 0);
Collider *planeColliderUp = new PlaneCol(glm::vec3{ 0,-1,0 }, 10);

Collider *planeColliderLeft = new PlaneCol(glm::vec3{ 1,0,0 }, 5);
Collider *planeColliderRight = new PlaneCol(glm::vec3{ -1,0,0 }, 5);

Collider *planeColliderFront = new PlaneCol(glm::vec3{ 0,0,-1 }, 5);
Collider *planeColliderBack = new PlaneCol(glm::vec3{ 0,0,1 }, 5);

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
			Sphere::updateSphere(Spheres[i]->initPos, Spheres[i]->radius);
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

void PhysicsInit()
{	

	for (int i = 0; i < MAX_SPHERES; i++) 
	{
		RigidSphere* a = new RigidSphere;
		Spheres.push_back(a);
	}
	
}

void euler(float dt, RigidSphere& sph) 
{
	if(planeColliderDown->checkCollision(sph.initPos, sph.radius)) std::cout << "COLISON PLANO ABAJO" << std::endl;
	if(planeColliderUp->checkCollision(sph.initPos, sph.radius))	std::cout << "COLISON PLANO ARRIBA" << std::endl;

	if(planeColliderLeft->checkCollision(sph.initPos, sph.radius))std::cout << "COLISON PLANO LEFT" << std::endl;
	if(planeColliderRight->checkCollision(sph.initPos, sph.radius))std::cout << "COLISON PLANO RIGHT" << std::endl;
	
	if(planeColliderFront->checkCollision(sph.initPos, sph.radius))std::cout << "COLISON PLANO FRONT" << std::endl;
	if(planeColliderBack->checkCollision(sph.initPos, sph.radius))std::cout << "COLISON PLANO BACK" << std::endl;
	//sph.velocity = sph.linearMomentum / sph.mass;
	//sph.linearMomentum = sph.linearMomentum + (dt*sph.mass*sph.velocity);
}


void PhysicsUpdate(float dt)
{
	chrono += dt;

	for (int i = 0; i < Spheres.size(); i++)
	{
		euler(dt,*Spheres[i]);
	}

	//std::cout << chrono << std::endl;

	if (chrono >= 3)
	{
		for (int i = 0; i < Spheres.size(); i++)
		{
			for (std::vector<RigidSphere*>::iterator it = Spheres.begin(); it != Spheres.end(); ++it)
			{
				delete (*it);
			}
			Spheres.clear();
			PhysicsInit();
		}
		chrono = 0;
	}

}

void PhysicsCleanup()
{
	for (std::vector<RigidSphere*>::iterator it = Spheres.begin(); it != Spheres.end(); ++it)
	{
		delete (*it);
	}
	Spheres.clear();
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
		
		
		ImGui::DragFloat3("GravityAccel", static_cast<float*>(&gravity.x));
		ImGui::DragFloat3("Elasticity Coefficient", &elasticity);

		if (ImGui::TreeNode("Spheres")) {
			ImGui::DragFloat("SphereMass_0", &Spheres[0]->mass);
			ImGui::DragFloat("SphereRadius_0", &Spheres[0]->radius, 0); 
			ImGui::DragFloat("SphereMass_1", &Spheres[1]->mass);
			ImGui::DragFloat("SphereRadius_1", &Spheres[1]->radius, 0);
			ImGui::DragFloat("SphereMass_2", &Spheres[2]->mass);
			ImGui::DragFloat("SphereRadius_2", &Spheres[2]->radius, 0);

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