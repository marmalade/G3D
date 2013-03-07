#ifndef G3D_MATHS_MATRIX_H
#define G3D_MATHS_MATRIX_H

//4x4 matrix
class CG3DMatrix {
public:
	float m11;
	float m12;
	float m13;
	float m14;
	float m21;
	float m22;
	float m23;
	float m24;
	float m31;
	float m32;
	float m33;
	float m34;
	float m41;
	float m42;
	float m43;
	float m44;
public:
	static CG3DMatrix s_Identity;
public:
	CG3DMatrix() : m11(1), m12(0), m13(0), m14(0), m21(0), m22(1), m23(0), m24(0), m31(0), m32(0), m33(1), m34(0), m41(0), m42(0), m43(0), m44(1) {}
	CG3DMatrix(float _11,float _12,float _13,float _14,float _21,float _22,float _23,float _24,float _31,float _32,float _33,float _34,float _41,float _42,float _43,float _44) :
		m11(_11), m12(_12), m13(_13), m14(_14), m21(_21), m22(_22), m23(_23), m24(_24), m31(_31), m32(_32), m33(_33), m34(_34), m41(_41), m42(_42), m43(_43), m44(_44) {}

	static CG3DMatrix CreateBillboard(CG3DVector3 objectPos, CG3DVector3 cameraPos,CG3DVector3 up, CG3DVector3 forward = CG3DVector3::s_Forward);
	static CG3DMatrix CreateTranslation(CG3DVector3 pos);
	static CG3DMatrix CreateTranslation(float x, float y, float z);
	static CG3DMatrix CreateScale(CG3DVector3 scale);
	static CG3DMatrix CreateScale(float x, float y, float z);
	static CG3DMatrix CreateRotationX(float angle);
	static CG3DMatrix CreateRotationY(float angle);
	static CG3DMatrix CreateRotationZ(float angle);
	static CG3DMatrix CreateRotationEuler(float x, float y, float z);
	static CG3DMatrix CreateRotationEuler(CG3DVector3 vec);
	static CG3DMatrix CreateFromAxisAngle(CG3DVector3 vec, float angle);
	static CG3DMatrix CreateFromQuaternion(CG3DQuaternion quat);
	static CG3DMatrix CreatePerspectiveFOV(float fieldOfView, float aspect, float nearZ, float farZ);
	static CG3DMatrix CreatePerspective(float width, float height, float nearZ, float farZ);
	static CG3DMatrix CreatePerspectiveOffCentre(float left, float right, float top, float bottom, float nearZ, float farZ);
	static CG3DMatrix CreateOrthographic(float width, float height, float nearZ, float farZ);
	static CG3DMatrix CreateOrthographicOffCentre(float left, float right, float top, float bottom, float nearZ, float farZ);
	static CG3DMatrix CreateLookAt(CG3DVector3 pos, CG3DVector3 target, CG3DVector3 up);
	static CG3DMatrix CreateWorld(CG3DVector3 pos, CG3DVector3 forward, CG3DVector3 up);
	
	CG3DVector3 GetVectorX() { return CG3DVector3(m11, m12, m13); }
	CG3DVector3 GetVectorY() { return CG3DVector3(m21, m22, m23); }
	CG3DVector3 GetVectorZ() { return CG3DVector3(m31, m32, m33); }
	CG3DVector3 GetTranslation() { return CG3DVector3(m41, m42, m43); }
	
	void SetVectorX(CG3DVector3 vec) { m11 = vec.x; m12 = vec.y; m13 = vec.z; }
	void SetVectorY(CG3DVector3 vec) { m21 = vec.x; m22 = vec.y; m23 = vec.z; }
	void SetVectorZ(CG3DVector3 vec) { m31 = vec.x; m32 = vec.y; m33 = vec.z; }
	void SetTranslation(CG3DVector3 vec) { m41 = vec.x; m42 = vec.y; m43 = vec.z; }
	void SetScale(CG3DVector3 vec) { m11 = vec.x; m22 = vec.y; m33 = vec.z; m12 = m13 = m21 = m23 = m31 = m32 = 0; }
	void SetEuler(CG3DVector3 vec);

