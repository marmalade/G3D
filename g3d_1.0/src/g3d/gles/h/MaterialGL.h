#ifndef G3D_GLES_MATERIAL_H
#define G3D_GLES_MATERIAL_H

class CG3DTextureGL;

class CG3DParamValueGL : public CG3DParameterGL {
public:
	CG3DParamValueGL(const char* name, CG3DEffect::ParameterType type, int size) : 
	  CG3DParameterGL(name), m_Type(type), m_Size(size) {}
	virtual void Prepare(CG3DGraphicsGL* graphics);
public:
	CG3DEffect::ParameterType m_Type;
	int m_Size;
};

class CG3DParamFValueGL : public CG3DParamValueGL {
public:
	CG3DParamFValueGL(const char* name, CG3DEffect::ParameterType type, int size) : 
	  CG3DParamValueGL(name, type, size), m_Data(new float[size]) {}
	~CG3DParamFValueGL() { delete m_Data; }
	virtual void Set(const float* value);
	virtual void Apply(int slot, int& numTextures);
public:
	float* m_Data;
};

class CG3DParamMValueGL : public CG3DParamFValueGL {
public:
	CG3DParamMValueGL(const char* name, CG3DEffect::ParameterType type, int size) : 
	  CG3DParamFValueGL(name, type, size) {}
	virtual void Apply(int slot, int& numTextures);
};

class CG3DParamIValueGL : public CG3DParamValueGL {
public:
	CG3DParamIValueGL(const char* name, CG3DEffect::ParameterType type, int size) : 
	  CG3DParamValueGL(name, type, size), m_Data(new int[size]) {}
	~CG3DParamIValueGL() { delete m_Data; }
	virtual void Set(const int* value);
	virtual void Apply(int slot, int& numTextures);
public:
	int* m_Data;
};

class CG3DParamTValueGL : public CG3DParameterGL {
public:
	CG3DParamTValueGL(const char* name) :  CG3DParameterGL(name) {}
	virtual void Prepare(CG3DGraphicsGL* graphics);
	virtual void Set(const CG3DTexture* value);
	virtual void Apply(int slot, int& numTextures);
public:
	CG3DTextureGL* m_Texture;
};

class CG3DMaterialGL : public CG3DMaterial {
	typedef std::map<CG3DGlobalSet::SetType, CG3DGlobalSet*> SetMap;
public:
	CG3DMaterialGL(CG3DEffectGL* effect, bool fullMaterial);

	virtual void SetParameter(const char* name, const float* values);
	virtual void SetParameter(const char* name, const int* values);
	virtual void SetTexture(const char* name, CG3DTexture* texture);

	virtual void Apply();
	virtual void DoneApply();
	
	virtual CG3DEffect* GetEffect() { return m_Effect; }

	virtual void AddSet(CG3DGlobalSet::SetType type, CG3DGlobalSet* set);
	virtual void RemoveSet(CG3DGlobalSet::SetType type);
protected:
	virtual void UploadInternal() {}
	virtual void ReloadInternal() {}
	virtual void UnloadInternal();
	virtual void ReleaseInternal() {}
	virtual void ApplyInternal();
	virtual void AddInternal(CG3DResource* item) {}
	virtual void RemoveInternal(CG3DResource* item) {}

	CG3DParameterGL* GetParameter(const char* name);
private:
	SetMap m_Sets;
	std::vector<CG3DParameterGL*> m_Params;
	CG3DEffectGL* m_Effect;
	bool m_FullMaterial;
};

#endif