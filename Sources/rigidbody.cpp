#include "rigidbody.hpp"

using namespace std;
using namespace glm;

Rigidbody::Rigidbody(Model* model, float mass)
    : Transform(model), mass(mass)
{

}