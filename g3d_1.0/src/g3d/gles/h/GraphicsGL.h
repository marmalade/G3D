#ifndef G3D_GLES_GRAPHICSGL_H
#define G3D_GLES_GRAPHICSGL_H

#include "G3DCore.h"
#include "GraphicsCommon.h"

#ifdef G3D_USE_IWGL
#include <IwGL.h>
#else
#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#endif

class CG3DGraphicsGL;

#include "VertexBufferGL.h"
#include "IndexBufferGL.h"
#include "ShaderGL.h"
#include "EffectGL.h"
#include "MaterialGL.h"
#include "TextureGL.h"
#include "RenderToGL.h"

class CG3DGraphicsGL : public CG3DGraphicsCommon
{
public:
	CG3DGraphicsGL();
	virtual void Init();
	virtual bool IsDX() { return false; }
	
	virtual CG3DVertexBuffer* MakeVertexBuffer();
	virtual CG3DIndexBuffer* MakeIndexBuffer();
	virtual CG3DShader* MakeShader();
	virtual CG3DEffect* MakeEffect();
	virtual CG3DTexture* MakeTexture();
	virtual CG3DRenderTo* MakeRenderTo();
	
	virtual void ApplySet(CG3DGlobalSet::SetType type);

	virtual void Clear();
public:
	void SetEffect(CG3DEffectGL* effect);
	void ApplyEffect();
	void SetParameter(CG3DParameterGL* param);
	void RemoveParameter(CG3DParameterGL* param);
private:
	void ApplyStencil(CG3DStencilSet* set);
	GLenum GetOp(CG3DStencilSet::Op op);
	GLenum GetBlendFunc(CG3DBlendSet::Func func);
	GLenum GetBlendParam(CG3DBlendSet::Param param);
private:
	GLbitfield m_ClearEnabled;

	CG3DEffectGL* m_CurrEffect;
	bool m_EffectDirty;
	std::map<std::string, CG3DParameterGL*> m_Parameters;
public:
	unsigned int m_NumAttrib;
};

#endif