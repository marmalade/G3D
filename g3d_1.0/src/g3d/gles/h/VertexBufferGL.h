#ifndef G3D_GLES_VERTEX_BUFFER_H
#define G3D_GLES_VERTEX_BUFFER_H

#include <string>

class CG3DVertexBufferGL : public CG3DVertexBuffer {
	struct Element {
		std::string m_Name;
		GLuint m_Offset;
		GLint m_Size;
		GLenum m_Type;
		GLboolean m_Normalized;
		GLvoid* m_Pointer;
	};
public:
	CG3DVertexBufferGL(CG3DGraphicsGL* graphics) : m_Buffer(0), m_Graphics(graphics) { }

	virtual void AddElement(const char* name,ElementType type, int offset);
	virtual void PatchUp(CG3DEffect* effect, CG3DVertexBuffer** auxBufferList = NULL);
protected:
	virtual void UploadInternal();
	virtual void ReloadInternal();
	virtual void UnloadInternal();
	virtual void ApplyInternal();
private:
    GLuint m_Buffer;
	std::vector<Element> m_Elements;
	CG3DGraphicsGL* m_Graphics;
};

#endif