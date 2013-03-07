#include "Maths.h"

CG3DMatrix CG3DMatrix::s_Identity(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1);

CG3DMatrix CG3DMatrix::CreateBillboard(CG3DVector3 objectPos, CG3DVector3 cameraPos,CG3DVector3 up, CG3DVector3 forward)
{
	CG3DVector3 vec = objectPos - cameraPos;
	float num = vec.LengthSquared();
	if (num < 0.0001f)
		vec = forward;
	else
		vec *= 1.0f / sqrtf(num);

	CG3DVector3 vec2 = up.Cross(vec).Normalise();
	CG3DVector3 vec3 = vec.Cross(vec2);
	
	CG3DMatrix result;
	result.SetVectorX(vec3);
	result.SetVectorY(vec2);
	result.SetVectorZ(vec);
	result.SetTranslation(objectPos);
	return result;
}

CG3DMatrix CG3DMatrix::CreateTranslation(CG3DVector3 pos)
{
	CG3DMatrix result;
	result.SetTranslation(pos);
	return result;
}

CG3DMatrix CG3DMatrix::CreateTranslation(float x, float y, float z)
{
	CG3DMatrix result;
	result.SetTranslation(CG3DVector3(x, y, z));
	return result;
}

CG3DMatrix CG3DMatrix::CreateScale(CG3DVector3 scale)
{
	CG3DMatrix result;
	result.SetScale(scale);
	return result;
}

CG3DMatrix CG3DMatrix::CreateScale(float x, float y, float z)
{
	CG3DMatrix result;
	result.SetScale(CG3DVector3(x, y, z));
	return result;
}
CG3DMatrix CG3DMatrix::CreateRotationX(float angle)
{
	float cval = cosf(angle), sval = sinf(angle);
	CG3DMatrix result;
	result.SetVectorX(CG3DVector3(0, 0, 0));
	result.SetVectorY(CG3DVector3(0, cval, sval));
	result.SetVectorZ(CG3DVector3(0, -sval, cval));
	return result;
}
CG3DMatrix CG3DMatrix::CreateRotationY(float angle)
{
	float cval = cosf(angle), sval = sinf(angle);
	CG3DMatrix result;
	result.SetVectorX(CG3DVector3(cval, 0, -sval));
	result.SetVectorY(CG3DVector3(0, 0, 0));
	result.SetVectorZ(CG3DVector3(sval, 0, cval));
	return result;
}
CG3DMatrix CG3DMatrix::CreateRotationZ(float angle)
{
	float cval = cosf(angle), sval = sinf(angle);
	CG3DMatrix result;
	result.SetVectorX(CG3DVector3(cval, sval, 0));
	result.SetVectorY(CG3DVector3(-sval, cval, 0));
	result.SetVectorZ(CG3DVector3(0, 0, 0));
	return result;
}

CG3DMatrix CG3DMatrix::CreateRotationEuler(float x, float y, float z)
{
	CG3DMatrix result;
	result.SetEuler(CG3DVector3(x, y, z));
	return result;
}

CG3DMatrix CG3DMatrix::CreateRotationEuler(CG3DVector3 vec)
{
	CG3DMatrix result;
	result.SetEuler(vec);
	return result;
}

void CG3DMatrix::SetEuler(CG3DVector3 vec)
{
	//b,h,a
	CG3DVector3 cval(cosf(vec.x), cosf(vec.y), cosf(vec.z));
	CG3DVector3 sval(sinf(vec.x), sinf(vec.y), sinf(vec.z));

	SetVectorX(CG3DVector3(cval.y*cval.z, -cval.y*sval.z*cval.x+sval.y*sval.x, cval.y*sval.z*sval.x+sval.y*cval.x));
	SetVectorY(CG3DVector3(sval.z,        cval.z*cval.x,                       -cval.z*sval.x));
	SetVectorZ(CG3DVector3(-sval.y*cval.z,sval.y*sval.z*cval.x + cval.y*sval.x,-sval.y*sval.z*sval.x + cval.y*cval.x));
}

CG3DMatrix CG3DMatrix::CreateFromAxisAngle(CG3DVector3 vec, float angle)
{
	float cval = cosf(angle), sval = sinf(angle);
	CG3DVector3 sq(vec.x * vec.x, vec.y * vec.y, vec.z * vec.z);
	CG3DVector3 cr(vec.y * vec.z, vec.x * vec.z, vec.x * vec.y);
	
	CG3DMatrix result;
	result.SetVectorX(CG3DVector3(sq.x + cval * (1.0f - sq.x),       cr.z - cval * cr.z + sval * vec.z, cr.y - cval * cr.y - sval * vec.y));
	result.SetVectorY(CG3DVector3(cr.z - cval * cr.z - sval * vec.z, sq.y + cval * (1.0f - sq.y),       cr.x - cval * cr.x + sval * vec.x));
	result.SetVectorZ(CG3DVector3(cr.y - cval * cr.y + sval * vec.y, cr.x - cval * cr.x - sval * vec.x, sq.y + cval * (1.0f - sq.y)));
	return result;
}

