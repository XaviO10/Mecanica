#include <imgui\imgui.h>
#include <imgui\imgui_impl_sdl_gl3.h>
#include <glm\gtc\matrix_transform.hpp>
#include <iostream>
#include <vector>
#include <windows.h>
#include <iostream>
#include <glm/gtx/intersect.hpp>

glm::vec3 gravity = { 0,2,0 };
glm::vec3 wind = { 0,0,0 };
float gravityF[3] = { gravity.x,gravity.y,gravity.z };
float elasticity = 0.5;

namespace Box {
	void drawCube();
}
namespace Axis {
	void drawAxis();
}

namespace Sphere {
	extern void updateSphere(glm::vec3 pos, float radius = 1.f);
	extern void drawSphere();
	glm::vec3 Sphereposition(1.f, 2.f, 0.f);
	float massSphere = 1.f;
	float rad = 1.f;
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

struct Particle 
{
	std::vector<glm::vec3> positions;
	std::vector<glm::vec3> last_position;
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

struct ForceActuator
{
	virtual glm::vec3 computeForce(float mass, const glm::vec3& position) = 0;
};

struct GravityForce :ForceActuator{

	glm::vec3 computeForce(float mass, const glm::vec3& position)
	{
		return gravity * mass;
	}
};

struct WindForce :ForceActuator {

	glm::vec3 computeForce(float mass, const glm::vec3& position)
	{
		return wind * mass;
	}
};

struct SpringForce :ForceActuator 
{
	glm::vec3 computeForce(float mass, const glm::vec3& position)
	{
		return glm::vec3(0,0,0);		
	}
};

glm::vec3 springforce(const glm::vec3& P1, const glm::vec3& V1, const glm::vec3& P2, const glm::vec3& V2, float L0, float ke, float kd) 
{
	return (-(ke*((P1 - P2) - L0) + kd * (V1 - V2)*((P1 - P2) / (P1 - P2)))*((P1 - P2) / (P1 - P2)));
}

struct Collider {

	float d;
	glm::vec3 normal;

	virtual bool checkCollision(const glm::vec3& prev_pos, const glm::vec3& next_pos) = 0;
	virtual void getPlane(glm::vec3& normal, float& d) = 0;

	void computeCollision(const glm::vec3& old_pos, const glm::vec3& old_vel, glm::vec3& new_pos, glm::vec3& new_vel)
	{
		getPlane(normal, d);
		new_pos = new_pos - (1 + elasticity) * ((glm::dot(normal, new_pos) + d)*normal);
		new_vel = new_pos - (1 + elasticity) * (glm::dot(normal, new_vel)*normal);
	}
};
struct PlaneCollider : Collider {

	float p_d;
	glm::vec3 p_normal;

	PlaneCollider(glm::vec3 _nrmal, float _d)
	{
		p_normal = _nrmal;
		p_d = _d;
	}

	bool checkCollision(const glm::vec3& prev_pos, const glm::vec3& next_pos)
	{
		float distancePrev = ((glm::dot(prev_pos, p_normal)) + p_d) / glm::length(p_normal);
		float distanceNext = ((glm::dot(next_pos, p_normal)) + p_d) / glm::length(p_normal);

		if (distancePrev > 0 && distanceNext <= 0) return true;
		else return false;
	}

