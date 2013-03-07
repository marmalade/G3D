#include "Maths.h"

CG3DMatrix33 CG3DMatrix33::s_Identity(1, 0, 0, 0, 1, 0, 0, 0, 1);

CG3DMatrix33 CG3DMatrix33::CreateTranslation(CG3DVector2 pos)
{
	CG3DMatrix33 result;
	result.SetTranslation(pos);
	return result;
}

CG3DMatrix33 CG3DMatrix33::CreateTranslation(float x, float y)
{
	CG3DMatrix33 result;
	result.SetTranslation(CG3DVector2(x, y));
	return result;
}

CG3DMatrix33 CG3DMatrix33::CreateScale(CG3DVector2 scale)
{
	CG3DMatrix33 result;
	result.SetScale(scale);
	return result;
}

CG3DMatrix33 CG3DMatrix33::CreateScale(float x, float y)
{
	CG3DMatrix33 result;
	result.SetScale(CG3DVector2(x, y));
	return result;
}
CG3DMatrix33 CG3DMatrix33::CreateRotation(float angle)
{
	float cval = cosf(angle), sval = sinf(angle);
	CG3DMatrix33 result;
	result.SetVectorX(CG3DVector2(cval, sval));
	result.SetVectorY(CG3DVector2(-sval, cval));
	return result;
}

/*
float CG3DMatrix33::Determinant() {
	return  m11 * (m22 * (m33 * m44 - m34 * m43) - m23 * (m32 * m44 - m34 * m42) + m24 * (m32 * m43 - m33 * m42)) -
			m12 * (m21 * (m33 * m44 - m34 * m43) - m23 * (m31 * m44 - m34 * m41) + m24 * (m31 * m43 - m33 * m41)) +
			m13 * (m21 * (m32 * m44 - m34 * m42) - m22 * (m31 * m44 - m34 * m41) + m24 * (m31 * m42 - m32 * m41)) -
			m14 * (m21 * (m32 * m43 - m33 * m42) - m22 * (m31 * m43 - m33 * m41) + m23 * (m31 * m42 - m32 * m41));
}

CG3DMatrix33 CG3DMatrix33::Invert()
{
	CG3DMatrix33 result;

	float nux = m33 * m44 - m34 * m43;
	float nux2 = m32 * m44 - m34 * m42;
	float nux3 = m32 * m43 - m33 * m42;
	float nux4 = m31 * m44 - m34 * m41;
	float nux5 = m31 * m43 - m33 * m41;
	float nux6 = m31 * m42 - m32 * m41;
	float nux7 = m22 * nux - m23 * nux2 + m24 * nux3;
	float nux8 = -(m21 * nux - m23 * nux4 + m24 * nux5);
	float nux9 = m21 * nux2 - m22 * nux4 + m24 * nux6;
	float nux10 = -(m21 * nux3 - m22 * nux5 + m23 * nux6);
	float nux11 = 1.0f / (m11 * nux7 + m12 * nux8 + m13 * nux9 + m14 * nux10);
	result.m11 = nux7 * nux11;
	result.m21 = nux8 * nux11;
	result.m31 = nux9 * nux11;
	result.m41 = nux10 * nux11;
	result.m12 = -(m12 * nux - m13 * nux2 + m14 * nux3) * nux11;
	result.m22 = (m11 * nux - m13 * nux4 + m14 * nux5) * nux11;
	result.m32 = -(m11 * nux2 - m12 * nux4 + m14 * nux6) * nux11;
	result.m42 = (m11 * nux3 - m12 * nux5 + m13 * nux6) * nux11;
	float nux12 = m23 * m44 - m24 * m43;
	float nux13 = m22 * m44 - m24 * m42;
	float nux14 = m22 * m43 - m23 * m42;
	float nux15 = m21 * m44 - m24 * m41;
	float nux16 = m21 * m43 - m23 * m41;
	float nux17 = m21 * m42 - m22 * m41;
	result.m13 = (m12 * nux12 - m13 * nux13 + m14 * nux14) * nux11;
	result.m23 = -(m11 * nux12 - m13 * nux15 + m14 * nux16) * nux11;
	result.m33 = (m11 * nux13 - m12 * nux15 + m14 * nux17) * nux11;
	result.m43 = -(m11 * nux14 - m12 * nux16 + m13 * nux17) * nux11;
	float nux18 = m23 * m34 - m24 * m33;
	float nux19 = m22 * m34 - m24 * m32;
	float nux20 = m22 * m33 - m23 * m32;
	float nux21 = m21 * m34 - m24 * m31;
	float nux22 = m21 * m33 - m23 * m31;
	float nux23 = m21 * m32 - m22 * m31;
	result.m14 = -(m12 * nux18 - m13 * nux19 + m14 * nux20) * nux11;
	result.m24 = (m11 * nux18 - m13 * nux21 + m14 * nux22) * nux11;
	result.m34 = -(m11 * nux19 - m12 * nux21 + m14 * nux23) * nux11;
	result.m44 = (m11 * nux20 - m12 * nux22 + m13 * nux23) * nux11;
	return result;
}*/