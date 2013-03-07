#ifndef G3D_HELPER_BINARY_LOADERS_H
#define G3D_HELPER_BINARY_LOADERS_H

#include "Binary.h"

//load texture file sections
class TextureHandler : public CG3DBinaryHandler {
public:
	virtual CG3DRefCount* Load(BinaryFP* fp);
private:
	CG3DTexture::Target GetTarget(const char* str);
};

//load effect file sections
class EffectHandler : public CG3DBinaryHandler {
    enum Flags {
        Program=0,
        Shader=1,

        Vertex=2,
        Pixel=4,

        Binary=0,
        Source=8,

        HLSL=0,
        GLSL=16,
    };
public:
	virtual CG3DRefCount* Load(BinaryFP* fp);
};

//load material file sections
class MaterialHandler : public CG3DBinaryHandler {
public:
	virtual CG3DRefCount* Load(BinaryFP* fp);
};

//load mesh file sections
class ModelHandler : public CG3DBinaryHandler {
public:
	virtual CG3DRefCount* Load(BinaryFP* fp);
};

//load scene file sections
class SceneHandler : public CG3DBinaryHandler {
public:
	virtual CG3DRefCount* Load(BinaryFP* fp);
};

//load simple scene module
class SimpleSceneHandler : public CG3DBinaryHandler {
public:
	virtual CG3DRefCount* Load(BinaryFP* fp);
};

//load game object file sections
class GOBHandler : public CG3DBinaryHandler {
public:
	virtual CG3DRefCount* Load(BinaryFP* fp);
};

//load dummy module
class DummyModuleHandler : public CG3DBinaryHandler {
public:
	virtual CG3DRefCount* Load(BinaryFP* fp);
};

//load model module
class ModelModuleHandler : public CG3DBinaryHandler {
public:
	virtual CG3DRefCount* Load(BinaryFP* fp);
};

//load camera module
class CameraModuleHandler : public CG3DBinaryHandler {
public:
	virtual CG3DRefCount* Load(BinaryFP* fp);
};

//load frame m module
class FrameMModuleHandler : public CG3DBinaryHandler {
public:
	virtual CG3DRefCount* Load(BinaryFP* fp);
};

//load frame prs module
class FramePRSModuleHandler : public CG3DBinaryHandler {
public:
	virtual CG3DRefCount* Load(BinaryFP* fp);
};

//load frame pqs module
class FramePQSModuleHandler : public CG3DBinaryHandler {
public:
	virtual CG3DRefCount* Load(BinaryFP* fp);
};

//load frame ident module
class FrameIdentModuleHandler : public CG3DBinaryHandler {
public:
	virtual CG3DRefCount* Load(BinaryFP* fp);
};

#endif