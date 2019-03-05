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
	glm::vec3 Sphereposition(1.f, 1.f, 1.f);
	float massSphere = 5.f;
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
	std::vector<float> velocity;
	std::vector<float> mass;
	
};

struct ForceActuator {
	virtual glm::vec3 computeForce(float mass, const glm::vec3& position) = 0;
};

struct Collider {
	virtual bool checkCollision(const glm::vec3& prev_pos,const glm::vec3& next_pos) = 0;
	virtual void getPlane(glm::vec3& normal, float& d) = 0;
	
	void computeCollision(const glm::vec3& old_pos,const glm::vec3& old_vel, glm::vec3& new_pos,glm::vec3& new_vel) {
		//...
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
std::vector<Collider> colliders;
std::vector<ForceActuator> force_acts;

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
		int numParticlesToDraw =100;
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

	for (int i = 0; i < 5000; i++) 
	{			
		//*****************INIT PARTICLES*****************//
										//******X******//  //******Y******//  //******Z******//
		particles.positions.push_back({ randomFloat(-5,5),randomFloat(10, 5),randomFloat(-5,5) });
		particles.mass.push_back(randomFloat(1,3));
		particles.velocity.push_back( randomFloat(1,3));
		
		//*****************INIT FORCES*****************//
		
	}
	
	data = &particles.positions[0].x;
	Particles::updateParticles(0,4999,data);
	// ...................................
}

glm::vec3 computeForces(float mass, const glm::vec3& position, const std::vector<ForceActuator*>& force_acts) 
{
	glm::vec3 force = { glm::vec3{ 0,9.81,0 } };
	

	return force;
}
void euler(float dt, ParticleSystem &auxParticle)
{

	for (int i = 0; i < 5000; i++)
	{
		//computeForces(auxParticle.mass[i], auxParticle.positions[i]);
		auxParticle.positions[i] = auxParticle.positions[i]-dt*auxParticle.velocity[i];
		//auxParticle.velocity[i]=auxParticle.velocity[i]
	}
	data = &particles.positions[0].x;
}

void PhysicsUpdate(float dt) {
	// Do your update code here...
	euler(dt, particles);
	Particles::updateParticles(0, 6100, data);
	
	// ...........................
}

void PhysicsCleanup() {
	// Do your cleanup code here...


	// ............................
}