#ifndef G3D_HELPER_EFFECT_LIB_H
#define G3D_HELPER_EFFECT_LIB_H

#include "G3DCore.h"
#include <map>
#include <string>

//load an text based effects file

class CG3DEffectLib : public CG3DRefCount
{
public:
	CG3DEffectLib(CG3DGraphics* graphics);
	void LoadText(const char* filename);	//load the file
	CG3DMaterial* MakeMaterial(const char* name);	//make a material from the named effect
	CG3DMaterial* MakeSubMaterial(const char* name, const char* buffer);	//make a sub material
	virtual void Release();
private:
	void LoadFile(const char* filename);
	void LoadFileBinary(FILE* fp, unsigned int length);
	void LoadFileText(FILE* fp, unsigned int length);
	bool LoadProgram(CG3DEffect* effect, unsigned int format);
	bool LoadShader(CG3DShader* shader, CG3DShader::Type type);
	bool LoadShader(CG3DShader* shader, const char* filename, const char* type, const char* entry);
	bool LoadBinaryShader(CG3DShader* shader, CG3DShader::Type type, unsigned int format);
public:
	CG3DPtr<CG3DGraphics> m_Graphics;
private:
	std::map<std::string, CG3DEffect*> m_Effects;
	unsigned char* m_Data;
	unsigned int m_Length;
};

#endif