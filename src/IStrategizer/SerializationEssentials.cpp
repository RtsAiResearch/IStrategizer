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
#ifndef DESTROYENTITYTYPEGOAL_H
#include "DestroyEntityTypeGoal.h"
#endif
#ifndef COLLECTRESOURCEGOAL_H
#include "CollectResourceGoal.h"
#endif
#ifndef WINGAMEGOAL_H
#include "WinGameGoal.h"
#endif
#ifndef TRAINFORCEGOAL_H
#include "TrainForceGoal.h"
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
#include "TrainArmyGoal.h"
#include "PlayerAttributeExist.h"

using namespace IStrategizer;

DECL_SERIALIZABLE(CaseEx);

DECL_SERIALIZABLE(CollectResourceGoal);
DECL_SERIALIZABLE(TrainForceGoal);
DECL_SERIALIZABLE(DeployArmyGoal);
DECL_SERIALIZABLE(DestroyEntityTypeGoal);
DECL_SERIALIZABLE(WinGameGoal);
DECL_SERIALIZABLE(BuildInfrastructureGoal);
DECL_SERIALIZABLE(TrainArmyGoal);

DECL_SERIALIZABLE(AttackEntityAction);
DECL_SERIALIZABLE(AttackGroundAction);
DECL_SERIALIZABLE(ResearchAction);
DECL_SERIALIZABLE(TrainAction);
DECL_SERIALIZABLE(BuildActionEx);
DECL_SERIALIZABLE(MoveAction);
DECL_SERIALIZABLE(GatherResourceAction);

DECL_SERIALIZABLE(ResourceExist);
DECL_SERIALIZABLE(ResearchDone);
DECL_SERIALIZABLE(EntityClassExist);
DECL_SERIALIZABLE(EntityClassNearArea);
DECL_SERIALIZABLE(PlayerAttributeExist);
DECL_SERIALIZABLE(And);
DECL_SERIALIZABLE(Not);
DECL_SERIALIZABLE(Or);

DECL_SERIALIZABLE(CaseBaseEx);
DECL_SERIALIZABLE_NAMED(OlcbpPlan, "AdjListDigraph(PlanStepEx*)");

DECL_SERIALIZABLE(ExecutionHistory);
DECL_SERIALIZABLE(ExecutionTrial);

void SerializationEssentials::Init()
{
    static bool initialized = false;

    if (initialized)
        return;
    
    g_ObjectFormatter.FinalizeTypeTable(g_ObjectSerializer.TypeTable(), g_ObjectFactory.GetObjectTable());
    
    initialized = true;
}
