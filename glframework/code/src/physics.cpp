#include <imgui\imgui.h>
#include <imgui\imgui_impl_sdl_gl3.h>
#include <glm\gtc\matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <stdlib.h>
#include <time.h>
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
	glm::vec3 Sphereposition(0.f, 0.f, 5.f);
	float massSphere = 50.f;
	float rad=1.f;
	
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

struct ParticleSystem 
{
	std::vector<glm::vec3> positions;
	std::vector<glm::vec3> velocity;
	std::vector<float> mass;
};

struct ForceActuator {
	virtual glm::vec3 computeForce(float mass, const glm::vec3& position) = 0;
};

float constant = 1;

struct SphereForce :ForceActuator 
{
	glm::vec3 computeForce(float mass, const glm::vec3& position) 
	{
		glm::vec3 Sphforce = constant * Sphere::massSphere*mass*(position - Sphere::Sphereposition);
		Sphforce = Sphforce / sqrt(((position.x - Sphere::Sphereposition.x) + (position.y - Sphere::Sphereposition.y) + (position.z - Sphere::Sphereposition.z)));
		Sphforce = Sphforce / ((position.x - Sphere::Sphereposition.x) + (position.y - Sphere::Sphereposition.y) + (position.z - Sphere::Sphereposition.z));
		return Sphforce;
	}
};
struct GravityForce :ForceActuator 
{
	glm::vec3 gravity = { 0,9.81,0 };
	glm::vec3 computeForce(float mass, const glm::vec3& position) 
	{
		return gravity;
	}
};

struct Collider {
	virtual bool checkCollision(const glm::vec3& prev_pos,const glm::vec3& next_pos) = 0;
	virtual void getPlane(glm::vec3& normal, float& d) = 0;
	
	void computeCollision(const glm::vec3& old_pos,const glm::vec3& old_vel, glm::vec3& new_pos,glm::vec3& new_vel) {
		//...
		
	}
};

struct PlaneCollider :Collider 
{
	
	float plane_d;
	glm::vec3 plane_normal;

	bool checkCollision(const glm::vec3& prev_pos, const glm::vec3& next_pos)
	{
		//Sale por X
		if ((prev_pos.x > -5) && (next_pos.x < -5))
		{
			//Plano de la izquierda con normal (1,0,0)
			plane_d = -5;
			plane_normal = { 1,0,0 };
			getPlane(plane_normal, plane_d);
			return true;
		}

		if ((prev_pos.x < 5) && (next_pos.x > 5))
		{
			plane_d = 5;
			//Plano de arriba con normal (-1,0,0)
			plane_normal = { -1,0,0 };
			getPlane(plane_normal, plane_d);
			return true;
		}	
		

		//Sale por Y
		if ((prev_pos.y > 0) && (next_pos.y < 0)) 
		{
			//Plano de la izquierda normal en direccion al eje Y
			plane_d = 0;
			plane_normal = { 0,1,0 };
			getPlane(plane_normal, plane_d);
			return true;
		}
		if ((prev_pos.y > 0) && (next_pos.y < 0))
		{
			plane_d = 10;
			plane_normal = { 0,-1,0 };
			getPlane(plane_normal, plane_d);
			return true;
		}
		
		//Sale por Z
		if ((prev_pos.z > -5) && (next_pos.z < -5))
		{
			//Plano de la izquierda con normal (1,0,0)
			plane_d = -5;
			plane_normal = { 0,0,1 };
			getPlane(plane_normal, plane_d);
			return true;
		}

		if ((prev_pos.z < 5) && (next_pos.z > 5))
		{
			plane_d = 5;
			//Plano de arriba con normal (-1,0,0)
			plane_normal = { 0,0,-1 };
			getPlane(plane_normal, plane_d);
			return true;
		}

		return false;
	}
	
	void getPlane(glm::vec3& normal, float& d)
	{
		normal=plane_normal;
		d=plane_d;
	}	
};