	CG3DVector3 GetUp() { return CG3DVector3(m21, m22, m23); }
	CG3DVector3 GetDown() { return CG3DVector3(-m21, -m22, -m23); }
	CG3DVector3 GetLeft() { return CG3DVector3(-m11, -m12, -m13); }
	CG3DVector3 GetRight() { return CG3DVector3(m11, m12, m13); }
	CG3DVector3 GetForwards() { return CG3DVector3(-m31, -m32, -m33); }
	CG3DVector3 GetBackwards() { return CG3DVector3(m31, m32, m33); }
	
	void SetUp(CG3DVector3 vec) { m21 = vec.x; m22 = vec.y; m23 = vec.z; }
	void SetDown(CG3DVector3 vec) { m21 = -vec.x; m22 = -vec.y; m23 = -vec.z; }
	void SetLeft(CG3DVector3 vec) { m11 = -vec.x; m12 = -vec.y; m13 = -vec.z; }
	void SetRight(CG3DVector3 vec) { m11 = vec.x; m12 = vec.y; m13 = vec.z; }
	void SetForwards(CG3DVector3 vec) { m31 = -vec.x; m32 = -vec.y; m33 = -vec.z; }
	void SetBackwards(CG3DVector3 vec) { m31 = vec.x; m32 = vec.y; m33 = vec.z; }

	float Determinant();
	CG3DMatrix Transpose() { return CG3DMatrix(m11, m21, m31, m41, m12, m22, m32, m42, m13, m23, m33, m43, m14, m24, m34, m44); }
	void TransposeMe() { float t; t = m12; m12 = m21; m21 = t; t = m13; m13 = m31; m31 = t; t = m14; m14 = m41; m41 = t;
		t = m23; m23 = m32; m32 = t; t = m24; m24 = m42; m42 = t; t = m34; m34 = m43; m43 = t; }
	CG3DMatrix Invert();
	void InvertMe() { CG3DMatrix inv = Invert(); *this = inv; }
	
	CG3DMatrix& operator=(CG3DMatrix mat) { m11=mat.m11; m12=mat.m12; m13=mat.m13; m14=mat.m14; m21=mat.m21; m22=mat.m22; m23=mat.m23; m24=mat.m24;
		m31=mat.m31; m32=mat.m32; m33=mat.m33; m34=mat.m34; m41=mat.m41; m42=mat.m42; m43=mat.m43; m44=mat.m44; return *this; }
	CG3DMatrix operator-() { return CG3DMatrix(-m11, -m12, -m13, -m14, -m21, -m22, -m23, -m24, -m31, -m32, -m33, -m34, -m41, -m42, -m43, -m44); }
	bool operator==(CG3DMatrix mat) { return mat.m11==m11 && mat.m12==m12 && mat.m13==m13 && mat.m14==m14 && mat.m21==m21 && mat.m22==m22 && mat.m23==m23 && mat.m24==m24 &&
		mat.m31==m31 && mat.m32==m32 && mat.m33==m33 && mat.m34==m34 && mat.m41==m41 && mat.m42==m42 && mat.m43==m43 && mat.m44==m44; }
	bool operator!=(CG3DMatrix mat) { return mat.m11!=m11 || mat.m12!=m12 || mat.m13!=m13 || mat.m14!=m14 || mat.m21!=m21 || mat.m22!=m22 || mat.m23!=m23 || mat.m24!=m24 ||
		mat.m31!=m31 || mat.m32!=m32 || mat.m33!=m33 || mat.m34!=m34 || mat.m41!=m41 || mat.m42!=m42 || mat.m43!=m43 || mat.m44!=m44; }

