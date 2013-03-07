#ifndef G3D_GLES_SHADER_H
#define G3D_GLES_SHADER_H

#include <string>

class CG3DShaderGL : public CG3DShader {
public:
	CG3DShaderGL() : m_Binary(NULL), m_BinaryLen(0) {}

	virtual bool SetSource(Type type, const char* source);
	virtual void* MakeBinaryBuffer(Type type, int length, unsigned int format = 0);
	virtual bool ValidFormat(unsigned int format);
	virtual void AddDefine(const char* name, const char* value);
	virtual void RemoveDefine(const char* name);
	virtual void ClearDefines();
protected:
	virtual void UploadInternal();
	virtual void ReloadInternal() {}
	virtual void UnloadInternal();
	virtual void ReleaseInternal();
	virtual void ApplyInternal() {}

	std::string MakeString();
private:
	GLenum m_Type;
	std::string m_Source;
	char* m_Binary;
	int m_BinaryLen;
	GLenum m_BinaryFormat;
	std::vector<std::string> m_Defines;
public:
	GLuint m_Shader;
};

#endif