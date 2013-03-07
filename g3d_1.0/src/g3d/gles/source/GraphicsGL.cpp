#include "G3DMaths.h"

#include "GraphicsGL.h"

CG3DGraphicsGL::CG3DGraphicsGL() : m_ClearEnabled(GL_COLOR_BUFFER_BIT), m_NumAttrib(0), m_CurrEffect(NULL), m_EffectDirty(true)
{
	CG3DColourSet* colour = new CG3DColourSet(CG3DColour());
	colour->AddRef();
	m_Stacks[CG3DGlobalSet::COLOUR].m_Value = colour;

	CG3DDepthSet* depth = new CG3DDepthSet(1);
	depth->m_Test = CG3DDepthSet::ALWAYS;
	depth->AddRef();
	m_Stacks[CG3DGlobalSet::DEPTH].m_Value = depth;

	CG3DStencilSet* stencil = new CG3DStencilSet(0);
	stencil->m_DoClear = false;
	stencil->AddRef();
	m_Stacks[CG3DGlobalSet::STENCIL].m_Value = stencil;
	
	CG3DCullSet* cull = new CG3DCullSet(CG3DCullSet::CULL_NONE);
	cull->AddRef();
	m_Stacks[CG3DGlobalSet::CULL].m_Value = cull;
	
	CG3DViewportSet* view = new CG3DViewportSet(CG3DVector4(0,0,100,100));
	view->AddRef();
	m_Stacks[CG3DGlobalSet::VIEWPORT].m_Value = view;

	CG3DScissorSet* scissor = new CG3DScissorSet(CG3DVector4(0,0,100,100));
	scissor->m_Do = false;
	scissor->AddRef();
	m_Stacks[CG3DGlobalSet::SCISSOR].m_Value = scissor;

	CG3DBlendSet* blend = new CG3DBlendSet(CG3DBlendSet::OFF);
	blend->AddRef();
	m_Stacks[CG3DGlobalSet::BLEND].m_Value = blend;

	CG3DSampleCoverageSet* sample = new CG3DSampleCoverageSet(false, false);
	sample->AddRef();
	m_Stacks[CG3DGlobalSet::SAMPLE_COVERAGE].m_Value = sample;

	CG3DDitherSet* dither = new CG3DDitherSet(false);
	dither->AddRef();
	m_Stacks[CG3DGlobalSet::DITHER].m_Value = dither;

	CG3DPolygonOffsetSet* offset = new CG3DPolygonOffsetSet(false);
	offset->AddRef();
	m_Stacks[CG3DGlobalSet::POLYGON_OFFSET].m_Value = offset;
}
void CG3DGraphicsGL::Init()
{
	for (int i=0; i<CG3DGlobalSet::MAX; i++)
		ApplySet((CG3DGlobalSet::SetType)i);
}

CG3DVertexBuffer* CG3DGraphicsGL::MakeVertexBuffer()
{
	return new CG3DVertexBufferGL(this);
}

CG3DIndexBuffer* CG3DGraphicsGL::MakeIndexBuffer()
{
	return new CG3DIndexBufferGL(this);
}

CG3DShader* CG3DGraphicsGL::MakeShader()
{
	return new CG3DShaderGL();
}

CG3DEffect* CG3DGraphicsGL::MakeEffect()
{
	return new CG3DEffectGL(this);
}

CG3DTexture* CG3DGraphicsGL::MakeTexture()
{
	return new CG3DTextureGL();
}


CG3DRenderTo* CG3DGraphicsGL::MakeRenderTo()
{
	return new CG3DRenderToGL(this);
}

