#ifndef G3D_CORE_RENDER_TO_H
#define G3D_CORE_RENDER_TO_H

// A collection of textures and buffers to render to

#include "ResourceList.h"
#include "Texture.h"

class CG3DRenderTo : public CG3DResourceList<CG3DTexture> {
public:
	enum BufferType {
		COLOUR,
		DEPTH,
		STENCIL,
	};
	enum BufferFormat {
		FORMAT_NONE,
		FORMAT_DEPTH_16,
		FORMAT_RGBA_4444,
		FORMAT_RGBA_5551,
		FORMAT_RGB_565,
		FORMAT_STENCIL_8,
	};
public:
	// add a render buffer to the frame buffer
	virtual void SetBuffer(BufferType type, BufferFormat format, int width, int height) = 0;
	// add a texture image to the frame buffer
	virtual void SetTexture(BufferType type, CG3DTexture* texture, CG3DTexture::Target target = CG3DTexture::TARGET_2D, int mipmap = 0) = 0;
	// finished applying this render to buffer
	virtual void DoneApply() = 0;

	// add a global state to be set during this material
	virtual void AddSet(CG3DGlobalSet::SetType type, CG3DGlobalSet* set) = 0;
	// remove a global state
	virtual void RemoveSet(CG3DGlobalSet::SetType type) = 0;
protected:
	virtual void Add(CG3DTexture* item) { CG3DResourceList<CG3DTexture>::Add(item); }
	virtual void Remove(CG3DTexture* item) { CG3DResourceList<CG3DTexture>::Remove(item); }
};

#endif