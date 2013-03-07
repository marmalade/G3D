#ifndef G3D_CORE_GLOBAL_SET_H
#define G3D_CORE_GLOBAL_SET_H

// Global Sets encapsulate sub sets of the graphics system state
// use the graphics class to push, pop, set or apply these sets

// base class for global sets
class CG3DGlobalSet : public CG3DRefCount {
public:
	enum SetType {
		COLOUR,
		DEPTH,
		STENCIL,
		CULL,
		VIEWPORT,
		SCISSOR,
		BLEND,
		SAMPLE_COVERAGE,
		DITHER,
		POLYGON_OFFSET,

		MAX
	};
};

// encapsulates clear colour, and masking of colour channels
// if no set is applied Colour is black
class CG3DColourSet : public CG3DGlobalSet {
public:
	CG3DColourSet(CG3DColour clear) : m_Clear(clear), m_DoClear(true), m_UseR(true), m_UseG(true), m_UseB(true), m_UseA(true) {}
public:
	CG3DColour m_Clear;						// colour to clear the screen on Clear call
	bool m_DoClear;							// do we clear the colour?
	bool m_UseR, m_UseG, m_UseB, m_UseA;	// do we mask colour channels?
};

// encapulates depth clear, masking and testing
// if no set is applied Test is ALWAYS and Clear is 1
class CG3DDepthSet : public CG3DGlobalSet {
public:
	enum Test {
		NEVER, ALWAYS, LESS, LEQUAL, EQUAL, GREATER, GEQUAL, NOTEQUAL
	};
public:
	CG3DDepthSet(float clear) : m_Clear(clear), m_DoClear(true), m_WriteDepth(true), m_Test(LESS) {}
public:
	float m_Clear;		// value to clear depth buffer with on Clear
	bool m_DoClear;		// do we clear the depth buffer?
	bool m_WriteDepth;	// do we write to the depth buffer?
	Test m_Test;		// test the use when writing to the depth buffer
};

// encapsulates stencil clear, masking and testing
// to set both front and back faces to the same values specify Face as FRONT_AND_BACK
// to set different values for front and back, create a set with Face as FRONT and another set with Face as BACK
//  and set OtherFace on the first set with the second set
// if no set is applied DoClear is false
class CG3DStencilSet : public CG3DGlobalSet {
public:
	enum Face {
		FRONT, BACK, FRONT_AND_BACK
	};
	enum Test {
		NEVER, ALWAYS, LESS, LEQUAL, EQUAL, GREATER, GEQUAL, NOTEQUAL
	};
	enum Op {
		KEEP, ZERO, REPLACE, INCR, DECR, INVERT, INCR_WRAP, DECR_WRAP
	};
public:
	CG3DStencilSet(int clear) : m_Clear(clear), m_DoClear(true), m_Face(FRONT_AND_BACK), m_Mask(0xffffffff), 
		m_Func(ALWAYS), m_FuncRef(0), m_FuncMask(0xffffffff), m_StencilFail(KEEP), m_DepthFail(KEEP), m_DepthPass(KEEP), m_OtherFace(NULL) {}
public:
	int m_Clear;			// clear stencil value
	bool m_DoClear;			// do we clear the stencil

	Face m_Face;			// do we affect front or back faces or both
	unsigned int m_Mask;	// draw to stencil mask
	Test m_Func;			// draw to stencil function
	int m_FuncRef;			// draw to stencil function reference value
	unsigned int m_FuncMask;// draw to stencil function mask value
	Op m_StencilFail;		// stencil fail operation
	Op m_DepthFail;			// stencil pass but depth fail operation
	Op m_DepthPass;			// depth pass operation

	CG3DStencilSet* m_OtherFace;
};

// encapsulate culling
// if no set is applied Cull is CULL_NONE
class CG3DCullSet : public CG3DGlobalSet {
public:
	enum Cull {
		CULL_NONE, CULL_FRONT, CULL_BACK
	};
	enum Tripple {
		YES,
		NO,
		DEFAULT
	};
public:
	CG3DCullSet(Cull cull) : m_Cull(cull), m_FrontIsCCW(DEFAULT) {}
public:
	Cull m_Cull;		// do we cull front and/or back faces?
	Tripple m_FrontIsCCW;	// is front a counter clockwise polygon? (default is yes on gl and no on dx)
};

