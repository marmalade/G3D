#ifndef G3D_CORE_TEXTURE_H
#define G3D_CORE_TEXTURE_H

// A collection of images and texture state that make up a texture

#include "Resource.h"
#include <stdlib.h>

// describe the format of a texture, only a subset of combinations are supported, which is platform dependant
class ImageFormat {
public:
	// The size of each channel (e.g. D8 specifies 8 bits in each channel and D5_5_5_1 specifies 5 bits in r, g and b and 1 bit in a)
	enum Depth {
        Unknown = 0,
        D32_8_24 = 64,
        D32 = 32,
        D24_8 = 24,
        D11_11_10 = 11,
        D10_10_10_2 = 10,
        D16 = 16,
        D5_6_5 = 6,
        D5_5_5_1 = 5,
        D4_4_4_4 = 4,
        D8 = 8,
        Compressed = 128
    };
	// type of each channel (some formats (like D32_8_24) may specify some channels differently)
    enum Type {
        Typeless = 0,
        Float = 1,
        Unsigned = 2,
        Signed = 4,
        Float_Half_Unsigned = 35,
        Float_Half_Signed = 37
    };
	// additional flags
	enum Flag {
        None = 0,
        Norm = 1,
        Norm_SRGB = 3,
        Alpha = 4,
    };
	// if uncompressed, specify the layout of channels
	// if compressed, specify the compression format
	// R is red, G is green, B is blue, A is alpha, L is luminance, X is unused, D is depth, S is stencil
	enum Layout {
		//uncompressed
		R = 1,
		RG = 3,
		RGB = 7,
		A = 8,
		RGBA = 15,
		L = 16,
		LA = 24,
		BGR = 39,
		BGRA = 47,
		BGRX = 71,
		D = 32,
		DS = 96,

		//compressed
        BC1 = 1,
        BC2 = 2,
        BC3 = 3,
        BC4 = 4,
        BC5 = 5,
        BC6H = 6,
        BC7 = 7,
                
        RGB_S3TC_DXT1 = 0,
        RGBA_S3TC_DXT1 = 1,
        RGBA_S3TC_DXT3 = 2,
        RGBA_S3TC_DXT5 = 3,
                
        ETC1_RGB8 = 16,

        RGB_PVRTC_4BPPV1 = 32,
        RGB_PVRTC_2BPPV1 = 33,
        RGBA_PVRTC_4BPPV1 = 34,
        RGBA_PVRTC_2BPPV1 = 35,
                
        ATC_RGB = 64,
        ATC_RGBA_EXPLICIT_ALPHA = 65,
        ATC_RGBA_INTERPOLATED_ALPHA = 66,
    };
public:
	Depth depth;
    Type type;
	Layout layout;
    Flag flag;

public:
	ImageFormat(Depth _depth,Type _type,Layout _layout,Flag _flag) { depth=_depth; type=_type; layout=_layout; flag=_flag; }
	ImageFormat(const ImageFormat& format) { depth=format.depth; type=format.type; layout=format.layout; flag=format.flag; }
	ImageFormat() { depth=Unknown; type=Typeless; layout=RGB; flag=None; }
	
	// int version of the data
	int GetHashCode() const {
        return ((int)(unsigned char)depth)+(((int)(unsigned char)type)<<8)+((int)(unsigned char)layout<<16)+(((int)(unsigned char)flag)<<24);
    }
	bool operator==(const ImageFormat& format) { return GetHashCode()==format.GetHashCode(); }
	bool operator!=(const ImageFormat& format) { return GetHashCode()!=format.GetHashCode(); }
};

class CG3DTexture : public CG3DResource {
public:
	enum MipMap {
        MIN_NONE = 0,
        MIN_NEAREST = 1,
        MIN_LINEAR = 2,
        MAG_NONE = 0,
        MAG_NEAREST = 4,
        MAG_LINEAR = 8,
        MIP_NONE = 0,
        MIP_NEAREST = 16,
        MIP_LINEAR = 32,
	};
	enum Wrap {
		CLAMP,
		MIRRORED,
		REPEAT,
		BORDER,
		MIRROR_ONCE,
	};
	enum Target {
		TARGET_2D,
		TARGET_POS_X,
		TARGET_POS_Y,
		TARGET_POS_Z,
		TARGET_NEG_X,
		TARGET_NEG_Y,
		TARGET_NEG_Z,
	};
public:
	// set up the type of texture and its state
	virtual void Setup(bool cube, bool generateMipmaps, MipMap filter, Wrap s, Wrap t) = 0;
	// Add a particular image to the collection of images specifying its type, format, dimensions, and mipmap level.
	// If the image is uncompressed dataSize can be 0, dataSize is then calculated from the format and dimensions.
	// This returns a pointer to the data buffer to be filled.
	virtual void* AddImage(Target target,ImageFormat format,int width, int height, int dataSize = 0, int mipmap = 0) = 0;
	// is this image format valid?
	virtual bool ValidFormat(ImageFormat format) = 0;
};

#endif