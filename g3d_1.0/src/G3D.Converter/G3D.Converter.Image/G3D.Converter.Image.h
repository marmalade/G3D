// G3D.Converter.Image.h
#pragma once

using namespace System;
using namespace System::Collections::Generic;
using namespace System::Text;

#define NAMESPACE_START namespace G3D { namespace Converter { namespace Image {
#define NAMESPACE_END }}}

NAMESPACE_START
	
public ref class ImageFormat {
public:
	enum class Depth : unsigned char {
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
    enum class Type : unsigned char {
        Typeless = 0,
        Float = 1,
        Unsigned = 2,
        Signed = 4,
        Float_Half_Unsigned = 35,
        Float_Half_Signed = 37
    };
	enum class Flag : unsigned char {
        None = 0,
        Norm = 1,
        Norm_SRGB = 3,
        Alpha = 4,
    };
	enum class Layout : unsigned char {
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
	ImageFormat^ format_alpha;
public:
	ImageFormat(Depth _depth,Type _type,Layout _layout,Flag _flag) { depth=_depth; type=_type; layout=_layout; flag=_flag; format_alpha=nullptr; }
	ImageFormat() { depth=Depth::Unknown; type=Type::Typeless; layout=Layout::RGB; flag=Flag::None; format_alpha=nullptr; }
	virtual int GetHashCode() override {
        return ((int)depth)+(((int)type)<<8)+((int)layout<<16)+(((int)flag)<<24);
    }
};
public ref class ImageData {
public:
	int m_Width;
	int m_Height;
	int m_BytesPerPixel;
	int m_MipLevel;
	array<unsigned char>^ m_Data;
	ImageFormat^ format;
};
public ref class ILImageFormat {
public:
	ImageFormat^ outFormat;
	int format;
	int type;
	bool postConvert;
	int convertBPP;
	int newBPP;
public:
	ILImageFormat(ImageFormat^ format) { outFormat = format; }
};

public ref class ImageMgr {
public:
	void Init();
	String^ Load(String^ filename, ImageFormat^ format, List<ImageData^>^ list, int numMips);
	void Shutdown();
private:
	String^ GetFormat(ILImageFormat^ format);
	ImageData^ MakeImage(ILImageFormat^ format, int level);
	void PostConvert(ILImageFormat^ format, ImageData^ data);
	void PostConvertLayout(ILImageFormat^ format, ImageData^ data);
};

NAMESPACE_END