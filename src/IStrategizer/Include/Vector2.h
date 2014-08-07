#ifndef VECTOR2_H
#define VECTOR2_H

#include <cmath>
#include <limits>
#include <sstream>
#include <cstdlib>
#include <time.h>

#define SQR(X) ((X) * (X))

namespace IStrategizer
{
    template<class T>
    class Vector2T
    {
    public:
        T X;
        T Y;

        Vector2T() : X(T(0)), Y(T(0)) {}
        Vector2T(T x, T y) : X(x), Y(y) {}

        bool operator ==(const Vector2T& right) const
        {
            return X == right.X && Y == right.Y;
        }

        bool operator !=(const Vector2T& right) const
        {
            return (X != right.X || Y != right.Y);
        }

        Vector2T& operator -=(const T n)
        {
            X -= n;
            Y -= n;
            return *this;
        }

        Vector2T& operator +=(const Vector2T& right)
        {
            X += right.X;
            Y += right.Y;
            return *this;
        }

        Vector2T& operator *=(const T n)
        {
            X *= n;
            Y *= n;
            return *this;
        }

        Vector2T& operator /=(const T n)
        {
            X /= n;
            Y /= n;
            return *this;
        }

        Vector2T operator + (const T n)
        {
            return Vector2T(X + n, Y + n);
        }

        Vector2T operator + (const Vector2T& right)
        {
            return Vector2T(X + right.X, Y + right.Y);
        }

        Vector2T operator - (const Vector2T& right)
        {
            return Vector2T(X - right.X, Y - right.Y);
        }

        Vector2T operator / (const T n)
        {
            return Vector2T(X / n, Y / n);
        }

        Vector2T operator * (const T n)
        {
            return Vector2T(X * n, Y * n);
        }

        T Length() const { return (T)sqrt(T(SQR(X) + SQR(Y))); }

        T Distance(const Vector2T& other) const { return (T)sqrt(T(SQR(other.X - X) + SQR(other.Y - Y))); }

        void Normalize()
        {
            T len = Length();
            X /= len;
            Y /= len;
        }

        std::string ToString()
        {
            std::stringstream ss;
            ss << '<';
            ss << X;
            ss << ',';
            ss << Y;
            ss << '>';

            return ss.str();
        }

        bool IsZero() const { return *this == Zero(); }
        bool IsInf() const { return *this == Inf(); }

        static const Vector2T& Zero() { static Vector2T zero; return zero; }
        static const Vector2T& One() { static Vector2T one(T(1), T(1)); return one; }
#undef max
        static const Vector2T& Inf() { static Vector2T inf(std::numeric_limits<T>::max(), std::numeric_limits<T>::max()); return inf; }
    };

    template<class T>
    class Circle2T
    {
    public:
        Circle2T() :
            Center(Vector2T<T>::Zero()),
            Radius(T(0))
        {}
        Circle2T(Vector2T<T> center, T radius) :
            Center(center),
            Radius(radius)
        {}

        bool IsInside(const Vector2T<T>& v)
        {
            return Center.Distance(v) < Radius;
        }

        // Generate a random point on the circle surface
        Vector2T<T> RandomInside()
        {
            Vector2T<T> v = Center;

            // Generate random radius in the range [1, R]
            int randR = (rand() % Radius) + 1;
            float randTheta = (float)rand();

            v.X += int(randR * (float)cos(randTheta));
            v.Y += int(randR * (float)sin(randTheta));

            return v;
        }

        Vector2T<T> Center;
        T Radius;
    };

    typedef Vector2T<int> Vector2;
    typedef Vector2T<float> Vector2F;
    typedef Circle2T<int> Circle2;
    typedef Circle2T<float> Circle2F;
}

#endif // VECTOR2_H