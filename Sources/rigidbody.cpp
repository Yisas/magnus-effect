#include "rigidbody.hpp"

using namespace std;
using namespace glm;

const vec3 Rigidbody::GRAVITY = vec3(0.0f, -9.81f, 0.0f);

Rigidbody::Rigidbody(Model* model, float mass)
    : Transform(model), mass(mass)
{

}

void Rigidbody::setLinearVelocity(vec3 newLinearVelocity)
{
    linearVelocity = newLinearVelocity;
}

void Rigidbody::addGravity()
{
    constantForces.push_back(GRAVITY);
}

void Rigidbody::update(float deltaTime)
{
    setPosition(getPosition() + linearVelocity * deltaTime);
}