struct SphereCollider : Collider {
	bool checkCollision(const glm::vec3& prev_pos, const glm::vec3& next_pos) {
		float checkCollisionX_prev = 0;
		float checkCollisionY_prev = 0;
		float checkCollisionZ_prev = 0;
		float checkCollision_prev = 0;
		float checkCollisionX_next = 0;
		float checkCollisionY_next = 0;
		float checkCollisionZ_next = 0;
		float checkCollision_next = 0;
		//comprobamos si la prev_pos ha colisionado o no
		checkCollisionX_prev = prev_pos.x - Sphere::Sphereposition.x;
		checkCollisionY_prev = prev_pos.y - Sphere::Sphereposition.y;
		checkCollision_prev = sqrt(pow(checkCollisionX_prev, 2) + pow(checkCollisionY_prev, 2));
		//comprobamos si la next_pos a colisionado o no
		checkCollisionX_next = prev_pos.x - Sphere::Sphereposition.x;
		checkCollisionY_next = prev_pos.y - Sphere::Sphereposition.y;
		checkCollision_next = sqrt(pow(checkCollisionX_prev, 2) + pow(checkCollisionY_prev, 2));
		//la particula solo colisiona si el prev_pos es más grande
		if ((Sphere::rad < checkCollision_prev) && (Sphere::rad > checkCollision_next)) {
		}
		//implementar las normales
		else {
			return false;
		}
		return false;
	}
	void getPlane(glm::vec3& normal, float& d) {

	}
};

struct CapsuleCol : Collider {
	bool checkCollision(const glm::vec3& prev_pos, const glm::vec3& next_pos) {

	}
	void getPlane(glm::vec3& normal, float& d) {

	}
};
// Boolean variables allow to show/hide the primitives
bool renderSphere = true;
bool renderCapsule = false;
bool renderParticles = true;
bool renderMesh = false;
bool renderFiber = false;
bool renderCube = false;

float* data;
ParticleSystem particles;
std::vector<Collider*> colliders;
std::vector<ForceActuator*> force_acts;

//***EULER***//
float Pf, Pi,Vf,Vi,Fi;

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
		int numParticlesToDraw =5000;
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

float randomFloat(float min, float max) 
{
	return  (max - min) * ((((float)rand()) / (float)RAND_MAX)) + min;
}

void PhysicsInit() {

	// Do your initialization code here...
	srand(time(NULL));	
	Sphere::updateSphere(Sphere::Sphereposition);
	
	SphereForce *SphForce = new SphereForce;
	GravityForce *GravForce = new GravityForce;
	force_acts.push_back(SphForce);
	force_acts.push_back(GravForce);

	Collider *planeCollider = new PlaneCollider;
	Collider *sphereCollider = new SphereCollider;

	colliders.push_back(planeCollider);
	colliders.push_back(sphereCollider);

	for (int i = 0; i < 5000; i++) 
	{			
		//*****************INIT PARTICLES*****************//
										//******X******//  //******Y******//  //******Z******//
		particles.positions.push_back({ randomFloat(-5,5),randomFloat(10, 5),randomFloat(-5,5) });
		particles.mass.push_back(randomFloat(1,3));
		particles.velocity.push_back({ randomFloat(-3,3),randomFloat(-3,3),randomFloat(-3,3) });		
	}
	
	data = &particles.positions[0].x;
	Particles::updateParticles(0,4999,data);
	// ...................................
}

glm::vec3 computeForces(float mass, const glm::vec3& position, const std::vector<ForceActuator*>& force_acts)
{
	glm::vec3 forces;
	
	for (int i = 0; i < force_acts.size(); i++)
	{
		forces += force_acts[0]->computeForce(mass, position);
	}
	return (forces/mass);
}

void euler(float dt, ParticleSystem &auxParticle, const std::vector<ForceActuator*>& force_acts)
{
	for (int i = 0; i < 5000; i++)
	{
		//computeForces(auxParticle.mass[i], auxParticle.positions[i]);
		auxParticle.positions[i] = auxParticle.positions[i]-dt*auxParticle.velocity[i];
		auxParticle.velocity[i] = auxParticle.velocity[i] + (dt* computeForces(auxParticle.mass[i], auxParticle.positions[i], force_acts));
		//auxParticle.velocity[i]=auxParticle.velocity[i]
	}
	data = &particles.positions[0].x;
}

void PhysicsUpdate(float dt) {
	// Do your update code here...
	euler(dt, particles, force_acts);
	Particles::updateParticles(0, 6100, data);
	for (int i = 0; i < colliders.size(); i++)
	{
		
	}
	// ...........................
}

void PhysicsCleanup() {
	// Do your cleanup code here...


	// ............................
}