CG3DMatrix CG3DMatrix::CreateFromQuaternion(CG3DQuaternion quat)
{
	CG3DVector3 sq(quat.x * quat.x, quat.y * quat.y, quat.z * quat.z);
	CG3DVector3 cr(quat.y * quat.z, quat.x* quat.z, quat.x * quat.y);
	CG3DVector3 w(quat.x * quat.w, quat.y * quat.w, quat.z * quat.w);
	
	CG3DMatrix result;
	result.SetVectorX(CG3DVector3(1.0f - 2.0f * (sq.y + sq.z), 2.0f * (cr.z + w.z),         2.0f * (cr.y - w.y)));
	result.SetVectorY(CG3DVector3(2.0f * (cr.z - w.z),         1.0f - 2.0f * (sq.x + sq.z), 2.0f * (cr.x + w.x)));
	result.SetVectorZ(CG3DVector3(2.0f * (cr.y + w.y),         2.0f * (cr.x - w.x),         1.0f - 2.0f * (sq.x + sq.y)));
	return result;
}

CG3DMatrix CG3DMatrix::CreatePerspectiveFOV(float fieldOfView, float aspect, float nearZ, float farZ)
{
	float num = 1.0f / tanf(fieldOfView * 0.5f);
	float m = num / aspect;

	CG3DMatrix result;
	result.m11 = m;
	result.m22 = num;
	result.m33 = farZ / (nearZ - farZ);
	result.m34 = -1.0f;
	result.m43 = nearZ * farZ / (nearZ - farZ);
	return result;
}

CG3DMatrix CG3DMatrix::CreatePerspective(float width, float height, float nearZ, float farZ)
{
	CG3DMatrix result;
	result.m11 = 2.0f * nearZ / width;
	result.m22 = 2.0f * nearZ / height;
	result.m33 = farZ / (nearZ - farZ);
	result.m34 = -1.0f;
	result.m43 = nearZ * farZ / (nearZ - farZ);
	return result;
}

CG3DMatrix CG3DMatrix::CreatePerspectiveOffCentre(float left, float right, float top, float bottom, float nearZ, float farZ)
{
	CG3DMatrix result;
	result.m11 = 2.0f * nearZ / (right - left);
	result.m22 = 2.0f * nearZ / (top - bottom);
	result.m31 = (left + right) / (right - left);
	result.m32 = (top + bottom) / (top - bottom);
	result.m33 = farZ / (nearZ - farZ);
	result.m34 = -1.0f;
	result.m43 = nearZ * farZ / (nearZ - farZ);
	return result;
}

CG3DMatrix CG3DMatrix::CreateOrthographic(float width, float height, float nearZ, float farZ)
{
	CG3DMatrix result;
	result.m11 = 2.0f / width;
	result.m22 = 2.0f / height;
	result.m33 = 1.0f / (nearZ - farZ);
	result.m34 = -1.0f;
	result.m43 = nearZ / (nearZ - farZ);
	return result;
}

CG3DMatrix CG3DMatrix::CreateOrthographicOffCentre(float left, float right, float top, float bottom, float nearZ, float farZ)
{
	CG3DMatrix result;
	result.m11 = 2.0f / (right - left);
	result.m22 = 2.0f / (top - bottom);
	result.m33 = 1.0f / (nearZ - farZ);
	result.m41 = (left + right) / (right - left);
	result.m42 = (top + bottom) / (top - bottom);
	result.m34 = -1.0f;
	result.m43 = nearZ / (nearZ - farZ);
	return result;
}

CG3DMatrix CG3DMatrix::CreateLookAt(CG3DVector3 pos, CG3DVector3 target, CG3DVector3 up)
{
	CG3DVector3 camera = (pos - target).Normalise();
	CG3DVector3 cross = up.Cross(camera).Normalise();
	CG3DVector3 axis = camera.Cross(cross);
	CG3DMatrix result;
	result.SetVectorX(CG3DVector3(cross.x, axis.x, camera.x));
	result.SetVectorY(CG3DVector3(cross.y, axis.y, camera.y));
	result.SetVectorZ(CG3DVector3(cross.z, axis.z, camera.z));
	result.SetTranslation(CG3DVector3(-cross.Dot(pos), -axis.Dot(pos), -camera.Dot(pos)));
	return result;
}

CG3DMatrix CG3DMatrix::CreateWorld(CG3DVector3 pos, CG3DVector3 forward, CG3DVector3 up)
{
	CG3DVector3 dirn = -forward.Normalise();
	CG3DVector3 cross = up.Cross(dirn).Normalise();
	CG3DVector3 axis = dirn.Cross(cross);
	CG3DMatrix result;
	result.SetVectorX(cross);
	result.SetVectorY(axis);
	result.SetVectorZ(dirn);
	result.SetTranslation(pos);
	return result;
}

float CG3DMatrix::Determinant() {
	return  m11 * (m22 * (m33 * m44 - m34 * m43) - m23 * (m32 * m44 - m34 * m42) + m24 * (m32 * m43 - m33 * m42)) -
			m12 * (m21 * (m33 * m44 - m34 * m43) - m23 * (m31 * m44 - m34 * m41) + m24 * (m31 * m43 - m33 * m41)) +
			m13 * (m21 * (m32 * m44 - m34 * m42) - m22 * (m31 * m44 - m34 * m41) + m24 * (m31 * m42 - m32 * m41)) -
			m14 * (m21 * (m32 * m43 - m33 * m42) - m22 * (m31 * m43 - m33 * m41) + m23 * (m31 * m42 - m32 * m41));
}

CG3DMatrix CG3DMatrix::Invert()
{
	CG3DMatrix result;

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
}