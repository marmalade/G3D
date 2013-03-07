#ifndef G3D_GLES_EFFECT_H
#define G3D_GLES_EFFECT_H

class CG3DParameterGL {
public:
	CG3DParameterGL(const char* name) : m_Name(name), m_Dirty(false) {}
	virtual void Set(const float* value) {}
	virtual void Set(const int* value) {}
	virtual void Set(const CG3DTexture* value) {}
	virtual void Prepare(CG3DGraphicsGL* graphics) {}
	virtual void Apply(int slot, int& numTextures) {}
public:
	std::string m_Name;
	bool m_Dirty;
};

class CG3DEffectGL : public CG3DEffect {
public:
	struct Parameter {
		char m_Name[64];
		GLint m_Location;
		ParameterType m_Type;
		unsigned int m_Offset;
		int m_Size;
		int m_Index;
	};
public:
	CG3DEffectGL(CG3DGraphicsGL* graphics) : m_Program(0xffffffff), m_Graphics(graphics), m_DoLink(true) {}
	virtual CG3DMaterial* MakeMaterial();
	virtual CG3DMaterial* MakeSubMaterial(const char* bufferName);
	virtual CG3DShader* MakeShader();
	
	virtual bool ValidProgramFormat(unsigned int format);
	virtual bool SetProgramBinary(void* data, int size, unsigned int format);

	virtual void AddParameter(const char* name, ParameterType type, const char* auxName);
	int GetVertexSlot(const char* name);

	Parameter* FindParameter(const char* name);
	void DoApply(std::map<std::string, CG3DParameterGL*>& params, bool dirty);
protected:
	virtual void AddInternal(CG3DShader* item);
	virtual void RemoveInternal(CG3DShader* item);

	virtual void UploadInternal();
	virtual void ReloadInternal() {}
	virtual void UnloadInternal();
	virtual void ApplyInternal();
	virtual void ReleaseInternal() {}
public:
	std::vector<Parameter> m_Parameters;
	bool m_DoLink;

	GLuint m_Program;
	CG3DGraphicsGL* m_Graphics;
};

#endif