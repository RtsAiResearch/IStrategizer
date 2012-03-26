#ifndef STATICCOMPONENT_H
#include "StaticComponent.h"
#endif

using namespace IStrategizer;

void StaticComponent::Initialize()
{
    if(!m_initalized)
    {
        InitializeAux();
        m_initalized = true;
    }
}
//----------------------------------------------------------------------------------------------
StaticComponent::~StaticComponent()
{

}
