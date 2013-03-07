#include "G3D.Converter.Image.h"
#include <string.h>

#undef _UNICODE
#include "IL\il.h"
#include "IL\ilu.h"

NAMESPACE_START

String^ GetString(const wchar_t* str) {
	StringBuilder^ sb=gcnew StringBuilder();
	for(int i=0;str[i]!=0;i++)
		sb->Append(str[i]);
	return sb->ToString();
}
String^ GetString(const char* str) {
	if(str==NULL) return "";

	StringBuilder^ sb=gcnew StringBuilder();
	for(int i=0;str[i]!=0;i++)
		sb->Append((wchar_t)str[i]);
	return sb->ToString();
}
wchar_t* FromStringW(String^ str) {
	wchar_t* res=new wchar_t[str->Length+1];
	for(int i=0;i<str->Length;i++)
		res[i]=str[i];
	res[str->Length]=0;
	return res;
}
char* FromStringA(String^ str) {
	char* res=new char[str->Length+1];
	for(int i=0;i<str->Length;i++)
		res[i]=(char)str[i];
	res[str->Length]=0;
	return res;
}

void ImageMgr::Init() {
	ilInit();
	iluInit();
}
String^ ImageMgr::GetFormat(ILImageFormat^ f)
{	
	f->format = ilGetInteger(IL_IMAGE_FORMAT);
	f->type = ilGetInteger(IL_IMAGE_TYPE);
	f->postConvert = false;
	f->convertBPP = 0;
	f->newBPP = 0;
	
	if (f->format == IL_COLOUR_INDEX)
	{
		int ptype = ilGetInteger(IL_PALETTE_TYPE);
		if (ptype == IL_PAL_RGBA32 || ptype == IL_PAL_BGRA32)
			f->format = IL_RGBA;
		else
			f->format = IL_RGB;
	}
	else if (f->format == IL_ALPHA || f->format == IL_RGBA || f->format == IL_BGRA || f->format == IL_LUMINANCE_ALPHA)
		f->format = IL_RGBA;
	else
		f->format = IL_RGB;

	if(f->outFormat->format_alpha != nullptr && f->format ==IL_RGBA)
		f->outFormat = f->outFormat->format_alpha;

	if (f->outFormat->depth == ImageFormat::Depth::Unknown)
	{
		f->outFormat = gcnew ImageFormat();
		f->outFormat->depth = ImageFormat::Depth::D8;
		f->outFormat->type = ImageFormat::Type::Unsigned;
		f->convertBPP = (f->format == IL_RGB) ? 3 : 4;

		if (f->format == IL_RGBA)
			f->outFormat->layout = ImageFormat::Layout::RGBA;
		else
			f->outFormat->layout = ImageFormat::Layout::RGB;
	}
	else
	{
		switch (f->outFormat->depth)
		{
			case ImageFormat::Depth::D32_8_24:
				f->convertBPP = 4;
				f->newBPP = 8;
				break;
			case ImageFormat::Depth::D24_8:
				f->convertBPP = 4;
				f->newBPP = 4;
				break;
			case ImageFormat::Depth::D32:
				f->convertBPP = 4;
				break;
			case ImageFormat::Depth::D11_11_10:
			case ImageFormat::Depth::D10_10_10_2:
				f->convertBPP = 2;
				f->newBPP = 4;
				break;
			case ImageFormat::Depth::D16:
				f->convertBPP = 2;
				break;
			case ImageFormat::Depth::D5_6_5:
			case ImageFormat::Depth::D5_5_5_1:
			case ImageFormat::Depth::D4_4_4_4:
				f->convertBPP = 1;
				f->newBPP = 2;
				break;
			case ImageFormat::Depth::D8:
				f->convertBPP = 1;
				break;
			case ImageFormat::Depth::Compressed:
				return "compressed not available yet";
			default:
				return "Unknown format";
		}
		switch (f->outFormat->type)
		{
			case ImageFormat::Type::Float:
				switch (f->convertBPP)
				{
					case 2: f->type = IL_HALF; break;
					case 4: f->type = IL_FLOAT; break;
					default: return "unknown float depth";
				}
				break;
			case ImageFormat::Type::Signed:
				switch (f->convertBPP)
				{
					case 1: f->type = IL_BYTE; break;
					case 2: f->type = IL_SHORT; break;
					case 4: f->type = IL_INT; break;
					default: return "unknown signed depth";
				}
				break;
			case ImageFormat::Type::Unsigned:
				switch (f->convertBPP)
				{
					case 1: f->type = IL_UNSIGNED_BYTE; break;
					case 2: f->type = IL_UNSIGNED_SHORT; break;
					case 4: f->type = IL_UNSIGNED_INT; break;
					default: return "unknown unsigned depth";
				}
				break;
			default:
				return "unsupported type";
		}
		switch (f->outFormat->layout)
		{
			case ImageFormat::Layout::R:
			case ImageFormat::Layout::RG:
				f->postConvert = true;
			case ImageFormat::Layout::RGB:
				f->convertBPP *= 3;
				f->format = IL_RGB;
				break;
			case ImageFormat::Layout::RGBA:
				f->convertBPP *= 4;
				f->format = IL_RGBA;
				break;
			case ImageFormat::Layout::A:
				f->format = IL_ALPHA;
				break;
			case ImageFormat::Layout::L:
				f->format = IL_LUMINANCE;
				break;
			case ImageFormat::Layout::LA:
				f->convertBPP *= 2;
				f->format = IL_LUMINANCE_ALPHA;
				break;
			case ImageFormat::Layout::BGR:
				f->convertBPP *= 3;
				f->format = IL_BGR;
				break;
			case ImageFormat::Layout::BGRX:
			case ImageFormat::Layout::BGRA:
				f->convertBPP *= 4;
				f->format = IL_BGRA;
				break;
			case ImageFormat::Layout::D:
			case ImageFormat::Layout::DS:
				return "cannot convert depth textures";
		}
	}

	return "";
}
String^ ImageMgr::Load(String^ filename, ImageFormat^ outFormat, List<ImageData^>^ list, int numMips)
{
	ILuint image;
	char* str=FromStringA(filename);

	ilGenImages(1, &image);
	ilBindImage(image);
	if(!ilLoadImage(str)) {
		ILenum err=ilGetError();
		delete[] str;
		ilDeleteImages(1, &image);
		return  GetString((char*)iluErrorString(err));
	}
	delete[] str;
	
	ILImageFormat^ format = gcnew ILImageFormat(outFormat);
	String^ error = GetFormat(format);
	if(error!="")
		return error;

	if(!ilConvertImage(format->format, format->type))
	{
		ilDeleteImages(1, &image);
		return "failed to convert image";
	}

	if (numMips>0)
	{
		iluBuildMipmaps();

		for(int i=0; i<numMips; i++) {
			ImageData^ data=MakeImage(format,i);
			if(data!=nullptr)
			{
				if (format->newBPP > 0)
					PostConvert(format, data);
				if (format->postConvert)
					PostConvertLayout(format, data);
				list->Add(data);
			}
			// this advances number of mips not sets on level x (idiotic)
			if(!ilActiveMipmap(1))
				break;
		}
	} else {
		ImageData^ data=MakeImage(format,0);
		if(data!=nullptr)
		{
			if (format->newBPP > 0)
				PostConvert(format, data);
			if (format->postConvert)
				PostConvertLayout(format, data);
			list->Add(data);
		}
	}

	ilDeleteImages(1, &image);

	return "";
}
ImageData^ ImageMgr::MakeImage(ILImageFormat^ format, int level) {

	ImageData^ data=gcnew ImageData();
	
	data->m_Width = ilGetInteger(IL_IMAGE_WIDTH);
	data->m_Height = ilGetInteger(IL_IMAGE_HEIGHT);
	data->m_MipLevel = level;

	data->m_BytesPerPixel = format->convertBPP;
	data->format = format->outFormat;

	data->m_Data = gcnew array<unsigned char>(data->m_Width * data->m_Height * data->m_BytesPerPixel);
	pin_ptr<unsigned char> p=&data->m_Data[0];
	unsigned char* np=p;

	int res=ilCopyPixels(0, 0, 0, data->m_Width, data->m_Height, 1, format->format, format->type, np);
	int error=ilGetError();
	if (res>0)
		return data;

	return nullptr;
}
void ImageMgr::PostConvert(ILImageFormat^ format, ImageData^ data)
{
#define SETI(offset,shift,mask,pos) (((icol[offset]>>shift)&mask)<<pos)

	array<unsigned char>^ newData = gcnew array<unsigned char>(data->m_Width * data->m_Height * format->newBPP);
	pin_ptr<unsigned char> p1=&newData[0];
	pin_ptr<unsigned char> p2=&data->m_Data[0];
	unsigned char* data1=p1;
	unsigned char* data2=p2;

	for (int i=0; i<data->m_Width * data->m_Height; i++)
	{
		unsigned int icol[4];
		float fcol;
		int num;

		switch (format->type)
		{
			case IL_BYTE:
				num  = format->convertBPP;
				for (int j=0; j<num; j++)
					icol[j] = ((unsigned char*)data2)[i*num + j];
				fcol = icol[0]/(float)SCHAR_MAX;
				break;
			case IL_UNSIGNED_BYTE:
				num  = format->convertBPP;
				for (int j=0; j<num; j++)
					icol[j] = ((unsigned char*)data2)[i*num + j];
				fcol = icol[0]/(float)UCHAR_MAX;
				break;
			case IL_SHORT:
				num  = format->convertBPP/2;
				for (int j=0; j<num; j++)
					icol[j] = ((unsigned short*)data2)[i*num + j];
				fcol = icol[0]/(float)SHRT_MAX;
				break;
			case IL_UNSIGNED_SHORT:
				num  = format->convertBPP/2;
				for (int j=0; j<num; j++)
					icol[j] = ((unsigned short*)data2)[i*num + j];
				fcol = icol[0]/(float)USHRT_MAX;
				break;
			case IL_INT:
				num  = format->convertBPP/4;
				for (int j=0; j<num; j++)
					icol[j] = ((unsigned int*)data2)[i*num + j];
				fcol = icol[0]/(float)INT_MAX;
				break;
			case IL_UNSIGNED_INT:
				num  = format->convertBPP/4;
				for (int j=0; j<num; j++)
					icol[j] = ((unsigned int*)data2)[i*num + j];
				fcol = icol[0]/(float)UINT_MAX;
				break;
			case IL_FLOAT:
				for (int j=0; j<num; j++)
					if (format->outFormat->type == ImageFormat::Type::Signed)
						icol[j] = (int)(((float*)data2)[i*num + j] * (float)INT_MAX);
					else
						icol[j] = (unsigned int)(((float*)data2)[i*num + j] * (float)UINT_MAX);
				fcol = ((float*)data2)[i*num];
				break;
			case IL_HALF:
				//TODO
				break;
		}

		switch (format->outFormat->depth)
		{
			case ImageFormat::Depth::D32_8_24:
				if (format->outFormat->type == ImageFormat::Type::Float)
					((float*)data1)[i*2] = fcol;
				else
					((unsigned int*)data1)[i*2] = icol[0];
				((unsigned int*)data1)[i*2+1] = (unsigned int)(SETI(1,24,0xff,0) + SETI(2,8,0xffffff,24));
				break;
			case ImageFormat::Depth::D24_8:
				((unsigned int*)data1)[i] = (unsigned int)(SETI(0,8,0xffffff,0) + SETI(1,24,0xff,24));
				break;
			case ImageFormat::Depth::D11_11_10:
				((unsigned int*)data1)[i] = (unsigned int)(SETI(0,5,0x7ff,0) + SETI(1,5,0x7ff,11) + SETI(2,6,0x3ff,22));
				break;
			case ImageFormat::Depth::D10_10_10_2:
				((unsigned int*)data1)[i] = (unsigned int)(SETI(0,6,0x3ff,0) + SETI(1,6,0x3ff,10) + SETI(2,6,0x3ff,20) + SETI(3,14,0x3,30));
				break;
			case ImageFormat::Depth::D5_6_5:
				((unsigned short*)data1)[i] = (unsigned short)(SETI(0,3,0x1f,0) + SETI(1,2,0x3f,5) + SETI(2,3,0x1f,11));
				break;
			case ImageFormat::Depth::D5_5_5_1:
				((unsigned short*)data1)[i] = (unsigned short)(SETI(0,3,0x1f,0) + SETI(1,3,0x1f,5) + SETI(2,3,0x1f,10) + SETI(3,7,0x1,15));
				break;
			case ImageFormat::Depth::D4_4_4_4:
				((unsigned short*)data1)[i] = (unsigned short)(SETI(0,4,0xf,0) + SETI(1,4,0xf,4) + SETI(2,4,0xf,8) + SETI(3,4,0xf,12));
				break;
		}
	}
	data->m_BytesPerPixel = format->newBPP;
	data->m_Data = newData;
#undef SETI
}
void ImageMgr::PostConvertLayout(ILImageFormat^ format, ImageData^ data)
{
	int width = 0, num = 0;
	switch (format->outFormat->layout)
	{
		case ImageFormat::Layout::R:  num = 1; width = format->convertBPP / 3; break;
		case ImageFormat::Layout::RG: num = 2; width = format->convertBPP * 2 / 3; break;
		default: return;
	}

	array<unsigned char>^ newData = gcnew array<unsigned char>(data->m_Width * data->m_Height * 2);
	pin_ptr<unsigned char> p1=&newData[0];
	pin_ptr<unsigned char> p2=&data->m_Data[0];
	unsigned char* data1=p1;
	unsigned char* data2=p2;

	for (int i=0; i<data->m_Width * data->m_Height; i++)
	{
		memcpy(data1 + i * width * num,data2 + i * width * 3, width * num);
	}
}
void ImageMgr::Shutdown() {
	ilShutDown();
}

NAMESPACE_END