	CG3DMatrix operator+(CG3DMatrix mat) { return CG3DMatrix(m11+mat.m11, m12+mat.m12, m13+mat.m13, m14+mat.m14, m21+mat.m21, m22+mat.m22, m23+mat.m23, m24+mat.m24,
		m31+mat.m31, m32+mat.m32, m33+mat.m33, m34+mat.m34, m41+mat.m41, m42+mat.m42, m43+mat.m43, m44+mat.m44 ); }
	CG3DMatrix operator-(CG3DMatrix mat) { return CG3DMatrix(m11-mat.m11, m12-mat.m12, m13-mat.m13, m14-mat.m14, m21-mat.m21, m22-mat.m22, m23-mat.m23, m24-mat.m24,
		m31-mat.m31, m32-mat.m32, m33-mat.m33, m34-mat.m34, m41-mat.m41, m42-mat.m42, m43-mat.m43, m44-mat.m44 ); }
	CG3DMatrix operator*(CG3DMatrix mat) { return CG3DMatrix(m11*mat.m11 + m12*mat.m21 + m13*mat.m31 + m14*mat.m41, m11*mat.m12 + m12*mat.m22 + m13*mat.m32 + m14*mat.m42,
		m11*mat.m13 + m12*mat.m23 + m13*mat.m33 + m14*mat.m43, m11*mat.m14 + m12*mat.m24 + m13*mat.m34 + m14*mat.m44, m21*mat.m11 + m22*mat.m21 + m23*mat.m31 + m24*mat.m41,
		m21*mat.m12 + m22*mat.m22 + m23*mat.m32 + m24*mat.m42, m21*mat.m13 + m22*mat.m23 + m23*mat.m33 + m24*mat.m43, m21*mat.m14 + m22*mat.m24 + m23*mat.m34 + m24*mat.m44,
		m31*mat.m11 + m32*mat.m21 + m33*mat.m31 + m34*mat.m41, m31*mat.m12 + m32*mat.m22 + m33*mat.m32 + m34*mat.m42, m31*mat.m13 + m32*mat.m23 + m33*mat.m33 + m34*mat.m43,
		m31*mat.m14 + m32*mat.m24 + m33*mat.m34 + m34*mat.m44, m41*mat.m11 + m42*mat.m21 + m43*mat.m31 + m44*mat.m41, m41*mat.m12 + m42*mat.m22 + m43*mat.m32 + m44*mat.m42,
		m41*mat.m13 + m42*mat.m23 + m43*mat.m33 + m44*mat.m43, m41*mat.m14 + m42*mat.m24 + m43*mat.m34 + m44*mat.m44); }
	CG3DMatrix operator/(CG3DMatrix mat) { return CG3DMatrix(m11/mat.m11, m12/mat.m12, m13/mat.m13, m14/mat.m14, m21/mat.m21, m22/mat.m22, m23/mat.m23, m24/mat.m24,
		m31/mat.m31, m32/mat.m32, m33/mat.m33, m34/mat.m34, m41/mat.m41, m42/mat.m42, m43/mat.m43, m44/mat.m44); }
	CG3DMatrix operator*(float value) { return CG3DMatrix(m11*value, m12*value, m13*value, m14*value, m21*value, m22*value, m23*value, m24*value,
		m31*value, m32*value, m33*value, m34*value, m41*value, m42*value, m43*value, m44*value); }
	CG3DMatrix operator/(float value) { return CG3DMatrix(m11/value, m12/value, m13/value, m14/value, m21/value, m22/value, m23/value, m24/value,
		m31/value, m32/value, m33/value, m34/value, m41/value, m42/value, m43/value, m44/value); }
	
