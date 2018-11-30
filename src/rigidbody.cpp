#include "rigidbody.h"

#include <glm/gtc/quaternion.hpp>
#include <iostream>

using namespace std;

bool RigidBody::useGravity = true;
bool RigidBody::useMagnusForce = false;

const float RigidBody::GROUND_COORDINATE = 0;
const glm::vec3 RigidBody::GROUND_NORMAL = glm::vec3(0, 1, 0);
const glm::vec3 RigidBody::GRAVITY = glm::vec3(0.0f, -9.81f, 0.0f);

RigidBody::RigidBody(shared_ptr<Model> model, float mass, float bounciness, float drag)
    : Transform(model), mass(mass), bounciness(bounciness), drag(drag)
{

}

void RigidBody::initialize()
{
    centerOfMass = glm::vec3(0, 0, 0);
    bodySpaceInertiaTensor = glm::mat3(2.0f / 5.0f * mass * pow(scale.x / 2, 2));
    bodySpaceInertiaTensorInverse = glm::inverse(bodySpaceInertiaTensor);
    
    position = initialPosition;
    rotation = initialRotation;
    linearVelocity = initialLinearVelocity;
    angularVelocity = initialAngularVelocity;
    angularMomentum = bodySpaceInertiaTensor * angularVelocity;

	peakHeight = position.y;
	peakLinearVelocity = linearVelocity;
	horizontalDisplacementAtBounce = 0;
	hasBounced = false;
}

void RigidBody::initialize(fileReader::DataEntry dataEntry)
{
	// If thrown from left to right ...
	if(dataEntry.initialAngle > 0 && dataEntry.initialAngle < 90)
		initialPosition = glm::vec3(1, dataEntry.initialHeight, 0);
	else
		// ... if thrown from right to left
		initialPosition = glm::vec3(-1, dataEntry.initialHeight, 0);

	initialRotation = glm::mat3(1);

	switch (dataEntry.spinType)
	{
	case fileReader::SpinType::BackSpin:
		initialAngularVelocity = glm::vec3(0, 0, dataEntry.rotVelocity);
		break;
	case fileReader::SpinType::Topspin:
		initialAngularVelocity = glm::vec3(0, 0, -dataEntry.rotVelocity);
		break;
	default:
		initialAngularVelocity = glm::vec3(0, 0, 0);
		break;
	}

	initialLinearVelocity = glm::vec3(
		cos((180.0f - dataEntry.initialAngle) * glm::pi<float>() / 180.0f),
		sin((180.0f - dataEntry.initialAngle) * glm::pi<float>() / 180.0f),
		0
	);
	initialLinearVelocity = glm::normalize(initialLinearVelocity);
	initialLinearVelocity *= dataEntry.initialVelocity;
	
	initialize();
}

void RigidBody::update(float deltaTime)
{
    // forces
    if (useGravity)
        addForce(mass * GRAVITY, centerOfMass);
    if (useMagnusForce)
        addForce(drag * glm::cross(angularVelocity, linearVelocity), centerOfMass);
    glm::vec3 force = calculateResultingForce();
    
    // linear displacement
    linearVelocity += force / mass * deltaTime;
    position += linearVelocity * deltaTime;
    
    // angular displacement
    glm::vec3 torque = calculateTorque();
    angularMomentum += torque * deltaTime;
    glm::mat3 inertiaTensorInverse = glm::mat3(rotation)
        * bodySpaceInertiaTensorInverse
        * transpose(glm::mat3(rotation));
    angularVelocity = inertiaTensorInverse * angularMomentum;
    rotation *= glm::quat(angularVelocity * deltaTime);

	// Check for peak values
	if (position.y > peakHeight)
		peakHeight = position.y;
	if (abs(linearVelocity.x) > abs(peakLinearVelocity.x))
		peakLinearVelocity.x = linearVelocity.x;
	if (abs(linearVelocity.y) > abs(peakLinearVelocity.y))
		peakLinearVelocity.y = linearVelocity.y;
	if (abs(linearVelocity.z) > abs(peakLinearVelocity.z))
		peakLinearVelocity.z = linearVelocity.z;

    // clean up
    forces.clear();
    checkCollision();
}

void RigidBody::addForce(glm::vec3 force, glm::vec3 position)
{
    forces.push_back(pair<glm::vec3, glm::vec3>(force, position));
}

glm::vec3 RigidBody::calculateResultingForce()
{
    glm::vec3 resultingForce = glm::vec3(0);
    for (pair<glm::vec3, glm::vec3> force : forces)
    {
        resultingForce += force.first;
    }
    return resultingForce;
}

glm::vec3 RigidBody::calculateTorque()
{
    glm::vec3 torque = glm::vec3(0);
    for (pair<glm::vec3, glm::vec3> force : forces)
    {
        torque += cross(force.second - centerOfMass, force.first);
    }
    return torque;
}

void RigidBody::checkCollision()
{
    if (position.y - (scale.y / 2) <= GROUND_COORDINATE)
    {
        float penetration = GROUND_COORDINATE + (scale.y / 2) - position.y;
        position -= penetration / -linearVelocity.y * linearVelocity;
        linearVelocity = glm::reflect(linearVelocity, GROUND_NORMAL) * bounciness;
        angularMomentum *= bounciness;

		// Record deltaX at bounce
		if (!hasBounced) 
		{
			horizontalDisplacementAtBounce = position.x - initialPosition.x;
			hasBounced = true;
		}
    }
}