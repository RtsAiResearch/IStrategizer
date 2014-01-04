#ifndef GAMEDEPENDENCY_H
#define GAMEDEPENDENCY_H

namespace IStrategizer
{
    enum EntityClassType;
    enum ResearchType;
}

namespace IStrategizer
{
    using namespace IStrategizer;
    class GameDependency
    {
    protected:
        EntityClassType         m_sourceBuilding;
        vector<EntityClassType> m_requiredBuilding;
        vector<ResearchType>    m_requiredResearch;
        vector<EntityClassType> m_dependentBuilding;
        vector<ResearchType>    m_dependentResearch;

    public:
        EntityClassType                 SourceBuilding();
        const vector<EntityClassType>&  RequiredBuilding();
        const vector<ResearchType>&     RequiredResearch();
        const vector<EntityClassType>&  DependentBuilding();
        const vector<ResearchType>&     DependentResearch();
    };
}

#endif // GAMEDEPENDENCY_H
