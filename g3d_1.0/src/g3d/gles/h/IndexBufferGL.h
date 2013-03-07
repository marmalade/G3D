#ifndef G3D_GLES_INDEX_BUFFER_H
#define G3D_GLES_INDEX_BUFFER_H

class CG3DIndexBufferGL : public CG3DIndexBuffer {
public:
	CG3DIndexBufferGL(CG3DGraphicsGL* graphics) : m_Graphics(graphics), m_Buffer(0) { }
	
	virtual void Setup(Type type);
protected:
	virtual void UploadInternal();
	virtual void ReloadInternal();
	virtual void UnloadInternal();
	virtual void ApplyInternal();
private:
	CG3DGraphicsGL* m_Graphics;
    GLuint m_Buffer;
	GLenum m_Mode;
	GLenum m_Type;
};

#endif