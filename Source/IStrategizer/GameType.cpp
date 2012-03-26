#ifndef GAMETYPE_H
#include "GameType.h"
#endif

#ifndef TOOLBOX_H
#include "Toolbox.h"
#endif

using namespace IStrategizer;

GameType::~GameType()
{
    Toolbox::MemoryClean(m_requiredResources);
}