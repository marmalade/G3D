#ifndef G3D_HIERARCHY_FRAME_MODULE_H
#define G3D_HIERARCHY_FRAME_MODULE_H

#include "G3DCore.h"
#include "UpdateModule.h"

//modules that specify the local matrix of the game object

//base class for frame modules, has data:
//	CG3DMatrix local
class CG3DFrameModule : public CG3DUpdateModule
{
public:
	CG3DFrameModule() : CG3DUpdateModule(99) {}
	virtual void DoUpdate(float timeStep);

	virtual CG3DMatrix GetLocal() = 0;
protected:
	virtual void* GetDataInternal(const char* name);
public:
	CG3DMatrix m_Local;
};

//local matrix is always the identity matrix
class CG3DFrameModuleIdentity : public CG3DFrameModule
{
public:
	CG3DFrameModuleIdentity() {}
	virtual CG3DMatrix GetLocal() { return CG3DMatrix::s_Identity; }
};

//frame module defined by position, rotation (as quaternion) and scale, has data:
//	CG3DVector3 pos
//	CG3DQuaternion quat
//	CG3DVector3 scale
class CG3DFrameModulePQS : public CG3DFrameModule
{
public:
	CG3DFrameModulePQS(CG3DVector3 pos, CG3DQuaternion quat, CG3DVector3 scale) : m_Pos(pos), m_Quat(quat), m_Scale(scale) {}
	virtual CG3DMatrix GetLocal();
protected:
	virtual void* GetDataInternal(const char* name);
public:
	CG3DVector3 m_Pos;
	CG3DQuaternion m_Quat;
	CG3DVector3 m_Scale;
};

//frame module defined by position, rotation (as euler) and scale, has data:
//	CG3DVector3 pos
//	CG3DVector3 rot
//	CG3DVector3 scale
class CG3DFrameModulePRS : public CG3DFrameModule
{
public:
	CG3DFrameModulePRS(CG3DVector3 pos, CG3DVector3 rot, CG3DVector3 scale) : m_Pos(pos), m_Rot(rot), m_Scale(scale) {}
	virtual CG3DMatrix GetLocal();
protected:
	virtual void* GetDataInternal(const char* name);
public:
	CG3DVector3 m_Pos;
	CG3DVector3 m_Rot;
	CG3DVector3 m_Scale;
};

//frame module defined by matrix, has data:
//	CG3DMatrix mat
class CG3DFrameModuleM : public CG3DFrameModule
{
public:
	CG3DFrameModuleM(CG3DMatrix mat) : m_Mat(mat) {}
	virtual CG3DMatrix GetLocal();
protected:
	virtual void* GetDataInternal(const char* name);
public:
	CG3DMatrix m_Mat;
};

#endif