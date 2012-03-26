#ifndef SIMILARITYCOMPUTER_H
#define SIMILARITYCOMPUTER_H

#include <cmath>
#include <vector>
#include "MathHelper.h"
using namespace std;

class SimilarityComputer
{
    template<class TFeature>
    static int ComputeSimilarity(const vector<TFeature>& p_featureVector1, const vector<TFeature>& p_featureVector2)
    {
        int m_distance = MathHelper::EuclideanDistance(p_featureVector1, p_featureVector2);
        
        return 
    }
};

#endif // SIMILARITYCOMPUTER_H