#ifndef VECTOR2_H
#define VECTOR2_H

class Vector2
{
public:
    int X;
    int Y;

    Vector2() : X(0), Y(0) {}
    Vector2(int p_X, int p_Y) : X(p_X), Y(p_Y) {}

	bool operator ==(const Vector2& p_right)
	{
		return this->X == p_right.X && this->Y == p_right.Y;
	}

	bool operator !=(const Vector2& p_right)
	{
		return this->X != p_right.X || this->Y == p_right.Y;
	}

    static const Vector2& Zero()	{ static Vector2 zero; return zero; }
    static const Vector2& One()		{ static Vector2 one(1, 1); return one; }
	static const Vector2& Null()	{ static Vector2 one(-1, -1); return one; }
};

#endif // VECTOR2_H