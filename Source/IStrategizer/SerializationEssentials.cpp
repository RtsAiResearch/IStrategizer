#include "SerializationEssentials.h"

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
#ifndef DESTROYENTITYTYPEGOAL_H
#include "DestroyEntityTypeGoal.h"
#endif
#ifndef COLLECTRESOURCEGOAL_H
#include "CollectResourceGoal.h"
#endif
#ifndef WINGAMEGOAL_H
#include "WinGameGoal.h"
#endif
#ifndef TRAINARMYGOAL_H
#include "TrainArmyGoal.h"
#endif
#ifndef BUILDINFRASTRUCTUREGOAL_H
#include "BuildInfrastructureGoal.h"
#endif
#ifndef DEPLOYARMYGOAL_H
#include "DeployArmyGoal.h"
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
#ifndef MOVEENTITYACTION_H
#include "MoveEntityAction.h"
#endif
#ifndef MOVEACTION_H
#include "MoveAction.h"
#endif
#ifndef GATHERRESOURCEACTION_H
#include "GatherResourceAction.h"
#endif
#ifndef RESOURCEEXIST_H
#include "ResourceExist.h"
#endif
#ifndef RESEARCHDONE_H
#include "ResearchDone.h"
#endif
#ifndef ENTITYCLASSNEARAREA_H
#include "EntityClassNearArea.h"
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
#include "ExecutionHistory.h"
#include "ExecutionTrial.h"

using namespace IStrategizer;

void SerializationEssentials::Init()
{
    static bool initialized = false;

    if (initialized)
        return;

    g_ObjectFactory.AddPrototype(new CaseEx);
    g_ObjectFactory.AddPrototype(new GameStateEx);

    g_ObjectFactory.AddPrototype(new CollectResourceGoal);
    g_ObjectFactory.AddPrototype(new TrainArmyGoal);
    g_ObjectFactory.AddPrototype(new DeployArmyGoal);
    g_ObjectFactory.AddPrototype(new DestroyEntityTypeGoal);
    g_ObjectFactory.AddPrototype(new WinGameGoal);
    g_ObjectFactory.AddPrototype(new BuildInfrastructureGoal);

    g_ObjectFactory.AddPrototype(new AttackEntityAction);
    g_ObjectFactory.AddPrototype(new AttackGroundAction);
    g_ObjectFactory.AddPrototype(new ResearchAction);
    g_ObjectFactory.AddPrototype(new TrainAction);
    g_ObjectFactory.AddPrototype(new BuildActionEx);
    g_ObjectFactory.AddPrototype(new MoveEntityAction);
    g_ObjectFactory.AddPrototype(new MoveAction);
	g_ObjectFactory.AddPrototype(new GatherResourceAction);

    g_ObjectFactory.AddPrototype(new ResourceExist);
    g_ObjectFactory.AddPrototype(new ResearchDone);
    g_ObjectFactory.AddPrototype(new EntityClassExist);
    g_ObjectFactory.AddPrototype(new EntityClassNearArea);
    g_ObjectFactory.AddPrototype(new And);
    g_ObjectFactory.AddPrototype(new Not);
    g_ObjectFactory.AddPrototype(new Or);

    g_ObjectFactory.AddPrototype(new CaseBaseEx);
    g_ObjectFactory.AddPrototype(new OlcbpPlan, "AdjListDigraph(PlanStepEx*)");

    g_ObjectFactory.AddPrototype(new ExecutionHistory);
    g_ObjectFactory.AddPrototype(new ExecutionTrial);

    g_ObjectFormatter.FinalizeTypeTable(g_ObjectSerializer.TypeTable(), g_ObjectFactory.GetObjectTable());
    
    initialized = true;
}
