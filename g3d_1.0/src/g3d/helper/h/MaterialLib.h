#ifndef G3D_HELPER_MATERIAL_LIB_H
#define G3D_HELPER_MATERIAL_LIB_H

#include "G3DCore.h"
#include "EffectLib.h"

//load a text based material file (and texture file)

class CG3DMaterialLib : public CG3DRefCount
{
public:
	//point to the effect lib with the effects referenced in
	CG3DMaterialLib(CG3DEffectLib* effect) : m_Graphics(effect->m_Graphics.get()), m_Effect(effect) {}
	void LoadText(const char* filename);			//load the file
	CG3DMaterial* GetMaterial(const char* name);	//get one of the materials loaded
	virtual void Release();
	CG3DTexture* LoadTexture(const char* filename);	//load a text based texture file
private:
	bool GetBool(const char* str);
	CG3DDepthSet::Test GetTest(const char* str);
	CG3DStencilSet::Face GetFace(const char* str);
	CG3DStencilSet::Op GetOp(const char* str);
	CG3DBlendSet::Func GetFunc(const char* str);
	CG3DBlendSet::Param GetParam(const char* str);
	CG3DCullSet::Tripple GetTripple(const char* str);

	CG3DTexture::MipMap GetFilter(const char* str);
	CG3DTexture::Wrap GetWrap(const char* str);
	CG3DTexture::Target GetTarget(const char* str);
	ImageFormat::Depth GetDepth(const char* str);
	ImageFormat::Type GetType(const char* str);
	ImageFormat::Layout GetLayout(const char* str);
	ImageFormat::Flag GetFlag(const char* str);
	
	void LoadFileBinary(FILE* fp, unsigned char* data, unsigned int len);
private:
	CG3DPtr<CG3DGraphics> m_Graphics;
	CG3DPtr<CG3DEffectLib> m_Effect;
	std::map<std::string, CG3DMaterial*> m_Materials;
};

#endif