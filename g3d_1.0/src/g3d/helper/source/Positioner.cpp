#include "Positioner.h"

char CG3DPositioner::s_ConstantBuffer[64] = "PositionBuffer";
char CG3DPositioner::s_Parameter[64] = "World";

void CG3DPositioner::Setup(const char* bufferName, const char* paramName)
{
	strncpy(s_ConstantBuffer, bufferName, 64);
	strncpy(s_Parameter, paramName, 64);
}

char CG3DCamera::s_ConstantBuffer[64] = "CameraBuffer";
char CG3DCamera::s_ParamView[64] = "View";
char CG3DCamera::s_ParamProj[64] = "Proj";

void CG3DCamera::Setup(const char* bufferName, const char* viewName, const char* projName)
{
	strncpy(s_ConstantBuffer, bufferName, 64);
	strncpy(s_ParamView, viewName, 64);
	strncpy(s_ParamProj, projName, 64);
}