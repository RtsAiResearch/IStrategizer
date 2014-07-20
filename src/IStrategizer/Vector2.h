#ifndef VECTOR2_H
#define VECTOR2_H

#include <cmath>

#define SQR(X) ((X) * (X))

namespace IStrategizer
{
    class Vector2
    {
    public:
        int X;
        int Y;

        Vector2() : X(0), Y(0) {}
        Vector2(int x, int y) : X(x), Y(y) {}

        bool operator ==(const Vector2& right) const
        {
            return this->X == right.X && this->Y == right.Y;
        }

        bool operator !=(const Vector2& right) const
        {
            return this->X != right.X || this->Y == right.Y;
        }

        Vector2& operator +=(const Vector2& right)
        {
            X += right.X;
            Y += right.Y;
            return *this;
        }

        Vector2& operator /=(int n)
        {
            X /= n;
            Y /= n;
            return *this;
        }

        Vector2 operator - (const Vector2& right)
        {
            Vector2 res = *this;
            res.X -= right.X;
            res.Y -= right.Y;
            return res;
        }

        int Length() const { return Distance(Zero()); }

        int Distance(const Vector2& other) const { return (int)sqrt(SQR(other.X - X) + SQR(other.Y - Y)); }

        void Normalize()
        {
            int len = Length();
            X /= len;
            Y /= len;
        }

        bool IsNull()                    { return X == -1 && Y == -1; }
        static const Vector2& Zero()    { static Vector2 zero; return zero; }
        static const Vector2& One()        { static Vector2 one(1, 1); return one; }
        static const Vector2& Null()    { static Vector2 one(-1, -1); return one; }

    };
}

#endif // VECTOR2_H