// encapsulate viewport and z depth range
// if no set is applied Rect is 0, 0, 100, 100
class CG3DViewportSet : public CG3DGlobalSet {
public:
	CG3DViewportSet(CG3DVector4 rect) : m_Rect(rect), m_NearZ(0), m_FarZ(1) {}
public:
	CG3DVector4 m_Rect;	//size of the viewport
	float m_NearZ;		//near z range (in the range 0-1)
	float m_FarZ;		//far z range (in the range 0-1)
};

// encapsulate scissor
// if no set is applied Do is false
class CG3DScissorSet : public CG3DGlobalSet {
public:
	CG3DScissorSet(CG3DVector4 rect) : m_Rect(rect), m_Do(true) {}
public:
	CG3DVector4 m_Rect;	// scissor rectangle
	bool m_Do;			// do we do scissor?
};

// encapsulate blend mode
// if Mode is SEPARATE alpha blending is controlled separately by FuncAlpha, SrcAlpha and DestAlpha
// if no set is applied Mode is OFF
class CG3DBlendSet : public CG3DGlobalSet {
public:
	enum Mode {
		OFF, COMBINED, SEPARATE
	};
	enum Func {
		FUNC_ADD, FUNC_SUBTRACT, FUNC_REVERSE_SUBTRACT
	};
	enum Param {
		ZERO, ONE, SRC_COLOR, ONE_MINUS_SRC_COLOR, DST_COLOR, ONE_MINUS_DST_COLOR, SRC_ALPHA, ONE_MINUS_SRC_ALPHA,
		DST_ALPHA, ONE_MINUS_DST_ALPHA, CONSTANT_COLOR, ONE_MINUS_CONSTANT_COLOR, CONSTANT_ALPHA, ONE_MINUS_CONSTANT_ALPHA,
		SRC_ALPHA_SATURATE
	};
public:
	CG3DBlendSet(Mode mode) : m_Mode(mode), m_Func(FUNC_ADD), m_FuncAlpha(FUNC_ADD), m_Src(ONE), m_SrcAlpha(ONE), m_Dest(ZERO), m_DestAlpha(ZERO) {}
public:
	Mode m_Mode;		// if OFF no blending is applied
	Func m_Func;		// blend function
	Func m_FuncAlpha;	// blend function for alpha (if mode is SEPARATE)
	Param m_Src;		// blend source
	Param m_SrcAlpha;	// blend source for alpha (if mode is SEPARATE)
	Param m_Dest;		// blend destination
	Param m_DestAlpha;	// blend destination for alpha (if mode is SEPARATE)
	CG3DColour m_Colour;// blend constant colour
};

// encapsulates sample coverage
// if no set is applied Coverage and AlphaCoverage are false
class CG3DSampleCoverageSet : public CG3DGlobalSet {
public:
	CG3DSampleCoverageSet(bool coverage, bool alphaCoverage) :
	  m_Coverage(coverage), m_AlphaCoverage(alphaCoverage), m_Value(1), m_Invert(false) {}
public:
	bool m_Coverage;		// do we use sample coverage?
	bool m_AlphaCoverage;	// do we use alpha sample coverage?
	float m_Value;			// sample coverage value
	bool m_Invert;			// do we invert sample coverage?
};

// encapsulste dither
// if no set is applied Dither is false
class CG3DDitherSet : public CG3DGlobalSet {
public:
	CG3DDitherSet(bool dither) : m_Dither(dither) {}
public:
	bool m_Dither;	// do we dither?
};

// encapsulate polygon offset
// if no set is applied PolygonOffset is false
class CG3DPolygonOffsetSet : public CG3DGlobalSet {
public:
	CG3DPolygonOffsetSet(bool polygonOffset) : m_PolygonOffset(polygonOffset), m_Factor(0), m_Units(0) {}
public:
	bool m_PolygonOffset;	// do we offset polygons?
	float m_Factor;			// polygon offset factor
	float m_Units;			// polygon offset units

};

#endif