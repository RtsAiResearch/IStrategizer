#include "stdafx.h"
#include "GmlHelper.h"
#include <map>
#include "ogdf\layered\SugiyamaLayout.h"
#include "ogdf\layered\OptimalRanking.h"
#include "ogdf\layered\MedianHeuristic.h"
#include "ogdf\layered\OptimalHierarchyLayout.h"

using namespace IStrategizer;
using namespace std;
using namespace ogdf;

void GmlHelper::Load(_In_ const IOlcbpPlan* pPlan)
{
    m_graph.clear();
    m_graphAttr.clearAllBends();
    m_nodeMap.clear();
    
    auto nodes = pPlan->GetNodes();
    for (auto nodeId : nodes)
    {
        m_nodeMap[nodeId] = m_graph.newNode();
    }

    for (auto nodeEntry : m_nodeMap)
    {
        for (auto adjNodeId : pPlan->GetAdjacentNodes(nodeEntry.first))
        {
            m_graph.newEdge(m_nodeMap[nodeEntry.first], m_nodeMap[adjNodeId]);
        }
    }
}
//////////////////////////////////////////////////////////////////////////
void GmlHelper::Layout()
{

}
