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
        static double EuclideanDistance(int value1, int value2)
        {
            return sqrt(pow(value1 - value2, 2));
            
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

        static void MinimumBoundingBox(const std::vector<Vector2>& p_points, Vector2& upperLeft, Vector2& lowerRight)
        {
            auto xExtremes = std::minmax_element(p_points.begin(), p_points.end(),
                [](const Vector2& lhs, const Vector2& rhs) {
                return lhs.X < rhs.X;
            });

            auto yExtremes = std::minmax_element(p_points.begin(), p_points.end(),
                [](const Vector2& lhs, const Vector2& rhs) {
                return lhs.Y < rhs.Y;
            });

            upperLeft = Vector2(xExtremes.first->X, yExtremes.first->Y);
            lowerRight = Vector2(xExtremes.second->X, yExtremes.second->Y);
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

        template<typename T>
        static T Clamp(T x, T low, T high)
        {
            return x < low ? low : (x > high ? high : x); 
        }

#undef get16bits
#if (defined(__GNUC__) && defined(__i386__)) || defined(__WATCOMC__) \
    || defined(_MSC_VER) || defined (__BORLANDC__) || defined (__TURBOC__)
#define get16bits(d) (*((const uint16_t *) (d)))
#endif

#if !defined (get16bits)
#define get16bits(d) ((((uint32_t)(((const uint8_t *)(d))[1])) << 8)\
    +(uint32_t)(((const uint8_t *)(d))[0]) )
#endif
        static uint32_t SuperFastHash(_In_ const char* data, _In_ int len) 
        {
            uint32_t hash = len, tmp;
            int rem;

            if (len <= 0 || data == NULL) return 0;

            rem = len & 3;
            len >>= 2;

            /* Main loop */
            for (;len > 0; len--) {
                hash  += get16bits (data);
                tmp    = (get16bits (data+2) << 11) ^ hash;
                hash   = (hash << 16) ^ tmp;
                data  += 2*sizeof (uint16_t);
                hash  += hash >> 11;
            }

            /* Handle end cases */
            switch (rem) {
            case 3: hash += get16bits (data);
                hash ^= hash << 16;
                hash ^= ((signed char)data[sizeof (uint16_t)]) << 18;
                hash += hash >> 11;
                break;
            case 2: hash += get16bits (data);
                hash ^= hash << 11;
                hash += hash >> 17;
                break;
            case 1: hash += (signed char)*data;
                hash ^= hash << 10;
                hash += hash >> 1;
            }

            /* Force "avalanching" of final 127 bits */
            hash ^= hash << 3;
            hash += hash >> 5;
            hash ^= hash << 4;
            hash += hash >> 17;
            hash ^= hash << 25;
            hash += hash >> 6;

            return hash;
        }
    };
}

#endif // MATHHELPER_H
