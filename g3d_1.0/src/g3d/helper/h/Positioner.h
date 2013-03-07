#ifndef G3D_HELPER_POSITIONER_H
#define G3D_HELPER_POSITIONER_H

#include "G3DCore.h"

/*
To use these classes, all your glsl vertex shaders must have the following:
    uniform mat4 World;
    uniform mat4 View;
    uniform mat4 Proj;
and your hlsl vertex shaders:
    cbuffer PositionBuffer {
      matrix World;
    }
    cbuffer CameraBuffer {
      matrix View;
      matrix Proj;
    }
you can change the names uses by calling: (change the strings you use the ones you use)
	CG3DPositioner::Setup("PositionBuffer", "World");
	CG3DPositioner::Setup("CameraBuffer", "View", "Proj");
Note: there is a limit of 64 characters on these names
*/

//base class for the other classes, handles the material
class CG3DPositionerBase : public CG3DResource
{
public:
	CG3DPositionerBase(CG3DMaterial* material) : m_Material(material) { }
protected:
	virtual void UploadInternal() { m_Material->Upload(); }
	virtual void ReloadInternal() { m_Material->Reload(); }
	virtual void UnloadInternal() { m_Material->Unload(); }
	virtual void ApplyInternal() { m_Material->Apply(); }
public:
	virtual void ReleaseInternal() { m_Material->ReleaseInternal(); }
protected:
	CG3DPtr<CG3DMaterial> m_Material;
};

//encapsulate the world matrix
class CG3DPositioner : public CG3DPositionerBase
{
public:
	CG3DPositioner(CG3DEffect* effect) : CG3DPositionerBase(effect->MakeSubMaterial(s_ConstantBuffer)) { }
	void Set(CG3DMatrix& value) { m_Material->SetParameter(s_Parameter, value); }

	static void Setup(const char* bufferName, const char* paramName);
private:
	static char s_ConstantBuffer[64];
	static char s_Parameter[64];
};

//encapsulate the view and projection matrices
class CG3DCamera : public CG3DPositionerBase
{
public:
	CG3DCamera(CG3DEffect* effect) : CG3DPositionerBase(effect->MakeSubMaterial(s_ConstantBuffer)) { }
	void SetView(CG3DMatrix& value) { m_Material->SetParameter(s_ParamView, value); }
	void SetProj(CG3DMatrix& value) { m_Material->SetParameter(s_ParamProj, value); }
	
	static void Setup(const char* bufferName, const char* viewName, const char* projName);
private:
	static char s_ConstantBuffer[64];
	static char s_ParamView[64];
	static char s_ParamProj[64];
};

#endif