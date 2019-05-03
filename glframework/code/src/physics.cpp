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
	glm::vec3 Sphereposition(1.f, 5.f, 0.f);
	
	float epsilon=0;
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
struct stateV {
	glm::vec3 position;
	glm::vec3 linearMomentum = { RandomFloat(0.1,2),RandomFloat(0.1,2) ,RandomFloat(0.1,2) };//
	glm::vec3 angularMomentum = { RandomFloat(0.1,2),RandomFloat(0.1,2) ,RandomFloat(0.1,2) };
};
struct RigidSphere : Collider {
	glm::mat3  inertiaTensor;//inerta tensor
	glm::vec3 velocity;
	stateV stateV;
	
	glm::vec3 initPos = { RandomFloat(-5,5),RandomFloat(0,10) ,RandomFloat(-5,5) };
	float mass =RandomFloat(0.1,2);
	float radius = RandomFloat(0.1, 2);
	
	void restart() 
	{
		stateV.linearMomentum = { RandomFloat(0.1,2),RandomFloat(0.1,2) ,RandomFloat(0.1,2) };
		stateV.angularMomentum = { RandomFloat(0.1,2),RandomFloat(0.1,2) ,RandomFloat(0.1,2) };
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
struct SphereCollider : Collider {
	glm::vec3 sphere_normal = { 0,0,0 };
	glm::vec3 sphere_normal2 = { 0,0,0 };
	float sphere_d;
	bool checkCollision(const glm::vec3& prev_pos, const glm::vec3& next_pos) {
		float checkCollisionX_prev = 0;
		float checkCollisionY_prev = 0;
		float checkCollisionZ_prev = 0;
		float checkCollision_prev = 0;
		float checkCollisionX_next = 0;
		float checkCollisionY_next = 0;
		float checkCollisionZ_next = 0;
		float checkCollision_next = 0;
		glm::vec3 intersectionPoint1 = { 0,0,0 };
		glm::vec3 intersectionPoint2 = { 0,0,0 };
		glm::vec3 vectorpFinal = { 0,0,0 };
		glm::vec3 equacionRecta = { 0,0,0 };
		float a, b;


		//comprobamos si la prev_pos ha colisionado o no
		checkCollisionX_prev = prev_pos.x - Sphere::Sphereposition.x;
		checkCollisionY_prev = prev_pos.y - Sphere::Sphereposition.y;
		checkCollisionZ_prev = prev_pos.z - Sphere::Sphereposition.z;
		checkCollision_prev = sqrt(pow(checkCollisionX_prev, 2) + pow(checkCollisionY_prev, 2) + pow(checkCollisionZ_prev, 2));
		//comprobamos si la next_pos a colisionado o no
		checkCollisionX_next = next_pos.x - Sphere::Sphereposition.x;
		checkCollisionY_next = next_pos.y - Sphere::Sphereposition.y;
		checkCollisionZ_next = next_pos.z - Sphere::Sphereposition.z;
		checkCollision_next = sqrt(pow(checkCollisionX_next, 2) + pow(checkCollisionY_next, 2) + pow(checkCollisionZ_next, 2));
		//la particula solo colisiona si el prev_pos es más grande que la esfera del radio y si el next_pos es más pequeño que el radio
	/*	for (int i = 0; i < 3; i++) {
			if ((Spheres[i]->radius < checkCollision_prev) && (Spheres[i]->radius > checkCollision_next))
			{
				glm::intersectLineSphere(prev_pos, next_pos, Sphere::Sphereposition, Spheres[i]->radius, intersectionPoint1, sphere_normal, intersectionPoint2, sphere_normal2);
				a = sqrt(pow(prev_pos.x - intersectionPoint1.x, 2) + pow(prev_pos.y - intersectionPoint1.y, 2) + pow(prev_pos.z - intersectionPoint1.z, 2));
				b = sqrt(pow(prev_pos.x - intersectionPoint2.x, 2) + pow(prev_pos.y - intersectionPoint2.y, 2) + pow(prev_pos.z - intersectionPoint2.z, 2));
				if (a < b)
				{
					sphere_d = -((intersectionPoint1.x*sphere_normal.x) + (intersectionPoint1.y*sphere_normal.y) + (intersectionPoint1.z*sphere_normal.z));
				}
				else if (a > b) {
					sphere_d = -((intersectionPoint2.x*sphere_normal2.x) + (intersectionPoint2.y*sphere_normal2.y) + (intersectionPoint2.z*sphere_normal2.z));
				}
				return true;
			}
			else
			{
				return false;
			}
		}*/

	}
	void getPlane(glm::vec3& normal, float& d)
	{
		normal = sphere_normal;
		d = sphere_d;
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

void euler(float dt, RigidSphere* sph)
{
	/*for (RigidSphere*col : colliders) {
		if (col->checkCollision(sph->stateV.position, sph->radius)) {
			//tenim el collider i la normal del collider
			//calculem inertia matrix
			glm::vec3 ra = glm::vec3(sph->radius - sph->stateV.position.x, sph-> radius - sph->stateV.position.y, sph->radius - sph->stateV.position.x);

			sph->inertiaTensor = glm::mat3(sph->mass*((pow(ra.y, 2)) + pow(ra.z, 2)), -sph->mass*ra.x*ra.y, -sph->mass*ra.x*ra.z,
				-sph->mass*ra.y*ra.x, sph->mass*((pow(ra.x, 2)) + pow(ra.z, 2)), -sph->mass*ra.y*ra.z,
				-sph->mass*ra.z*ra.x, -sph->mass*ra.z*ra.y, sph->mass*((pow(ra.x, 2)) + pow(ra.y, 2)));

			glm::vec3 rb = glm::vec3(col->radius - col->stateV.position.x,
				col->radius - col->stateV.position.y,
				col->radius - col->stateV.position.z);


			col->inertiaTensor = glm::mat3(col->mass*((pow(rb.y, 2)) + pow(rb.z, 2)), -col->mass*rb.x*rb.y, -col->mass*rb.x*rb.z,
				-col->mass*rb.y*rb.x, col->mass*((pow(rb.x, 2)) + pow(rb.z, 2)), -col->mass*rb.y*rb.z,
				-col->mass*rb.z*rb.x, -col->mass*rb.z*rb.y, col->mass*((pow(rb.x, 2)) + pow(rb.y, 2)));

			//computeImpulseCorrection()
			computeImpulseCorrection(sph->mass, ra, sph->inertiaTensor,
				col->mass, rb, col->inertiaTensor,
				sph->velocity, Sphere::epsilon,/*getnormal);

			//update colliders
		}
		if (plane1->checkCollision(col->stateV.position, col->radius) {
			//fer el rebot amb la plane1 normal
		}

	}*/
	if (planeColliderDown->checkCollision(sph->initPos, sph->radius)) std::cout << "COLISON PLANO ABAJO" << std::endl;
	if (planeColliderUp->checkCollision(sph->initPos, sph->radius))	std::cout << "COLISON PLANO ARRIBA" << std::endl;

	if (planeColliderLeft->checkCollision(sph->initPos, sph->radius))std::cout << "COLISON PLANO LEFT" << std::endl;
	if (planeColliderRight->checkCollision(sph->initPos, sph->radius))std::cout << "COLISON PLANO RIGHT" << std::endl;

	if (planeColliderFront->checkCollision(sph->initPos, sph->radius))std::cout << "COLISON PLANO FRONT" << std::endl;
	if (planeColliderBack->checkCollision(sph->initPos, sph->radius))std::cout << "COLISON PLANO BACK" << std::endl;

	sph->stateV.linearMomentum += dt * glm::vec3(gravity.x, gravity.y, gravity.z);
	sph->velocity = sph->stateV.linearMomentum / sph->mass;
	sph->stateV.position += dt * sph->velocity;
}

void PhysicsUpdate(float dt)
{
	if (play) {


		chrono += dt;

		for (int i = 0; i < Spheres.size(); i++)
		{
			euler(dt, Spheres[i]);
		}

		//std::cout << chrono << std::endl;

		if (chrono >= 15)
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