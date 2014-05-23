#ifndef SIMILARITYWEIGHTMODEL_H
#define SIMILARITYWEIGHTMODEL_H

namespace IStrategizer
{
    class SimilarityWeightModel
    {
    public:
        float GameFrame;
        float MapArea;

        class
        {
        public:
            class
            {
            public:
                float NumBuildings;
                float NumWorkers;
                float NumAttackingUnits;
                float NumDoneResearches;
                float W;
            } Entities;


            class 
            {
            public:
                float Primary;
                float Secondary;
                float Supply;
                float W;
            } Resources;

            float W;
        } Player;
    };
}

#endif // SIMILARITYWEIGHTMODEL_H