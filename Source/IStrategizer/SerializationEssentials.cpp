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

#ifndef CASEBASEEX_H
#include "CaseBaseEx.h"
#endif
#ifndef CASEEX_H
#include "CaseEx.h"
#endif
#ifndef GAMESTATEEX_H
#include "GameStateEx.h"
#endif
#ifndef ATTACKENEMYGOAL_H
#include "AttackEnemyGoal.h"
#endif
#ifndef BUILDBASEGOAL_H
#include "BuildBaseGoal.h"
#endif
#ifndef WINWARGUSGOALEX_H
#include "WinWargusGoalEx.h"
#endif
#ifndef TRAINFORCEGOAL_H
#include "TrainForceGoal.h"
#endif
#ifndef ATTACKENTITYACTION_H
#include "AttackEntityAction.h"
#endif
#ifndef ATTACKGROUNDACTION_H
#include "AttackGroundAction.h"
#endif
#ifndef BUILDACTIONEX_H
#include "BuildActionEx.h"
#endif
#ifndef RESEARCHACTION_H
#include "ResearchAction.h"
#endif
#ifndef TRAINACTION_H
#include "TrainAction.h"
#endif
#ifndef RESOURCEEXIST_H
#include "ResourceExist.h"
#endif
#ifndef RESEARCHDONE_H
#include "ResearchDone.h"
#endif
#ifndef ENTITYOBJECTEXIST_H
#include "EntityObjectExist.h"
#endif
#ifndef CHECKENTITYOBJECTSIMILARITY_H
#include "CheckEntityObjectSimilarity.h"
#endif
#ifndef CHECKCOLONYFILTERCOUNT_H
#include "CheckColonyFilterCount.h"
#endif
#ifndef NOT_H
#include "Not.h"
#endif
#ifndef OR_H
#include "Or.h"
#endif
#ifndef CELLFEATURE_H
#include "CellFeature.h"
#endif

#ifndef PLANGRAPH_H
	#include "PlanGraph.h"
#endif

using namespace IStrategizer;

void SerializationEssentials::Init()
{
	static bool initialized = false;

	if (initialized)
		return;

    g_ObjectFactory.AddPrototype(new CaseEx);
    g_ObjectFactory.AddPrototype(new GameStateEx);

    g_ObjectFactory.AddPrototype(new Colony);

    g_ObjectFactory.AddPrototype(new AttackEnemyGoal);
    g_ObjectFactory.AddPrototype(new BuildBaseGoal);
    g_ObjectFactory.AddPrototype(new WinWargusGoalEx);
    g_ObjectFactory.AddPrototype(new TrainForceGoal);

    g_ObjectFactory.AddPrototype(new AttackEntityAction);
    g_ObjectFactory.AddPrototype(new AttackGroundAction);
    g_ObjectFactory.AddPrototype(new ResearchAction);
    g_ObjectFactory.AddPrototype(new TrainAction);
	g_ObjectFactory.AddPrototype(new BuildActionEx);

    g_ObjectFactory.AddPrototype(new ResourceExist);
    g_ObjectFactory.AddPrototype(new ResearchDone);
    g_ObjectFactory.AddPrototype(new EntityObjectExist);
    g_ObjectFactory.AddPrototype(new EntityClassExist);
    g_ObjectFactory.AddPrototype(new CheckPositionFilterCount);
    g_ObjectFactory.AddPrototype(new CheckEntityObjectSimilarity);
    g_ObjectFactory.AddPrototype(new CheckEntityObjectAttribute);
    g_ObjectFactory.AddPrototype(new CheckEntityClassAttribute);
    g_ObjectFactory.AddPrototype(new CheckColonyFilterCount);
    g_ObjectFactory.AddPrototype(new And);
    g_ObjectFactory.AddPrototype(new Not);
    g_ObjectFactory.AddPrototype(new Or);

    g_ObjectFactory.AddPrototype(new PlanGraph);
    g_ObjectFactory.AddPrototype(new Diagraph<NodeValue, EdgeAnnotation>, "Diagraph(PlanStepEx*,vector(Expression*))");
    g_ObjectFactory.AddPrototype(new GraphNode<NodeValue, EdgeAnnotation>, "GraphNode(PlanStepEx*,vector(Expression*))");
    g_ObjectFactory.AddPrototype(new GraphEdge<EdgeAnnotation>, "GraphEdge(vector(Expression*))");
    
    g_ObjectFactory.AddPrototype(new CaseBaseEx);

	g_ObjectFactory.AddPrototype(new CellFeature);
	g_ObjectFactory.AddPrototype(new ForceDescription);
	g_ObjectFactory.AddPrototype(new BuildingDescription);

    g_ObjectFormatter.FinalizeTypeTable(g_ObjectSerializer.TypeTable(), g_ObjectFactory.GetObjectTable());
	
	initialized = true;
}
