#ifndef G3D_MATHS_VECTOR4_H
#define G3D_MATHS_VECTOR4_H

class CG3DVector4 {
public:
	float x;
	float y;
	float z;
	float w;
public:
	static CG3DVector4 s_Zero;
	static CG3DVector4 s_One;
	static CG3DVector4 s_UnitX;
	static CG3DVector4 s_UnitY;
	static CG3DVector4 s_UnitZ;
	static CG3DVector4 s_UnitW;
public:
	CG3DVector4() : x(0), y(0), z(0), w(0) {}
	CG3DVector4(float _x, float _y, float _z, float _w) : x(_x), y(_y), z(_z), w(_w) {}
	//CG3DVector4(CG3DVector2 vec, float _z, float _w) : x(vec.x), y(vec.y), z(_z), w(_w) {}
	//CG3DVector4(CG3DVector3 vec, float _w) : x(vec.x), y(vec.y), z(vec.z), w(_w) {}
	CG3DVector4(float value) : x(value), y(value), z(value), w(value) {}
	CG3DVector4(const CG3DVector4& vec) : x(vec.x), y(vec.y), z(vec.z), w(vec.w) {}
	
	float LengthSquared() { return x*x + y*y + z*z + w*w; }
	float Length() { return (float)sqrt(LengthSquared()); }
	float OneOverLength() { return 1.0f / (float)sqrt(x*x + y*y + z*z + w*w); }
	float Distance(CG3DVector4 vec) { return (*this - vec).Length(); }
	float DistanceSquared(CG3DVector4 vec) { return (*this - vec).LengthSquared(); }
	float Dot(CG3DVector4 vec) { return x * vec.x + y * vec.y + z * vec.z + w * vec.w; }
	CG3DVector4 Normalise() { float len = Length(); return CG3DVector4(x * len, y * len, z * len, w * len); }
	float NormaliseMe() { float len = Length(); *this *= len; return len; }
	CG3DVector4 Clamp(CG3DVector4 min, CG3DVector4 max) { return CG3DVector4(CLAMP(x, min.x, max.x), CLAMP(y, min.y, max.y), CLAMP(z, min.z, max.z), CLAMP(w, min.w, max.w)); }
	void ClampMe(CG3DVector4 min, CG3DVector4 max) { x = CLAMP(x, min.x, max.x); y = CLAMP(y, min.y, max.y); z = CLAMP(z, min.z, max.z); w = CLAMP(w, min.w, max.w); }
	
	CG3DVector4& operator=(CG3DVector4 vec) { x=vec.x; y=vec.y; z=vec.z; w=vec.w; return *this; }
	CG3DVector4 operator-() { return CG3DVector4(-x, -y, -z, -w); }
	bool operator==(CG3DVector4 vec) { return vec.x == x && vec.y == y && vec.z == z && vec.w == w; }
	bool operator!=(CG3DVector4 vec) { return vec.x != x || vec.y != y || vec.z != z || vec.w != w; }

	CG3DVector4 operator+(CG3DVector4 vec) { return CG3DVector4(x + vec.x, y + vec.y, z + vec.z, w + vec.w); }
	CG3DVector4 operator-(CG3DVector4 vec) { return CG3DVector4(x - vec.x, y - vec.y, z - vec.z, w - vec.w); }
	CG3DVector4 operator*(CG3DVector4 vec) { return CG3DVector4(x * vec.x, y * vec.y, z * vec.z, w * vec.w); }
	CG3DVector4 operator/(CG3DVector4 vec) { return CG3DVector4(x / vec.x, y / vec.y, z / vec.z, w / vec.w); }
	CG3DVector4 operator*(float value) { return CG3DVector4(x * value, y * value, z * value, w * value); }
	CG3DVector4 operator/(float value) { return CG3DVector4(x / value, y / value, z / value, w / value); }
	
	CG3DVector4& operator+=(CG3DVector4 vec) { x += vec.x; y += vec.y; z += vec.z; w += vec.w; return *this; }
	CG3DVector4& operator-=(CG3DVector4 vec) { x -= vec.x; y -= vec.y; z -= vec.z; w -= vec.w; return *this; }
	CG3DVector4& operator*=(CG3DVector4 vec) { x *= vec.x; y *= vec.y; z *= vec.z; w *= vec.w; return *this; }
	CG3DVector4& operator/=(CG3DVector4 vec) { x /= vec.x; y /= vec.y; z /= vec.z; w /= vec.w; return *this; }
	CG3DVector4& operator*=(float value) { x *= value; y *= value; z *= value; w *= value; return *this; }
	CG3DVector4& operator/=(float value) { x /= value; y /= value; z /= value; w /= value; return *this; }
};

#endif