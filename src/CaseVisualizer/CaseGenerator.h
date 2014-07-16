#ifndef CASEGENERATOR_H
#define CASEGENERATOR_H

#include "IdLookupReader.h"
#include "EngineDefs.h"
#include "EngineData.h"

namespace IStrategizer
{
	class CaseBaseEx;
	class CaseEx;

	class CaseGenerator
	{
	public:
		CaseGenerator(CrossMap<unsigned, std::string>& idLookup) :
			m_idLookup(idLookup),
			m_pCb(nullptr)
		{}

		void SetCaseBase(_In_ CaseBaseEx* pCb) { m_pCb = pCb; }
		void GenBuildOrderCases();
		void GenTrainOrderCases();
		CaseEx* NewCase(_In_ GoalType caseGoal);
		void GetTrainForceCases(const char* pBwapiUnitName, unsigned amount);

	private:
		const unsigned MaxSCVs = 15;

		void GenCollectPrimaryResourceCases();
		void GenCollectSecondaryResourceCases();
		void GenSCVTrainForceCases();
		void GenBuildRefineryCases();
		void GenRecursiveGoalActionCases(GoalType goalType, const PlanStepParameters& goalParams, ActionType actionType, const PlanStepParameters& actionParams);
		void DeleteAllGeneratedCases();

		CaseBaseEx* m_pCb;
		CrossMap<unsigned, std::string>& m_idLookup;
	};
}

#endif // CASEGENERATOR_H