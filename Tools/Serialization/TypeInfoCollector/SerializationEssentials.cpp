#ifndef SERIALIZATIONESSENTIALS_H
#include "SerializationEssentials.h"
#endif

#ifndef OBJECTFACTORY_H
#include "ObjectFactory.h"
#endif
#ifndef OBJECTFORMATTER_H
	#include "ObjectFormatter.h"
#endif
#ifndef OBJECTSERIALIZER_H
	#include "ObjectSerializer.h"
#endif

#ifndef PLANGRAPH_H
	#include "PlanGraph.h"
#endif

void SerializationEssentials::InitializeSerialization()
{
    g_ObjectFactory.AddPrototype(new PlanGraph);
    g_ObjectFactory.AddPrototype(new Diagraph<NodeValue, EdgeAnnotation>, "Diagraph(PlanStepEx*,vector(Expression*))");
    g_ObjectFactory.AddPrototype(new GraphNode<NodeValue, EdgeAnnotation>, "GraphNode(PlanStepEx*,vector(Expression*))");
    g_ObjectFactory.AddPrototype(new GraphEdge<EdgeAnnotation>, "GraphEdge(vector(Expression*))");

    g_ObjectFormatter.FinalizeTypeTable(g_ObjectSerializer.TypeTable(), g_ObjectFactory.GetObjectTable());
}
