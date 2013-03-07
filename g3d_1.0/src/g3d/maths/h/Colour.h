#ifndef G3D_MATHS_COLOUR_H
#define G3D_MATHS_COLOUR_H

#include "Maths.h"

//encapsulates colour as 4 bytes, get and set as 3/4 bytes or floats and as an unsigned int
//also set as web colour definition

class CG3DColour {
public:
	CG3DColour() { Set(CG3DColour::g_Black); }
	CG3DColour(unsigned int colour) { Set(colour); }
	CG3DColour(unsigned char r, unsigned char g, unsigned char b, unsigned char a = 255) { Set(r,g,b,a); }
	CG3DColour(CG3DVector4 colour) { Set(colour); }
	CG3DColour(CG3DVector3 colour) { Set(colour); }
	CG3DColour(const char* str) { Set(str); }

	void Set(unsigned int colour) { m_Colour = colour; }
	void Set(unsigned char r, unsigned char g, unsigned char b, unsigned char a = 255) { m_R = r; m_G = g; m_B = b; m_A = a; }
	void Set(CG3DVector4 colour);
	void Set(CG3DVector3 colour);
	void Set(const char* str);
	void Set(CG3DColour col) { m_Colour = col.m_Colour; }
	
	CG3DVector3 Get3f() { return CG3DVector3(m_R / 255.0f, m_G / 255.0f, m_B / 255.0f); }
	CG3DVector4 Get4f() { return CG3DVector4(m_R / 255.0f, m_G / 255.0f, m_B / 255.0f, m_A / 255.0f); }
public:
	union {
		unsigned int m_Colour;
		struct {
			unsigned char m_R;
			unsigned char m_G;
			unsigned char m_B;
			unsigned char m_A;
		};
	};
public:
	static CG3DColour g_Black;
	static CG3DColour g_White;
};

#endif