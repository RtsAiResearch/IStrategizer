#ifndef GMLHELPER_H
#define GMLHELPER_H

#include "EngineData.h"
#include "ogdf\basic\GraphAttributes.h"


namespace IStrategizer
{
    class GmlHelper
    {
    public:
        GmlHelper() :
            m_graphAttr(m_graph, ogdf::GraphAttributes::nodeGraphics | ogdf::GraphAttributes::edgeGraphics) 
        {}
        void Load(_In_ const IOlcbpPlan* pPlan);
        void Layout();

    private:
        ogdf::Graph m_graph;
        ogdf::GraphAttributes m_graphAttr;
        std::map<IOlcbpPlan::NodeID, ogdf::node> m_nodeMap;
    };
}

#endif // GMLHELPER_H