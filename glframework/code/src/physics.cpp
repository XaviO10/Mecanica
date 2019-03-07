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
	glm::vec3 Sphereposition(0.f, 0.f, 0.f);
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

float constant = 0;
float elasticity = 0;
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
	glm::vec3 gravity = { 0,-9.81,0 };
	glm::vec3 computeForce(float mass, const glm::vec3& position) 
	{
		return gravity*mass;
	}
};

struct Collider {

	float d;
	glm::vec3 normal;	

	virtual bool checkCollision(const glm::vec3& prev_pos,const glm::vec3& next_pos) = 0;
	virtual void getPlane(glm::vec3& normal, float& d) = 0;
	
	void computeCollision(const glm::vec3& old_pos,const glm::vec3& old_vel, glm::vec3& new_pos,glm::vec3& new_vel) 
	{		
		getPlane(normal, d);
		new_pos = new_pos - (1+elasticity) * ((glm::dot(normal, new_pos) + d)*normal);
		new_vel = new_pos - (1+elasticity) * (glm::dot(normal, new_vel)*normal);
	}
};

struct PlaneCollider: Collider {
	
	float p_d;
	glm::vec3 p_normal;

	PlaneCollider(glm::vec3 _nrmal, float _d) 
	{
		p_normal = _nrmal;
		p_d = _d;
	}

	bool checkCollision(const glm::vec3& prev_pos, const glm::vec3& next_pos)
	{
		float distancePrev = ((glm::dot(prev_pos, p_normal)) + p_d)/glm::length(p_normal);
		float distanceNext = ((glm::dot(next_pos, p_normal)) + p_d)/glm::length(p_normal);

		if (distancePrev > 0 && distanceNext <= 0) return true;
		else return false;
	}

	void getPlane(glm::vec3& normal, float& d)
	{
		normal = p_normal;
		d = p_d;		
	}	
};

/*struct SphereCollider : Collider {
	
	glm::vec3 sphere_normal = { 0,0,1 };
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
		glm::vec3 vectorpFinal = { 0,0,0 };
		glm::vec3 equacionRecta = { 0,0,0 };
		float a, b, c, d, x1, x2;
		float x;
		float y;
		float z;
		
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
			//implementar las normales
			vectorpFinal = { prev_pos.x - next_pos.x, prev_pos.y - next_pos.y, prev_pos.z - next_pos.z };
			equacionRecta = { prev_pos.x + vectorpFinal.x,prev_pos.y*t + vectorpFinal.y*t,prev_pos.z + vectorpFinal.z*t };//la nueva t me dará el punto de colision con la esfera
			t = sqrt(b*b - (4 * a*b));
		}
		if (t > 0)
		{
			x1 = ((b*-1) + (d)) / (2 * a);
			x2 = ((b*-1) - (d)) / (2 * a);
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
};*/
			
struct CapsuleCol : Collider {
	bool checkCollision(const glm::vec3& prev_pos, const glm::vec3& next_pos) {

	}
	void getPlane(glm::vec3& normal, float& d) {

	}
};
// Boolean variables allow to show/hide the primitives
bool renderSphere = false;
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
		int numParticlesToDraw =50;
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

	Collider *planeColliderDown  = new PlaneCollider(glm::vec3{0,1,0}, 0);
	Collider *planeColliderUp    = new PlaneCollider(glm::vec3{0,-1,0}, 10);

	Collider *planeColliderLeft  = new PlaneCollider(glm::vec3{1,0,0}, -5);
	Collider *planeColliderRight = new PlaneCollider(glm::vec3{-1,0,0}, 5);

	Collider *planeColliderFront = new PlaneCollider(glm::vec3{0,0,-1}, 5);
	Collider *planeColliderBack  = new PlaneCollider(glm::vec3{0,0,1}, -5);


	//Collider *sphereCollider = new SphereCollider;

	colliders.push_back(planeColliderDown);
	//colliders.push_back(planeColliderUp);
	colliders.push_back(planeColliderLeft);
	colliders.push_back(planeColliderRight);
	colliders.push_back(planeColliderFront);
	colliders.push_back(planeColliderBack);

	//colliders.push_back(sphereCollider);

	for (int i = 0; i < 5000; i++) 
	{			
		//*****************INIT PARTICLES*****************//
										//******X******//  //******Y******//  //******Z******//
		particles.positions.push_back({ randomFloat(-5,5),randomFloat(5, 10),randomFloat(-5,5) });
		particles.mass.push_back(randomFloat(1,3));
		particles.velocity.push_back({ randomFloat(-1,1),randomFloat(-1,1),randomFloat(-1,1) });		
	}
	
	data = &particles.positions[0].x;
	Particles::updateParticles(0,5000,data);
	// ...................................
}

glm::vec3 computeForces(float mass, const glm::vec3& position, const std::vector<ForceActuator*>& force_acts)
{
	glm::vec3 forces;
	
	for (int i = 0; i < force_acts.size(); i++)
	{
		forces += force_acts[i]->computeForce(mass, position);
	}
	return forces;
}

void euler(float dt, ParticleSystem &auxParticle, const std::vector<Collider*>& colliders, const std::vector<ForceActuator*>& force_acts)
{
	for (int i = 0; i < 5000; i++)
	{
		glm::vec3 oldpos, oldvel;		
		oldpos = auxParticle.positions[i];
		oldvel = auxParticle.velocity[i];

		auxParticle.positions[i] = auxParticle.positions[i]+dt*auxParticle.velocity[i];
		auxParticle.velocity[i] = auxParticle.velocity[i] + (dt* (computeForces(auxParticle.mass[i], auxParticle.positions[i], force_acts))/auxParticle.mass[i]);

		for (int j = 0; j < colliders.size(); j++) 
		{
			if ((colliders[j]->checkCollision(oldpos, auxParticle.positions[i])==true))
			{
				colliders[j]->computeCollision(oldpos, oldvel, auxParticle.positions[i], auxParticle.velocity[i]);
			}
		}
		
		
		//auxParticle.velocity[i]=auxParticle.velocity[i]
	}
	
	data = &particles.positions[0].x;
}

void PhysicsUpdate(float dt) {
	// Do your update code here...
	euler(dt, particles,colliders, force_acts);
	Particles::updateParticles(0, 5000, data);
	
	// ...........................
}

void PhysicsCleanup() {
	// Do your cleanup code here...


	// ............................
}