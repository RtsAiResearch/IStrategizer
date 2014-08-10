#ifndef WORKERS_MANAGER_H   
#define WORKERS_MANAGER_H

#include "EngineObject.h"
#include "DataMessage.h"
#include "ArmyController.h"
#include "StrategyManager.h"
#include <map>

namespace IStrategizer
{
	class RtsGame;

	class WorkersManager : public EngineObject
	{
	public:
		struct SourceRecord
		{
			TID Id;
			ResourceType Type;
			unsigned DistanceToBase;
			std::set<TID> WorkersAssigned;
		};

        WorkersManager() :
            m_workersArmy("WorkersArmy"),
			m_firstUpdate(true),
			m_primaryOptimalAssignment(0),
			m_secondaryOptimalAssignment(0)
		{}

		void Init();
		void Update();
		void NotifyMessegeSent(Message* p_pMessage);
        TID RequestBuilder(bool requstRelease);
        TID RequestScout();
        size_t WorkersCount() const { return m_workersArmy.Entities().size(); }

	private:
		void GetResourceSources(_In_ ResourceType resource, _Out_ EntityList& sources);
		void UpdateWorkersState();
		void MaintainSecondaryResources();
		void MaintainPrimaryResources();
		SourceRecord* GetFirstAvailPrimarySource();
		void UpdateDelayedSources();
		void AddSource(_In_ TID srcId, _In_ ResourceType type, _In_ unsigned dist);
		void RemoveSource(_In_ TID srcId);
		void AssignWorker(_In_ TID workerId, _In_ TID sourceId);
		void UnassignWorker(_In_ TID workerId);
		void UnassignAstrayWorkers();

		static const unsigned MinPrimaryGatherers = 2;

		std::map<TID, SourceRecord> m_sources;
		std::multimap<unsigned, TID> m_primarySrcDist;
		std::multimap<unsigned, TID> m_secondarySrcDist;

        ArmyController m_workersArmy;
        std::map<TID, TID> m_workerToSourceMap;
		std::map<ObjectStateType, std::set<GameEntity*>> m_lastFrameWorkers;

		unsigned m_primaryOptimalAssignment;
		unsigned m_secondaryOptimalAssignment;
		bool m_firstUpdate;
		std::vector<EntityCreateMessage> m_delayedUpdateSources;
	};
}

#endif // WORKERS_MANAGER_H