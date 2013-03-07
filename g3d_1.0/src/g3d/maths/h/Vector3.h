#ifndef G3D_MATHS_VECTOR3_H
#define G3D_MATHS_VECTOR3_H

class CG3DVector3 {
public:
	float x;
	float y;
	float z;
public:
	static CG3DVector3 s_Zero;
	static CG3DVector3 s_One;
	static CG3DVector3 s_UnitX;
	static CG3DVector3 s_UnitY;
	static CG3DVector3 s_UnitZ;
	static CG3DVector3 s_Up;
	static CG3DVector3 s_Down;
	static CG3DVector3 s_Left;
	static CG3DVector3 s_Right;
	static CG3DVector3 s_Forward;
	static CG3DVector3 s_Backward;
public:
	CG3DVector3() : x(0), y(0), z(0) {}
	CG3DVector3(float _x, float _y, float _z) : x(_x), y(_y), z(_z) {}
	//CG3DVector3(CG3DVector2 vec, float _z) : x(vec.x), y(vec.y), z(_z) {}
	CG3DVector3(float value) : x(value), y(value), z(value) {}
	CG3DVector3(const CG3DVector3& vec) : x(vec.x), y(vec.y), z(vec.z) {}
	
	float LengthSquared() { return x*x + y*y + z*z; }
	float Length() { return (float)sqrt(LengthSquared()); }
	float OneOverLength() { return 1.0f / (float)sqrt(x*x + y*y + z*z); }
	float Distance(CG3DVector3 vec) { return (*this - vec).Length(); }
	float DistanceSquared(CG3DVector3 vec) { return (*this - vec).LengthSquared(); }

	float Dot(CG3DVector3 vec) { return x * vec.x + y * vec.y + z * vec.z; }
	CG3DVector3 Cross(CG3DVector3 vec) { return CG3DVector3(y * vec.z - z * vec.y, z * vec.x - x * vec.z, x * vec.y - y * vec.x); }
	CG3DVector3 Reflect(CG3DVector3 vec) { float num = x * vec.x + y * vec.y + z * vec.z; return CG3DVector3(x - 2.0f * num * vec.x, y - 2.0f * num * vec.y, z - 2.0f * num * vec.z); }
	CG3DVector3 Normalise() { float len = Length(); return CG3DVector3(x * len, y * len, z * len); }
	float NormaliseMe() { float len = Length(); *this *= len; return len; }
	CG3DVector3 Clamp(CG3DVector3 min, CG3DVector3 max) { return CG3DVector3(CLAMP(x, min.x, max.x), CLAMP(y, min.y, max.y), CLAMP(z, min.z, max.z)); }
	void ClampMe(CG3DVector3 min, CG3DVector3 max) { x = CLAMP(x, min.x, max.x); y = CLAMP(y, min.y, max.y); z = CLAMP(z, min.z, max.z); }
	
	CG3DVector3& operator=(CG3DVector3 vec) { x=vec.x; y=vec.y; z=vec.z; return *this; }
	CG3DVector3 operator-() { return CG3DVector3(-x, -y, -z); }
	bool operator==(CG3DVector3 vec) { return vec.x == x && vec.y == y && vec.z == z; }
	bool operator!=(CG3DVector3 vec) { return vec.x != x || vec.y != y || vec.z != z; }

	CG3DVector3 operator+(CG3DVector3 vec) { return CG3DVector3(x + vec.x, y + vec.y, z + vec.z); }
	CG3DVector3 operator-(CG3DVector3 vec) { return CG3DVector3(x - vec.x, y - vec.y, z - vec.z); }
	CG3DVector3 operator*(CG3DVector3 vec) { return CG3DVector3(x * vec.x, y * vec.y, z * vec.z); }
	CG3DVector3 operator/(CG3DVector3 vec) { return CG3DVector3(x / vec.x, y / vec.y, z / vec.z); }
	CG3DVector3 operator*(float value) { return CG3DVector3(x * value, y * value, z * value); }
	CG3DVector3 operator/(float value) { return CG3DVector3(x / value, y / value, z / value); }
	
	CG3DVector3& operator+=(CG3DVector3 vec) { x += vec.x; y += vec.y; z += vec.z; return *this; }
	CG3DVector3& operator-=(CG3DVector3 vec) { x -= vec.x; y -= vec.y; z -= vec.z; return *this; }
	CG3DVector3& operator*=(CG3DVector3 vec) { x *= vec.x; y *= vec.y; z *= vec.z; return *this; }
	CG3DVector3& operator/=(CG3DVector3 vec) { x /= vec.x; y /= vec.y; z /= vec.z; return *this; }
	CG3DVector3& operator*=(float value) { x *= value; y *= value; z *= value; return *this; }
	CG3DVector3& operator/=(float value) { x /= value; y /= value; z /= value; return *this; }
};

#endif