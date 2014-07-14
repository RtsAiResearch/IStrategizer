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
		void GenResourceMgmtCases();
		CaseEx* NewCase(_In_ GoalType caseGoal);

	private:
		void GenCollectPrimaryResourceCases();
		void GenCollectSecondaryResourceCases();
		void GenSCVTrainForceCases();
		void GenBuildRefineryCases();

		CaseBaseEx* m_pCb;
		CrossMap<unsigned, std::string>& m_idLookup;
	};
}

#endif // CASEGENERATOR_H