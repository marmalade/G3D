#ifndef G3D_MATHS_MATRIX33_H
#define G3D_MATHS_MATRIX33_H

//4x4 matrix
class CG3DMatrix33 {
public:
	float m11;
	float m12;
	float m13;
	float m21;
	float m22;
	float m23;
	float m31;
	float m32;
	float m33;
public:
	static CG3DMatrix33 s_Identity;
public:
	CG3DMatrix33() : m11(1), m12(0), m13(0), m21(0), m22(1), m23(0), m31(0), m32(0), m33(1) {}
	CG3DMatrix33(float _11,float _12,float _13,float _21,float _22,float _23,float _31,float _32,float _33) :
		m11(_11), m12(_12), m13(_13), m21(_21), m22(_22), m23(_23), m31(_31), m32(_32), m33(_33) {}

	static CG3DMatrix33 CreateTranslation(CG3DVector2 pos);
	static CG3DMatrix33 CreateTranslation(float x, float y);
	static CG3DMatrix33 CreateScale(CG3DVector2 scale);
	static CG3DMatrix33 CreateScale(float x, float y);
	static CG3DMatrix33 CreateRotation(float angle);
	
	CG3DVector2 GetVectorX() { return CG3DVector2(m11, m12); }
	CG3DVector2 GetVectorY() { return CG3DVector2(m21, m22); }
	CG3DVector2 GetTranslation() { return CG3DVector2(m31, m32); }
	
	void SetVectorX(CG3DVector2 vec) { m11 = vec.x; m12 = vec.y; }
	void SetVectorY(CG3DVector2 vec) { m21 = vec.x; m22 = vec.y; }
	void SetTranslation(CG3DVector2 vec) { m31 = vec.x; m32 = vec.y; }
	void SetScale(CG3DVector2 vec) { m11 = vec.x; m22 = vec.y; m12 = m21 = 0; }

	CG3DVector2 GetUp() { return CG3DVector2(m21, m22); }
	CG3DVector2 GetDown() { return CG3DVector2(-m21, -m22); }
	CG3DVector2 GetLeft() { return CG3DVector2(-m11, -m12); }
	CG3DVector2 GetRight() { return CG3DVector2(m11, m12); }
	
	void SetUp(CG3DVector2 vec) { m21 = vec.x; m22 = vec.y; }
	void SetDown(CG3DVector2 vec) { m21 = -vec.x; m22 = -vec.y; }
	void SetLeft(CG3DVector2 vec) { m11 = -vec.x; m12 = -vec.y; }
	void SetRight(CG3DVector2 vec) { m11 = vec.x; m12 = vec.y; }

	//float Determinant();
	CG3DMatrix33 Transpose() { return CG3DMatrix33(m11, m21, m31, m12, m22, m32, m13, m23, m33); }
	void TransposeMe() { float t; t = m12; m12 = m21; m21 = t; t = m13; m13 = m31; m31 = t; t = m23; m23 = m32; m32 = t; }
	//CG3DMatrix Invert();
	//void InvertMe() { CG3DMatrix inv = Invert(); *this = inv; }
	
	CG3DMatrix33& operator=(CG3DMatrix33 mat) { m11=mat.m11; m12=mat.m12; m13=mat.m13; m21=mat.m21; m22=mat.m22; m23=mat.m23;
		m31=mat.m31; m32=mat.m32; m33=mat.m33; return *this; }
	CG3DMatrix33 operator-() { return CG3DMatrix33(-m11, -m12, -m13, -m21, -m22, -m23, -m31, -m32, -m33); }
	bool operator==(CG3DMatrix33 mat) { return mat.m11==m11 && mat.m12==m12 && mat.m13==m13 && mat.m21==m21 && mat.m22==m22 && mat.m23==m23 &&
		mat.m31==m31 && mat.m32==m32 && mat.m33==m33; }
	bool operator!=(CG3DMatrix33 mat) { return mat.m11!=m11 || mat.m12!=m12 || mat.m13!=m13 || mat.m21!=m21 || mat.m22!=m22 || mat.m23!=m23 ||
		mat.m31!=m31 || mat.m32!=m32 || mat.m33!=m33; }