void CG3DGraphicsGL::ApplySet(CG3DGlobalSet::SetType type)
{
	switch (type) {
		case CG3DGlobalSet::COLOUR: {
			CG3DColourSet* set = (CG3DColourSet*)m_Stacks[type].m_Value;

			if (set->m_DoClear)
				m_ClearEnabled |= GL_COLOR_BUFFER_BIT;
			else
				m_ClearEnabled &= ~GL_COLOR_BUFFER_BIT;
			
			CG3DVector4 cfloat = set->m_Clear.Get4f();
			glClearColor(cfloat.x, cfloat.y, cfloat.z, cfloat.w);
			
			glColorMask(set->m_UseR, set->m_UseG, set->m_UseB, set->m_UseA);
		} break;
		case CG3DGlobalSet::DEPTH: {
			CG3DDepthSet* set = (CG3DDepthSet*)m_Stacks[type].m_Value;

			if (set->m_DoClear)
				m_ClearEnabled |= GL_DEPTH_BUFFER_BIT;
			else
				m_ClearEnabled &= ~GL_DEPTH_BUFFER_BIT;
			
			glClearDepthf(set->m_Clear);
			
			glDepthMask(set->m_WriteDepth);

			switch (set->m_Test) {
				case CG3DDepthSet::ALWAYS: glDisable(GL_DEPTH_TEST); break;
				case CG3DDepthSet::NEVER: glEnable(GL_DEPTH_TEST); glDepthFunc(GL_NEVER); break;
				case CG3DDepthSet::LESS: glEnable(GL_DEPTH_TEST); glDepthFunc(GL_LESS); break;
				case CG3DDepthSet::LEQUAL: glEnable(GL_DEPTH_TEST); glDepthFunc(GL_LEQUAL); break;
				case CG3DDepthSet::EQUAL: glEnable(GL_DEPTH_TEST); glDepthFunc(GL_EQUAL); break;
				case CG3DDepthSet::GREATER: glEnable(GL_DEPTH_TEST); glDepthFunc(GL_GREATER); break;
				case CG3DDepthSet::GEQUAL: glEnable(GL_DEPTH_TEST); glDepthFunc(GL_GEQUAL); break;
				case CG3DDepthSet::NOTEQUAL: glEnable(GL_DEPTH_TEST); glDepthFunc(GL_NOTEQUAL); break;
			}
		} break;
		case CG3DGlobalSet::STENCIL: {
			CG3DStencilSet* set = (CG3DStencilSet*)m_Stacks[type].m_Value;

			if (set->m_DoClear)
				m_ClearEnabled |= GL_STENCIL_BUFFER_BIT;
			else
				m_ClearEnabled &= ~GL_STENCIL_BUFFER_BIT;
			
			glClearStencil(set->m_Clear);

			ApplyStencil(set);
		} break;
		case CG3DGlobalSet::CULL: {
			CG3DCullSet* set = (CG3DCullSet*)m_Stacks[type].m_Value;

			switch (set->m_Cull) {
				case CG3DCullSet::CULL_NONE: glDisable(GL_CULL_FACE); break;
				case CG3DCullSet::CULL_FRONT: glEnable(GL_CULL_FACE); glCullFace(GL_FRONT); break;
				case CG3DCullSet::CULL_BACK: glEnable(GL_CULL_FACE); glCullFace(GL_BACK); break;
			}

			//if (set->m_FrontIsCCW != CG3DCullSet::NO)
				glFrontFace(GL_CCW);
			//else
			//	glFrontFace(GL_CW);
		} break;
		case CG3DGlobalSet::VIEWPORT: {
			CG3DViewportSet* set = (CG3DViewportSet*)m_Stacks[type].m_Value;
			
			glViewport((int)set->m_Rect.x, (int)set->m_Rect.y, (int)set->m_Rect.z, (int)set->m_Rect.w);

			glDepthRangef(set->m_NearZ, set->m_FarZ);
		} break;
		case CG3DGlobalSet::SCISSOR: {
			CG3DScissorSet* set = (CG3DScissorSet*)m_Stacks[type].m_Value;

			if (set->m_Do)
				glEnable(GL_SCISSOR_TEST);
			else
				glDisable(GL_SCISSOR_TEST);
			
			glScissor((int)set->m_Rect.x, (int)set->m_Rect.y, (int)set->m_Rect.z, (int)set->m_Rect.w);
		} break;
		case CG3DGlobalSet::BLEND: {
			CG3DBlendSet* set = (CG3DBlendSet*)m_Stacks[type].m_Value;
			CG3DVector4 col = set->m_Colour.Get4f();

			switch (set->m_Mode)
			{
				case CG3DBlendSet::OFF:
					glDisable(GL_BLEND);
					break;
				case CG3DBlendSet::COMBINED:
					glEnable(GL_BLEND);
					glBlendEquation(GetBlendFunc(set->m_Func));
					glBlendFunc(GetBlendParam(set->m_Src), GetBlendParam(set->m_Dest));
					glBlendColor(col.x, col.y, col.z, col.w);
					break;
				case CG3DBlendSet::SEPARATE:
					glEnable(GL_BLEND);
					glBlendEquationSeparate(GetBlendFunc(set->m_Func), GetBlendFunc(set->m_FuncAlpha));
					glBlendFuncSeparate(GetBlendParam(set->m_Src), GetBlendParam(set->m_Dest), GetBlendParam(set->m_SrcAlpha), GetBlendParam(set->m_DestAlpha));
					glBlendColor(col.x, col.y, col.z, col.w);
					break;
			}
		} break;
		case CG3DGlobalSet::SAMPLE_COVERAGE: {
			CG3DSampleCoverageSet* set = (CG3DSampleCoverageSet*)m_Stacks[type].m_Value;

			if (set->m_Coverage)
				glEnable(GL_SAMPLE_COVERAGE);
			else
				glDisable(GL_SAMPLE_COVERAGE);
			
			if (set->m_AlphaCoverage)
				glEnable(GL_SAMPLE_ALPHA_TO_COVERAGE);
			else
				glDisable(GL_SAMPLE_ALPHA_TO_COVERAGE);
			glSampleCoverage(set->m_Value, set->m_Invert);
		} break;
		case CG3DGlobalSet::DITHER: {
			CG3DDitherSet* set = (CG3DDitherSet*)m_Stacks[type].m_Value;

			if (set->m_Dither)
				glEnable(GL_DITHER);
			else
				glDisable(GL_DITHER);
		} break;
		case CG3DGlobalSet::POLYGON_OFFSET: {
			CG3DPolygonOffsetSet* set = (CG3DPolygonOffsetSet*)m_Stacks[type].m_Value;

			if (set->m_PolygonOffset)
				glEnable(GL_POLYGON_OFFSET_FILL);
			else
				glDisable(GL_POLYGON_OFFSET_FILL);

			glPolygonOffset(set->m_Factor, set->m_Units);
		} break;
	}
}
GLenum CG3DGraphicsGL::GetBlendFunc(CG3DBlendSet::Func func)
{
	switch (func) {
		case CG3DBlendSet::FUNC_SUBTRACT: return GL_FUNC_SUBTRACT;
		case CG3DBlendSet::FUNC_REVERSE_SUBTRACT: return GL_FUNC_REVERSE_SUBTRACT;
	}
	return GL_FUNC_ADD;
}
GLenum CG3DGraphicsGL::GetBlendParam(CG3DBlendSet::Param param)
{
	switch (param) {
		case CG3DBlendSet::ONE: return GL_ONE;
		case CG3DBlendSet::SRC_COLOR: return GL_SRC_COLOR;
		case CG3DBlendSet::ONE_MINUS_SRC_COLOR: return GL_ONE_MINUS_SRC_COLOR;
		case CG3DBlendSet::DST_COLOR: return GL_DST_COLOR;
		case CG3DBlendSet::ONE_MINUS_DST_COLOR: return GL_ONE_MINUS_DST_COLOR;
		case CG3DBlendSet::SRC_ALPHA: return GL_SRC_ALPHA;
		case CG3DBlendSet::ONE_MINUS_SRC_ALPHA: return GL_ONE_MINUS_SRC_ALPHA;
		case CG3DBlendSet::DST_ALPHA: return GL_DST_ALPHA;
		case CG3DBlendSet::ONE_MINUS_DST_ALPHA: return GL_ONE_MINUS_DST_ALPHA;
		case CG3DBlendSet::CONSTANT_COLOR: return GL_CONSTANT_COLOR;
		case CG3DBlendSet::ONE_MINUS_CONSTANT_COLOR: return GL_ONE_MINUS_CONSTANT_COLOR;
		case CG3DBlendSet::CONSTANT_ALPHA: return GL_CONSTANT_ALPHA;
		case CG3DBlendSet::ONE_MINUS_CONSTANT_ALPHA: return GL_ONE_MINUS_CONSTANT_ALPHA;
		case CG3DBlendSet::SRC_ALPHA_SATURATE: return GL_SRC_ALPHA_SATURATE;
	}
	return GL_ZERO;
}
void CG3DGraphicsGL::ApplyStencil(CG3DStencilSet* set)
{
	GLenum face;
	switch (set->m_Face) {
		case CG3DStencilSet::FRONT: face = GL_FRONT; break;
		case CG3DStencilSet::BACK: face = GL_BACK; break;
		case CG3DStencilSet::FRONT_AND_BACK: face = GL_FRONT_AND_BACK; break;
	}

	glStencilMaskSeparate(face, set->m_Mask);
	
	switch (set->m_Func) {
		case CG3DStencilSet::ALWAYS: glDisable(GL_STENCIL_TEST); break;
		case CG3DStencilSet::NEVER: glEnable(GL_STENCIL_TEST); glStencilFuncSeparate(face, GL_NEVER, set->m_FuncRef, set->m_FuncMask); break;
		case CG3DStencilSet::LESS: glEnable(GL_STENCIL_TEST); glStencilFuncSeparate(face, GL_LESS, set->m_FuncRef, set->m_FuncMask); break;
		case CG3DStencilSet::LEQUAL: glEnable(GL_STENCIL_TEST); glStencilFuncSeparate(face, GL_LEQUAL, set->m_FuncRef, set->m_FuncMask); break;
		case CG3DStencilSet::EQUAL: glEnable(GL_STENCIL_TEST); glStencilFuncSeparate(face, GL_EQUAL, set->m_FuncRef, set->m_FuncMask); break;
		case CG3DStencilSet::GREATER: glEnable(GL_STENCIL_TEST); glStencilFuncSeparate(face, GL_GREATER, set->m_FuncRef, set->m_FuncMask); break;
		case CG3DStencilSet::GEQUAL: glEnable(GL_STENCIL_TEST); glStencilFuncSeparate(face, GL_GEQUAL, set->m_FuncRef, set->m_FuncMask); break;
		case CG3DStencilSet::NOTEQUAL: glEnable(GL_STENCIL_TEST); glStencilFuncSeparate(face, GL_NOTEQUAL, set->m_FuncRef, set->m_FuncMask); break;
	}
	glStencilOpSeparate(face, GetOp(set->m_StencilFail), GetOp(set->m_DepthFail), GetOp(set->m_DepthPass));

	if (set->m_OtherFace != NULL)
		ApplyStencil(set->m_OtherFace);
}
GLenum CG3DGraphicsGL::GetOp(CG3DStencilSet::Op op)
{
	switch (op) {
		case CG3DStencilSet::ZERO: return GL_ZERO;
		case CG3DStencilSet::REPLACE: return GL_REPLACE;
		case CG3DStencilSet::INCR: return GL_INCR;
		case CG3DStencilSet::DECR: return GL_DECR;
		case CG3DStencilSet::INVERT: return GL_INVERT;
		case CG3DStencilSet::INCR_WRAP: return GL_INCR_WRAP;
		case CG3DStencilSet::DECR_WRAP: return GL_DECR_WRAP;
	}
	return GL_KEEP;
}
void CG3DGraphicsGL::Clear()
{
	glClear(m_ClearEnabled);
}

void CG3DGraphicsGL::SetEffect(CG3DEffectGL* effect)
{
	if (m_CurrEffect != effect)
		m_EffectDirty = true;
	m_CurrEffect = effect;
}
void CG3DGraphicsGL::ApplyEffect()
{
	if (m_CurrEffect == NULL)
		return;

	m_CurrEffect->DoApply(m_Parameters, m_EffectDirty);
	m_EffectDirty = false;
}
void CG3DGraphicsGL::SetParameter(CG3DParameterGL* param)
{
	if (m_Parameters.find(param->m_Name) == m_Parameters.end() || m_Parameters[param->m_Name] != param)
		param->m_Dirty = true;
	m_Parameters[param->m_Name] = param;
}
void CG3DGraphicsGL::RemoveParameter(CG3DParameterGL* param)
{
	if (m_Parameters.find(param->m_Name) != m_Parameters.end() && m_Parameters[param->m_Name] == param)
		m_Parameters.erase(param->m_Name);
}

CG3DGraphics* MakeG3DGraphicsGLES()
{
	return new CG3DGraphicsGL();
}