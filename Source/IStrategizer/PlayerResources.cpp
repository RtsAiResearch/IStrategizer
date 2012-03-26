#include "PlayerResources.h"

using namespace IStrategizer;

bool PlayerResources::HasEnough(const WorldResources* p_resources)
{
	return this->Primary() >= p_resources->Primary() &&
		this->Secondary() >= p_resources->Secondary() &&
		this->Supply() >= p_resources->Supply();
}