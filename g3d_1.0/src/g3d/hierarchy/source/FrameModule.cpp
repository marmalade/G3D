#include "FrameModule.h"
#include "GameObject.h"

void CG3DFrameModule::DoUpdate(float timeStep)
{
	m_Local = GetLocal();
	m_Parent->DoEvent(new CG3DSetLocalEvent(m_Local));
}

void* CG3DFrameModule::GetDataInternal(const char* name)
{
	if(!strcmp(name, "local"))
		return &m_Local;
	return NULL;
}

CG3DMatrix CG3DFrameModulePQS::GetLocal()
{
	return CG3DMatrix::CreateScale(m_Scale) * CG3DMatrix::CreateFromQuaternion(m_Quat) * CG3DMatrix::CreateTranslation(m_Pos);
}

void* CG3DFrameModulePQS::GetDataInternal(const char* name)
{
	if(!strcmp(name, "pos"))
		return &m_Pos;
	if(!strcmp(name, "quat"))
		return &m_Quat;
	if(!strcmp(name, "scale"))
		return &m_Scale;
	return CG3DFrameModule::GetDataInternal(name);
}

CG3DMatrix CG3DFrameModulePRS::GetLocal()
{
	return CG3DMatrix::CreateScale(m_Scale) * CG3DMatrix::CreateRotationEuler(m_Rot) * CG3DMatrix::CreateTranslation(m_Pos);
}

void* CG3DFrameModulePRS::GetDataInternal(const char* name)
{
	if(!strcmp(name, "pos"))
		return &m_Pos;
	if(!strcmp(name, "rot"))
		return &m_Rot;
	if(!strcmp(name, "scale"))
		return &m_Scale;
	return CG3DFrameModule::GetDataInternal(name);
}

CG3DMatrix CG3DFrameModuleM::GetLocal()
{
	return m_Mat;
}

void* CG3DFrameModuleM::GetDataInternal(const char* name)
{
	if(!strcmp(name, "mat"))
		return &m_Mat;
	return CG3DFrameModule::GetDataInternal(name);
}