#ifndef VECTOR2_H
#define VECTOR2_H

#include <cmath>

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
            return X != right.X || Y == right.Y;
        }

        Vector2T& operator +=(const Vector2T& right)
        {
            X += right.X;
            Y += right.Y;
            return *this;
        }

        Vector2T& operator /=(T n)
        {
            X /= n;
            Y /= n;
            return *this;
        }

        Vector2T operator - (const Vector2T& right)
        {
            Vector2T res = *this;
            res.X -= right.X;
            res.Y -= right.Y;
            return res;
        }

        T Length() const { return (T)sqrt(T(SQR(other.X) + SQR(other.Y))); }

        T Distance(const Vector2T& other) const { return (T)sqrt(T(SQR(other.X - X) + SQR(other.Y - Y))); }

        void Normalize()
        {
            T len = Length();
            X /= len;
            Y /= len;
        }

        bool IsZero() { return *this == Zero(); }

        static const Vector2T& Zero() { static Vector2 zero; return zero; }
        static const Vector2T& One() { static Vector2 one(T(1), T(1)); return one; }
    };

    template<class T>
    class Circle2T
    {
    public:
        Vector2T<T> Center;
        T Radius;
    };

    typedef Vector2T<int> Vector2;
    typedef Vector2T<float> Vector2F;
    typedef Circle2T<int> Circle2;
    typedef Circle2T<float> Circle2F;
}

#endif // VECTOR2_H