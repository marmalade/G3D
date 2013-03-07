#ifndef G3D_GLES_TEXTURE_H
#define G3D_GLES_TEXTURE_H

class CG3DTextureGL : public CG3DTexture {
public:
	class Image {
	public:
		GLenum m_Target;
		int m_Level;
		int m_Format;
		GLsizei m_Width;
		GLsizei m_Height;
		GLenum m_Type;
		void* m_Data;
		int m_DataSize;
	public:
		int SetFormat(ImageFormat format); //returns stride
	};
public:
	CG3DTextureGL() : m_Texture(0), m_Target(GL_TEXTURE_2D), m_Generate(false), m_Mag(GL_LINEAR), m_Min(GL_LINEAR), m_S(GL_REPEAT), m_T(GL_REPEAT) { }
	
	virtual void Setup(bool cube, bool generateMipmaps, MipMap filter, Wrap s, Wrap t);
	virtual void* AddImage(Target target,ImageFormat format,int width, int height, int dataSize, int mipmap);
	virtual bool ValidFormat(ImageFormat format);
protected:
	virtual void UploadInternal();
	virtual void ReloadInternal();
	virtual void UnloadInternal();
	virtual void ApplyInternal();
public:
	virtual void ReleaseInternal();
public:
    GLuint m_Texture;
private:
	GLenum m_Target;
	bool m_Generate;
	GLenum m_Mag, m_Min, m_S, m_T;
	std::vector<Image> m_Images;
};

#endif