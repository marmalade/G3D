#ifndef G3D_MATHS_VECTOR2_H
#define G3D_MATHS_VECTOR2_H

class CG3DVector2 {
public:
	float x;
	float y;
public:
	static CG3DVector2 s_Zero;
	static CG3DVector2 s_One;
	static CG3DVector2 s_UnitX;
	static CG3DVector2 s_UnitY;
public:
	CG3DVector2() : x(0), y(0) {}
	CG3DVector2(float _x, float _y) : x(_x), y(_y) {}
	CG3DVector2(float value) : x(value), y(value) {}
	CG3DVector2(const CG3DVector2& vec) : x(vec.x), y(vec.y) {}
	
	float LengthSquared() { return x*x + y*y; }
	float Length() { return (float)sqrt(LengthSquared()); }
	float OneOverLength() { return 1.0f / (float)sqrt(x*x + y*y); }
	float Distance(CG3DVector2 vec) { return (*this - vec).Length(); }
	float DistanceSquared(CG3DVector2 vec) { return (*this - vec).LengthSquared(); }
	float Dot(CG3DVector2 vec) { return x * vec.x + y * vec.y; }
	CG3DVector2 Normalise() { float len = Length(); return CG3DVector2(x * len, y * len); }
	float NormaliseMe() { float len = Length(); *this *= len; return len; }
	CG3DVector2 Clamp(CG3DVector2 min, CG3DVector2 max) { return CG3DVector2(CLAMP(x, min.x, max.x), CLAMP(y, min.y, max.y)); }
	void ClampMe(CG3DVector2 min, CG3DVector2 max) { x = CLAMP(x, min.x, max.x); y = CLAMP(y, min.y, max.y); }
	
	CG3DVector2& operator=(CG3DVector2 vec) { x=vec.x; y=vec.y; return *this; }
	CG3DVector2 operator-() { return CG3DVector2(-x, -y); }
	bool operator==(CG3DVector2 vec) { return vec.x == x && vec.y == y; }
	bool operator!=(CG3DVector2 vec) { return vec.x != x || vec.y != y; }

	CG3DVector2 operator+(CG3DVector2 vec) { return CG3DVector2(x + vec.x, y + vec.y); }
	CG3DVector2 operator-(CG3DVector2 vec) { return CG3DVector2(x - vec.x, y - vec.y); }
	CG3DVector2 operator*(CG3DVector2 vec) { return CG3DVector2(x * vec.x, y * vec.y); }
	CG3DVector2 operator/(CG3DVector2 vec) { return CG3DVector2(x / vec.x, y / vec.y); }
	CG3DVector2 operator*(float value) { return CG3DVector2(x * value, y * value); }
	CG3DVector2 operator/(float value) { return CG3DVector2(x / value, y / value); }
	
	CG3DVector2& operator+=(CG3DVector2 vec) { x += vec.x; y += vec.y; return *this; }
	CG3DVector2& operator-=(CG3DVector2 vec) { x -= vec.x; y -= vec.y; return *this; }
	CG3DVector2& operator*=(CG3DVector2 vec) { x *= vec.x; y *= vec.y; return *this; }
	CG3DVector2& operator/=(CG3DVector2 vec) { x /= vec.x; y /= vec.y;return *this; }
	CG3DVector2& operator*=(float value) { x *= value; y *= value; return *this; }
	CG3DVector2& operator/=(float value) { x /= value; y /= value;return *this; }
};

#endif