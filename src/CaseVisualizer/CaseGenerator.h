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

	private:
		const unsigned MaxSCVs = 15;

		void GenCollectPrimaryResourceCases();
		void GenCollectSecondaryResourceCases();
		void GenRecursiveGoalActionCases(GoalType goalType, const PlanStepParameters& goalParams, ActionType actionType, const PlanStepParameters& actionParams);
		void GenTrainForceCases(const char* pBwapiUnitName, unsigned amount);
		void GenBuildInfraCases(const char* pBwapiUnitName, unsigned amount);

		CaseBaseEx* m_pCb;
		CrossMap<unsigned, std::string>& m_idLookup;
	};
}

#endif // CASEGENERATOR_H