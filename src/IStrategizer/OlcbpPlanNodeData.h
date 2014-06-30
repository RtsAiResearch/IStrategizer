#ifndef OLCBPPLANNODEDATA_H
#define OLCBPPLANNODEDATA_H

#include "EngineData.h"
#include "CaseEx.h"
#include "Logger.h"

namespace IStrategizer
{
    class OlcbpPlanNodeData
    {
    public:
        IOlcbpPlan::NodeID ID;
        CaseSet TriedCases;
        CaseEx* BelongingCase;
        IOlcbpPlan::NodeID SatisfyingGoal;
        unsigned WaitOnParentsCount;
        unsigned WaitOnChildrenCount;
        bool IsOpen;

        OlcbpPlanNodeData()
            :
            ID(IOlcbpPlan::NullNodeID),
            BelongingCase(nullptr),
            SatisfyingGoal(IOlcbpPlan::NullNodeID),
            WaitOnParentsCount(0),
            WaitOnChildrenCount(0),
            IsOpen(false)
        { }

        void DecWaitOnParentsCount() { LogInfo("Dec WaitOnParentsCount=%d for node[%d]", WaitOnParentsCount, ID); --WaitOnParentsCount; }
        void IncWaitOnParentsCount() { LogInfo("Inc WaitOnParentsCount=%d for node[%d]", WaitOnParentsCount, ID); ++WaitOnParentsCount; }
        void SetWaitOnParentsCount(unsigned val) { if (val != WaitOnParentsCount) { LogInfo("Set WaitOnParentsCount= %d -> %d for node[%d]", WaitOnParentsCount, val, ID); WaitOnParentsCount = val; } }
        void DecWaitOnChildrenCount() { LogInfo("Dec WaitOnChildrenCount=%d for node[%d]", WaitOnChildrenCount, ID); --WaitOnChildrenCount; }
        void SetWaitOnChildrenCount(unsigned val) { if (val != WaitOnChildrenCount) { LogInfo("Set WaitOnChildrenCount= %d -> %d for node[%d]", WaitOnChildrenCount, val, ID); WaitOnChildrenCount = val; } }
    };

    typedef const OlcbpPlanNodeData* ConstOlcbpPlanNodeDataPtr;
    typedef const OlcbpPlanNodeData& ConstOlcbpPlanNodeDataRef;
    typedef std::map<IOlcbpPlan::NodeID, OlcbpPlanNodeData> OlcbpPlanNodeDataMap;
    typedef const std::map<IOlcbpPlan::NodeID, OlcbpPlanNodeData>& ConstOlcbpPlanNodeDataMapRef;
    typedef const std::map<IOlcbpPlan::NodeID, OlcbpPlanNodeData>* ConstOlcbpPlanNodeDataMapPtr;
}

#endif // OLCBPPLANNODEDATA_H