	void getPlane(glm::vec3& normal, float& d)
	{
		normal = p_normal;
		d = p_d;
	}
};
struct SphereCollider : Collider {
	glm::vec3 sphere_normal = { 0,0,0 };
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
		checkCollisionZ_prev = prev_pos.y - Sphere::Sphereposition.z;
		checkCollision_prev = sqrt(pow(checkCollisionX_prev, 2) + pow(checkCollisionY_prev, 2) + pow(checkCollisionZ_prev, 2));
		//comprobamos si la next_pos a colisionado o no
		checkCollisionX_next = next_pos.x - Sphere::Sphereposition.x;
		checkCollisionY_next = next_pos.y - Sphere::Sphereposition.y;
		checkCollisionY_next = next_pos.z - Sphere::Sphereposition.z;
		checkCollision_next = sqrt(pow(checkCollisionX_next, 2) + pow(checkCollisionY_next, 2) + pow(checkCollisionZ_next, 2));
		//la particula solo colisiona si el prev_pos es más grande que la esfera del radio y si el next_pos es más pequeño que el radio
		if ((Sphere::rad < checkCollision_prev) && (Sphere::rad > checkCollision_next))
		{
			glm::intersectLineSphere(prev_pos, next_pos, Sphere::Sphereposition, Sphere::rad, intersectionPoint1, sphere_normal, intersectionPoint2);
			a = sqrt(pow(prev_pos.x - intersectionPoint1.x, 2) + pow(prev_pos.y - intersectionPoint1.y, 2) + pow(prev_pos.z - intersectionPoint1.z, 2));
			b = sqrt(pow(prev_pos.x - intersectionPoint2.x, 2) + pow(prev_pos.y - intersectionPoint2.y, 2) + pow(prev_pos.z - intersectionPoint2.z, 2));

			if (a < b) {
				sphere_d = -((intersectionPoint1.x*sphere_normal.x) + (intersectionPoint1.y*sphere_normal.y) + (intersectionPoint1.z*sphere_normal.z));
			}
			else if (a > b) {
				sphere_d = -((intersectionPoint2.x*sphere_normal.x) + (intersectionPoint2.y*sphere_normal.y) + (intersectionPoint2.z*sphere_normal.z));
			}
			//hace el rebote una vez y ya esta juraria, creo qu es mejor usaar if else que while
			//implementar las normales

		}
		else
		{
			return false;
		}

	}
	void getPlane(glm::vec3& normal, float& d)
	{
		normal = sphere_normal;
		d = sphere_d;
	}
};


std::vector<FiberStraw> fibers;
float* data;
std::vector<ForceActuator*> force_acts;
std::vector<Collider*> colliders;



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
float randomFloat(float a, float b) {
	float random = ((float)rand()) / (float)RAND_MAX;
	float diff = b - a;
	float r = random * diff;
	return a + r;
}
void verlet(float dt, FiberStraw &fiber,const std::vector<Collider*>& colliders,const std::vector<ForceActuator*>& force_acts)
{
	for (int i = 0; i < fiber.particles.mass.size(); i++)
	{
		glm::vec3 forces;
		for (int j = 0; j < force_acts.size(); j++)
		{
			forces += force_acts[j]->computeForce(fiber.particles.mass[i], fiber.particles.positions[i]);
		}
		fiber.particles.positions[i] = fiber.particles.positions[i]+(fiber.particles.positions[i]- fiber.particles.last_position[i])+forces*(dt*dt);
	}
	
}
	

void PhysicsInit() {
	// Do your initialization code here...
	GravityForce *gForce = new GravityForce;
	WindForce *wForce = new WindForce;
	SpringForce *sForce = new SpringForce;

	force_acts.push_back(gForce);
	force_acts.push_back(wForce);
	

	for (int i = 0; i < 99; i++)
	{
		FiberStraw tempFiber;
		tempFiber.Damping = randomFloat(0.1, 1);
		tempFiber.elasticity = randomFloat(0.1, 1);
		tempFiber.OriginaLength = 1.25;
		tempFiber.particles.positions.push_back({ randomFloat(-4,4),-1.5/*randomFloat(-1.5,-1.2)*/, randomFloat(-4,4) });
		tempFiber.particles.last_position.push_back({ randomFloat(-4,4),-1.5/*randomFloat(-1.5,-1.2)*/, randomFloat(-4,4) });
		tempFiber.particles.velocities.push_back({ randomFloat(-1,1),randomFloat(-1,1), randomFloat(-1,1) });
		tempFiber.particles.mass.push_back(randomFloat(0.1, 1));

		for (int j = 1; j < 5; j++)
		{
			tempFiber.particles.positions.push_back({ tempFiber.particles.positions[0].x,	tempFiber.particles.positions[j - 1].y + tempFiber.OriginaLength, tempFiber.particles.positions[0].z });
			tempFiber.particles.last_position.push_back({ tempFiber.particles.positions[0].x,	tempFiber.particles.positions[j - 1].y + tempFiber.OriginaLength, tempFiber.particles.positions[0].z });
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
	for (int i = 0; i < fibers.size(); i++)
	{
		verlet(dt, fibers[i], colliders, force_acts);
	}
	
	data = &fibers[0].particles.positions[0].x;
	Fiber::updateFiber(data);
	// ...........................*/
}

void PhysicsCleanup() {
	// Do your cleanup code here...


	// ............................
}
void GUI() {
	ImGuiWindowFlags window_flags = 0;
	bool show = true;
	static bool play = true;
	bool pause = true;
	ImGui::Begin("Physics Parameters", &show, 0);

	// Do your GUI code here....
	{

		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);//FrameRate
		ImGui::Checkbox("Play/pause", &play);
		if (!play)
			window_flags = 1;
		pause = !pause;




		if (ImGui::Button("ResetSimulation", ImVec2(650, 20))) {
			PhysicsCleanup();

		}
	/*	ImGui::DragFloat3("GravityControl", WFEdge1, gravity.x, gravity.y, gravity.z, 0);
		ImGui::DragFloat("Elasticity", &elasticity, elasticity);
		ImGui::DragFloat("Friction", FrictionCoefficient, *FrictionCoefficient);
		ImGui::DragFloat("SphereMass", &Sphere::massSphere, Sphere::massSphere);
		ImGui::DragFloat3("SpherePosition", SPosition, SPosition[1], SPosition[2], SPosition[3], 0);
		ImGui::DragFloat("SphereRadius", &Sphere::rad, 0);*/


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
//preguntar por index de la funcion computeForces y por force
