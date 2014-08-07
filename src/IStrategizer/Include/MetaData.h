#ifndef METADATA_H
#define METADATA_H

#include "RootMetaData.h"
#include "TypesMetaData.h"
#include "AttributesMetaData.h"

namespace IStrategizer
{
    typedef int TID;
    METADATAAPI void Init();
}

#define INVALID_TID -1
#define DONT_CARE -1

#endif // METADATA_H