	CG3DMatrix& operator+=(CG3DMatrix mat) { m11 += mat.m11; m12 += mat.m12; m13 += mat.m13; m14 += mat.m14; m21 += mat.m21; m22 += mat.m22; m23 += mat.m23; m24 += mat.m24;
		m31 += mat.m31; m32 += mat.m32; m33 += mat.m33; m34 += mat.m34; m41 += mat.m41; m42 += mat.m42; m43 += mat.m43; m44 += mat.m44; return *this; }
	CG3DMatrix& operator-=(CG3DMatrix mat) { m11 -= mat.m11; m12 -= mat.m12; m13 -= mat.m13; m14 -= mat.m14; m21 -= mat.m21; m22 -= mat.m22; m23 -= mat.m23; m24 -= mat.m24;
		m31 -= mat.m31; m32 -= mat.m32; m33 -= mat.m33; m34 -= mat.m34; m41 -= mat.m41; m42 -= mat.m42; m43 -= mat.m43; m44 -= mat.m44; return *this; }
	CG3DMatrix& operator*=(CG3DMatrix mat) { m11 = m11*mat.m11 + m12*mat.m21 + m13*mat.m31 + m14*mat.m41; m12 = m11*mat.m12 + m12*mat.m22 + m13*mat.m32 + m14*mat.m42;
		m13 = m11*mat.m13 + m12*mat.m23 + m13*mat.m33 + m14*mat.m43; m14 = m11*mat.m14 + m12*mat.m24 + m13*mat.m34 + m14*mat.m44;
		m21 = m21*mat.m11 + m22*mat.m21 + m23*mat.m31 + m24*mat.m41; m22 = m21*mat.m12 + m22*mat.m22 + m23*mat.m32 + m24*mat.m42;
		m23 = m21*mat.m13 + m22*mat.m23 + m23*mat.m33 + m24*mat.m43; m24 = m21*mat.m14 + m22*mat.m24 + m23*mat.m34 + m24*mat.m44;
		m31 = m31*mat.m11 + m32*mat.m21 + m33*mat.m31 + m34*mat.m41; m32 = m31*mat.m12 + m32*mat.m22 + m33*mat.m32 + m34*mat.m42;
		m33 = m31*mat.m13 + m32*mat.m23 + m33*mat.m33 + m34*mat.m43; m34 = m31*mat.m14 + m32*mat.m24 + m33*mat.m34 + m34*mat.m44;
		m41 = m41*mat.m11 + m42*mat.m21 + m43*mat.m31 + m44*mat.m41; m42 = m41*mat.m12 + m42*mat.m22 + m43*mat.m32 + m44*mat.m42;
		m43 = m41*mat.m13 + m42*mat.m23 + m43*mat.m33 + m44*mat.m43; m44 = m41*mat.m14 + m42*mat.m24 + m43*mat.m34 + m44*mat.m44; return *this; }
	CG3DMatrix& operator/=(CG3DMatrix mat) { m11 /= mat.m11; m12 /= mat.m12; m13 /= mat.m13; m14 /= mat.m14; m21 /= mat.m21; m22 /= mat.m22; m23 /= mat.m23; m24 /= mat.m24;
		m31 /= mat.m31; m32 /= mat.m32; m33 /= mat.m33; m34 /= mat.m34; m41 /= mat.m41; m42 /= mat.m42; m43 /= mat.m43; m44 /= mat.m44; return *this; }
	CG3DMatrix& operator*=(float value) { m11 *= value; m12 *= value; m13 *= value; m14 *= value; m21 *= value; m22 *= value; m23 *= value; m24 *= value;
		m31 *= value; m32 *= value; m33 *= value; m34 *= value; m41 *= value; m42 *= value; m43 *= value; m44 *= value; return *this; }
	CG3DMatrix& operator/=(float value) { m11 /= value; m12 /= value; m13 /= value; m14 /= value; m21 /= value; m22 /= value; m23 /= value; m24 /= value;
		m31 /= value; m32 /= value; m33 /= value; m34 /= value; m41 /= value; m42 /= value; m43 /= value; m44 /= value; return *this; }

	CG3DVector2 Transform(CG3DVector2 vec) { return CG3DVector2(vec.x * m11 + vec.y * m21 + m41, vec.x * m12 + vec.y * m22 + m42); }
	CG3DVector2 TransformNormal(CG3DVector2 vec) { return CG3DVector2(vec.x * m11 + vec.y * m21, vec.x * m12 + vec.y * m22); }
	CG3DVector3 Transform(CG3DVector3 vec) { return CG3DVector3(vec.x * m11 + vec.y * m21 + vec.z * m31 + m41, vec.x * m12 + vec.y * m22 + vec.z * m32 + m42,
		vec.x * m13 + vec.y * m23 + vec.z * m33 + m43); }
	CG3DVector3 TransformNormal(CG3DVector3 vec) { return CG3DVector3(vec.x * m11 + vec.y * m21 + vec.z * m31, vec.x * m12 + vec.y * m22 + vec.z * m32,
		vec.x * m13 + vec.y * m23 + vec.z * m33); }
	CG3DVector4 Transform(CG3DVector4 vec) { return CG3DVector4(vec.x * m11 + vec.y * m21 + vec.z * m31 + vec.w * m41,
		vec.x * m12 + vec.y * m22 + vec.z * m32 + vec.w * m42, vec.x * m13 + vec.y * m23 + vec.z * m33 + vec.w * m43, vec.x * m14 + vec.y * m24 + vec.z * m34 + vec.w * m44); }
};

#endif