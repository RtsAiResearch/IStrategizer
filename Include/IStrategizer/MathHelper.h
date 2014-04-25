#ifndef MATHHELPER_H
#define MATHHELPER_H

#include <cmath>
#include <cassert>
#include <vector>

namespace IStrategizer
{
    template< class T>
    class Rectangle
    {
    public:
        Rectangle() 
            : X(T()),
            Y(T()),
            Width(T()),
            Height(T()) {}

        T X;
        T Y;
        T Width;
        T Height;
    };

    class MathHelper
    {
    public:
        //////////////////////////////////////////////////////////////////////////
        template<class TValue>
        static void Normalize(TValue& p_value, const TValue& p_oldMin, const TValue& p_oldMax, const TValue& p_newMin, const TValue& p_newMax)
        {
            p_value = ((double)(p_value - p_oldMin) / (double)(p_oldMax - p_oldMin) * (double)(p_newMax - p_newMin)) + p_newMin;
        }

        //////////////////////////////////////////////////////////////////////////
        static int Distance2D(int p_x1Coord, int p_y1Coord, int p_x2Coord, int p_y2Coord)
        {
            int dx = p_x1Coord - p_x2Coord;
            int dy = p_y1Coord - p_y2Coord;

            return (int)sqrt((float)(dx * dx + dy * dy));
        }

        //////////////////////////////////////////////////////////////////////////
        template<class TFeature>
        static int EuclideanDistance(const std::vector<TFeature>& p_featureVector1, const std::vector<TFeature>& p_featureVector2)
        {
            _ASSERTE(p_featureVector1.size() == p_featureVector2.size());

            int m_distance = 0;
            for(unsigned i = 0; i < p_featureVector1.size(); ++i)
            {
                m_distance += (p_featureVector1[i] - p_featureVector2[i]) * (p_featureVector1[i] - p_featureVector2[i]);
            }

            return (int)sqrt((float)m_distance);
        }

        //////////////////////////////////////////////////////////////////////////
        static int WindowMembership(int p_centerXCoord, int p_centerYCoord, int p_testXCoord, int p_testYCoord, int p_windowSize)
        {
            return  abs((p_centerXCoord - p_testXCoord) / (float)p_windowSize) <= 0.5 &&
                abs((p_centerYCoord - p_testYCoord) / (float)p_windowSize) <= 0.5;
        }

        //////////////////////////////////////////////////////////////////////////
        static int CircleMembership(int p_centerXCoord, int p_centerYCoord, int p_radius, int p_testXCoord, int p_testYCoord)
        {
            return Distance2D(p_centerXCoord, p_centerYCoord, p_testXCoord, p_testYCoord) <= p_radius;
        }

        //////////////////////////////////////////////////////////////////////////
        static bool RectangleMembership(int p_topLeftXCoord, int p_topLeftYCoord, int p_width, int p_height, int p_testXCoord, int p_testYCoord)
        {
            return p_testXCoord < p_topLeftXCoord + p_width &&
                p_testXCoord >= p_topLeftXCoord &&
                p_testYCoord < p_topLeftYCoord + p_height &&
                p_testYCoord >= p_topLeftYCoord;
        }

        static void MinimumBoundingBox(const std::vector<std::pair<int, int> >& p_points, int& p_topLeftXCoord, int& p_topLeftYCoord, int& p_width, int& p_height)
        {
            p_topLeftXCoord = 0;
            p_topLeftYCoord = 0;
            p_width = 0;
            p_height = 0;
            int farthestWidth;
            int farthestHeight;

            if(p_points.size() == 0)
                return;

            p_topLeftXCoord = p_points[0].first;
            p_topLeftYCoord = p_points[0].second;
            for(std::vector<std::pair<int, int> >::const_iterator itr = p_points.begin() + 1;
                itr != p_points.end();
                itr++)
            {
                farthestWidth   = std::max(p_topLeftXCoord + p_width, itr->first);
                farthestHeight  = std::max(p_topLeftYCoord + p_width, itr->second);

                p_topLeftXCoord = std::min(p_topLeftXCoord, itr->first);
                p_topLeftYCoord = std::min(p_topLeftYCoord, itr->second);

                p_width = farthestWidth - p_topLeftYCoord;
                p_height = farthestHeight - p_topLeftYCoord;
            }
        }

        // Test whether rectangle r1 is fully contained in rectangle r2
        template<class T>
        static bool RectangleMembership(const Rectangle<T>& r1, const Rectangle<T> r2)
        {
            return
                r1.X >= r2.X &&
                r1.Y >= r2.Y &&
                r1.X + r1.Width <= r2.Width &&
                r1.Y + r1.Height <= r2.Y + r2.Height;
        }
    };
}

#endif // MATHHELPER_H
