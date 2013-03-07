#ifndef G3D_GLES_RENDER_TO_H
#define G3D_GLES_RENDER_TO_H

class CG3DRenderToGL : public CG3DRenderTo {
	typedef std::map<CG3DGlobalSet::SetType, CG3DGlobalSet*> SetMap;
	class Buffer {
	public:
		GLenum m_Format;
		int m_Width;
		int m_Height;

		CG3DTextureGL* m_Texture;
		GLenum m_Target;
		int m_Mipmap;

		GLuint m_Buffer;

		Buffer() : m_Format(FORMAT_NONE), m_Texture(NULL) {}
	};
public:
	CG3DRenderToGL(CG3DGraphicsGL* graphics) : m_Graphics(graphics) {}
	virtual void SetBuffer(BufferType type, BufferFormat format, int width, int height);
	virtual void SetTexture(BufferType type, CG3DTexture* texture, CG3DTexture::Target target = CG3DTexture::TARGET_2D, int mipmap = 0);
	
	virtual void Apply()
	{
		CG3DResource::Apply();
	}
	virtual void DoneApply();

	virtual void AddSet(CG3DGlobalSet::SetType type, CG3DGlobalSet* set);
	virtual void RemoveSet(CG3DGlobalSet::SetType type);
protected:
	virtual void UploadInternal();
	virtual void ReloadInternal() {}
	virtual void UnloadInternal();
	virtual void ReleaseInternal() {}
	virtual void ApplyInternal();
	virtual void AddInternal(CG3DTexture* item) {}
	virtual void RemoveInternal(CG3DTexture* item) {}

private:
	void Release(BufferType type);

private:
	GLuint m_FrameBuffer;
	GLuint m_PrevFrameBuffer;
	Buffer m_Buffers[3];
	SetMap m_Sets;
	CG3DGraphicsGL* m_Graphics;
};

#endif