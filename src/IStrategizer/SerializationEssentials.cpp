#include "SerializationEssentials.h"

#include "ObjectFactory.h"
#include "ObjectFormatter.h"
#include "ObjectSerializer.h"
#include "Not.h"
#include "IStrategizerEx.h"

using namespace IStrategizer;

DECL_SERIALIZABLE(Not);

void SerializationEssentials::Init()
{
    static bool initialized = false;

    if (initialized)
        return;
    
    g_SerializationSystemWorkingDir = ENGINE_IO_READ_DIR;

    g_ObjectFormatter.FinalizeTypeTable(g_ObjectSerializer.TypeTable(), g_ObjectFactory.GetObjectTable());
    
    initialized = true;
}