	CG3DMatrix33 operator+(CG3DMatrix33 mat) { return CG3DMatrix33(m11+mat.m11, m12+mat.m12, m13+mat.m13, m21+mat.m21, m22+mat.m22, m23+mat.m23,
		m31+mat.m31, m32+mat.m32, m33+mat.m33); }
	CG3DMatrix33 operator-(CG3DMatrix33 mat) { return CG3DMatrix33(m11-mat.m11, m12-mat.m12, m13-mat.m13, m21-mat.m21, m22-mat.m22, m23-mat.m23,
		m31-mat.m31, m32-mat.m32, m33-mat.m33); }
	CG3DMatrix33 operator*(CG3DMatrix33 mat) { return CG3DMatrix33(
		m11*mat.m11 + m12*mat.m21 + m13*mat.m31, m11*mat.m12 + m12*mat.m22 + m13*mat.m32, m11*mat.m13 + m12*mat.m23 + m13*mat.m33,
		m21*mat.m11 + m22*mat.m21 + m23*mat.m31, m21*mat.m12 + m22*mat.m22 + m23*mat.m32, m21*mat.m13 + m22*mat.m23 + m23*mat.m33,
		m31*mat.m11 + m32*mat.m21 + m33*mat.m31, m31*mat.m12 + m32*mat.m22 + m33*mat.m32, m31*mat.m13 + m32*mat.m23 + m33*mat.m33); }
	CG3DMatrix33 operator/(CG3DMatrix mat) { return CG3DMatrix33(m11/mat.m11, m12/mat.m12, m13/mat.m13, m21/mat.m21, m22/mat.m22, m23/mat.m23,
		m31/mat.m31, m32/mat.m32, m33/mat.m33); }
	CG3DMatrix33 operator*(float value) { return CG3DMatrix33(m11*value, m12*value, m13*value, m21*value, m22*value, m23*value,
		m31*value, m32*value, m33*value); }
	CG3DMatrix33 operator/(float value) { return CG3DMatrix33(m11/value, m12/value, m13/value, m21/value, m22/value, m23/value,
		m31/value, m32/value, m33/value); }
	
	CG3DMatrix33& operator+=(CG3DMatrix33 mat) { m11 += mat.m11; m12 += mat.m12; m13 += mat.m13; m21 += mat.m21; m22 += mat.m22; m23 += mat.m23;
		m31 += mat.m31; m32 += mat.m32; m33 += mat.m33; return *this; }
	CG3DMatrix33& operator-=(CG3DMatrix33 mat) { m11 -= mat.m11; m12 -= mat.m12; m13 -= mat.m13; m21 -= mat.m21; m22 -= mat.m22; m23 -= mat.m23;
		m31 -= mat.m31; m32 -= mat.m32; m33 -= mat.m33; return *this; }
	CG3DMatrix33& operator*=(CG3DMatrix33 mat) {
		m11 = m11*mat.m11 + m12*mat.m21 + m13*mat.m31; m12 = m11*mat.m12 + m12*mat.m22 + m13*mat.m32; m13 = m11*mat.m13 + m12*mat.m23 + m13*mat.m33;
		m21 = m21*mat.m11 + m22*mat.m21 + m23*mat.m31; m22 = m21*mat.m12 + m22*mat.m22 + m23*mat.m32; m23 = m21*mat.m13 + m22*mat.m23 + m23*mat.m33;
		m31 = m31*mat.m11 + m32*mat.m21 + m33*mat.m31; m32 = m31*mat.m12 + m32*mat.m22 + m33*mat.m32; m33 = m31*mat.m13 + m32*mat.m23 + m33*mat.m33;
		return *this; }
	CG3DMatrix33& operator/=(CG3DMatrix33 mat) { m11 /= mat.m11; m12 /= mat.m12; m13 /= mat.m13; m21 /= mat.m21; m22 /= mat.m22; m23 /= mat.m23;
		m31 /= mat.m31; m32 /= mat.m32; m33 /= mat.m33; return *this; }
	CG3DMatrix33& operator*=(float value) { m11 *= value; m12 *= value; m13 *= value; m21 *= value; m22 *= value; m23 *= value;
		m31 *= value; m32 *= value; m33 *= value; return *this; }
	CG3DMatrix33& operator/=(float value) { m11 /= value; m12 /= value; m13 /= value; m21 /= value; m22 /= value; m23 /= value;
		m31 /= value; m32 /= value; m33 /= value; return *this; }

	CG3DVector2 Transform(CG3DVector2 vec) { return CG3DVector2(vec.x * m11 + vec.y * m21 + m31, vec.x * m12 + vec.y * m22 + m32); }
	CG3DVector2 TransformNormal(CG3DVector2 vec) { return CG3DVector2(vec.x * m11 + vec.y * m21, vec.x * m12 + vec.y * m22); }
	CG3DVector3 Transform(CG3DVector3 vec) { return CG3DVector3(vec.x * m11 + vec.y * m21 + vec.z * m31, vec.x * m12 + vec.y * m22 + vec.z * m32,
		vec.x * m13 + vec.y * m23 + vec.z * m33); }
};

#endif