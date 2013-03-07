#define _CRT_SECURE_NO_WARNINGS

#include "MaterialLib.h"
#include <stdio.h>
#include <string.h>

bool CG3DMaterialLib::GetBool(const char* str)
{
	if (!strcmp(str, "false"))
		return false;
	return true;
}

CG3DCullSet::Tripple CG3DMaterialLib::GetTripple(const char* str)
{
	if (!strcmp(str, "default"))
		return CG3DCullSet::DEFAULT;
	if (!strcmp(str, "false"))
		return CG3DCullSet::NO;
	return CG3DCullSet::YES;
}

CG3DDepthSet::Test CG3DMaterialLib::GetTest(const char* test)
{
	if (!strcmp(test, "always"))
		return CG3DDepthSet::ALWAYS;
	else if (!strcmp(test, "less"))
		return CG3DDepthSet::LESS;
	else if (!strcmp(test, "lequal"))
		return CG3DDepthSet::LEQUAL;
	else if (!strcmp(test, "equal"))
		return CG3DDepthSet::EQUAL;
	else if (!strcmp(test, "greater"))
		return CG3DDepthSet::GREATER;
	else if (!strcmp(test, "gequal"))
		return CG3DDepthSet::GEQUAL;
	else if (!strcmp(test, "notequal"))
		return CG3DDepthSet::NOTEQUAL;
	return CG3DDepthSet::NEVER;
}

CG3DStencilSet::Face CG3DMaterialLib::GetFace(const char* str)
{
	if (!strcmp(str, "front"))
		return CG3DStencilSet::FRONT;
	else if (!strcmp(str, "back"))
		return CG3DStencilSet::BACK;
	return CG3DStencilSet::FRONT_AND_BACK;
}

CG3DStencilSet::Op CG3DMaterialLib::GetOp(const char* str)
{
	if (!strcmp(str, "zero"))
		return CG3DStencilSet::ZERO;
	else if (!strcmp(str, "replace"))
		return CG3DStencilSet::REPLACE;
	else if (!strcmp(str, "incr"))
		return CG3DStencilSet::INCR;
	else if (!strcmp(str, "decr"))
		return CG3DStencilSet::DECR;
	else if (!strcmp(str, "invert"))
		return CG3DStencilSet::INVERT;
	else if (!strcmp(str, "incr_wrap"))
		return CG3DStencilSet::INCR_WRAP;
	else if (!strcmp(str, "decr_wrap"))
		return CG3DStencilSet::DECR_WRAP;
	return CG3DStencilSet::KEEP;
}

CG3DBlendSet::Func CG3DMaterialLib::GetFunc(const char* str)
{
	if (!strcmp(str, "subtract"))
		return CG3DBlendSet::FUNC_SUBTRACT;
	else if (!strcmp(str, "reverse"))
		return CG3DBlendSet::FUNC_REVERSE_SUBTRACT;
	return CG3DBlendSet::FUNC_ADD;
}

CG3DBlendSet::Param CG3DMaterialLib::GetParam(const char* str)
{
	if (!strcmp(str, "1"))
		return CG3DBlendSet::ONE;
	else if (!strcmp(str, "sc"))
		return CG3DBlendSet::SRC_COLOR;
	else if (!strcmp(str, "1-sc"))
		return CG3DBlendSet::ONE_MINUS_SRC_COLOR;
	else if (!strcmp(str, "dc"))
		return CG3DBlendSet::DST_COLOR;
	else if (!strcmp(str, "1-dc"))
		return CG3DBlendSet::ONE_MINUS_DST_COLOR;
	else if (!strcmp(str, "sa"))
		return CG3DBlendSet::SRC_ALPHA;
	else if (!strcmp(str, "1-sa"))
		return CG3DBlendSet::ONE_MINUS_SRC_ALPHA;
	else if (!strcmp(str, "da"))
		return CG3DBlendSet::DST_ALPHA;
	else if (!strcmp(str, "1-da"))
		return CG3DBlendSet::ONE_MINUS_DST_ALPHA;
	else if (!strcmp(str, "cc"))
		return CG3DBlendSet::CONSTANT_COLOR;
	else if (!strcmp(str, "1-cc"))
		return CG3DBlendSet::ONE_MINUS_CONSTANT_COLOR;
	else if (!strcmp(str, "ca"))
		return CG3DBlendSet::CONSTANT_ALPHA;
	else if (!strcmp(str, "1-ca"))
		return CG3DBlendSet::ONE_MINUS_CONSTANT_ALPHA;
	else if (!strcmp(str, "sa_saturate"))
		return CG3DBlendSet::SRC_ALPHA_SATURATE;
	return CG3DBlendSet::ZERO;
}

void CG3DMaterialLib::LoadText(const char* filename)
{
	char directory[256];
	strcpy(directory, filename);
	char* ptr = strrchr(directory, '\\');
	if (ptr == NULL) ptr = strrchr(directory, '/');
	if (ptr != NULL) ptr[1] = NULL;
	else directory[0] = NULL;

	FILE* fp = fopen(filename, "r");
	if (fp == NULL) return;

	CG3DMaterial* material = NULL;
	CG3DColourSet* colset = NULL;
	CG3DDepthSet* depthset = NULL;
	CG3DStencilSet* stensilset = NULL;
	CG3DCullSet* cullset = NULL;
	CG3DViewportSet* viewset = NULL;
	CG3DScissorSet* scissorset = NULL;
	CG3DBlendSet* blendset = NULL;
	CG3DSampleCoverageSet* sampleset = NULL;
	CG3DDitherSet* ditherset = NULL;
	CG3DPolygonOffsetSet* polyset = NULL;

	while (!feof(fp))
	{
		char line[1024];
		if (fgets(line, 1024, fp) == NULL)
			break;

		while (strlen(line) > 0 && line[strlen(line)-1] <= 32)
			line[strlen(line)-1] = 0;
		
		if (!strncmp(line, "material ",9))
		{
			char name[256];
			char effect[256];
			sscanf(line, "material %s %s", name, effect);

			material = m_Effect->MakeMaterial(effect);
			material->AddRef();
			m_Materials[name] = material;
		}
		else if (!strncmp(line, "parameter ",10))
		{
			if(material == NULL) continue;

			float fData[4];
			int iData[4];
			//char sData[256];

			char name[256];
			char type[256];
			int len;
			sscanf(line, "parameter %s %s%n", name, type, &len);

			if(!strncmp(type, "float", 5))
			{
				sscanf(line+len, " %f %f %f %f", &fData[0], &fData[1], &fData[2], &fData[3]);
				material->SetParameter(name, fData);
			} else  if(!strncmp(type, "int", 3))
			{
				sscanf(line+len, " %d %d %d %d", &iData[0], &iData[1], &iData[2], &iData[3]);
				material->SetParameter(name, iData);
			} else  if(!strcmp(type, "texture"))
			{
				char file[256];
				strcpy(file, directory);
				sscanf(line+len, " %s", file + strlen(file));
				CG3DTexture* texture = LoadTexture(file);
				material->SetTexture(name, texture);
			}
		}
		else if (!strncmp(line, "set ",4))
		{
			if(material == NULL) continue;

			char type[256];
			int len;
			sscanf(line, "set %s%n", type, &len);

			if (!strcmp(type, "colour"))
			{
				colset = new CG3DColourSet(CG3DColour(line+len));
				material->AddSet(CG3DGlobalSet::COLOUR, colset);
			}
			else if (!strcmp(type, "depth"))
			{
				float depth;
				sscanf(line+len, "%f", &depth);
				depthset = new CG3DDepthSet(depth);
				material->AddSet(CG3DGlobalSet::DEPTH, depthset);
			}
			else if (!strcmp(type, "stencil"))
			{
				int stencil;
				sscanf(line+len, "%d", &stencil);
				stensilset = new CG3DStencilSet(stencil);
				material->AddSet(CG3DGlobalSet::STENCIL, stensilset);
			}
			else if (!strcmp(type, "cull"))
			{
				char cull[32];
				sscanf(line+len, "%s", cull);
				if (!strcmp(cull, "back"))
					cullset = new CG3DCullSet(CG3DCullSet::CULL_BACK);
				else if (!strcmp(cull, "front"))
					cullset = new CG3DCullSet(CG3DCullSet::CULL_FRONT);
				else
					cullset = new CG3DCullSet(CG3DCullSet::CULL_NONE);
				material->AddSet(CG3DGlobalSet::CULL, cullset);
			}
			else if (!strcmp(type, "viewport"))
			{
				float x1=0, x2=0, y1=0, y2=0;
				sscanf(line+len, "%f %f %f %f", &x1, &y1, &x2, &y2);
				viewset = new CG3DViewportSet(CG3DVector4(x1, y1, x2, y2));
				material->AddSet(CG3DGlobalSet::VIEWPORT, viewset);
			}
			else if (!strcmp(type, "scissor"))
			{
				float x1=0, x2=0, y1=0, y2=0;
				sscanf(line+len, "%f %f %f %f", &x1, &y1, &x2, &y2);
				scissorset = new CG3DScissorSet(CG3DVector4(x1, y1, x2, y2));
				material->AddSet(CG3DGlobalSet::SCISSOR, scissorset);
			}
			else if (!strcmp(type, "blend"))
			{
				char mode[32];
				sscanf(line+len, "%s", mode);
				if (!strcmp(mode, "combined"))
					blendset = new CG3DBlendSet(CG3DBlendSet::COMBINED);
				else if (!strcmp(mode, "separate"))
					blendset = new CG3DBlendSet(CG3DBlendSet::SEPARATE);
				else
					blendset = new CG3DBlendSet(CG3DBlendSet::OFF);
				material->AddSet(CG3DGlobalSet::BLEND, blendset);
			}
			else if (!strcmp(type, "sample_coverage"))
			{
				char col[32], alpha[32];
				sscanf(line+len, "%s %s", col, alpha);
				sampleset = new CG3DSampleCoverageSet(GetBool(col), GetBool(alpha));
				material->AddSet(CG3DGlobalSet::SAMPLE_COVERAGE, sampleset);
			}
			else if (!strcmp(type, "dither"))
			{
				char dither[32];
				sscanf(line+len, "%s", dither);
				ditherset = new CG3DDitherSet(GetBool(dither));
				material->AddSet(CG3DGlobalSet::DITHER, ditherset);
			}
			else if (!strcmp(type, "polygon_offset"))
			{
				char poly[32];
				sscanf(line+len, "%s", poly);
				polyset = new CG3DPolygonOffsetSet(GetBool(poly));
				material->AddSet(CG3DGlobalSet::POLYGON_OFFSET, polyset);
			}
		}
		else if (!strncmp(line, "setparam ",9))
		{
			if(material == NULL) continue;

			char type[256];
			char type2[256];
			int len;
			sscanf(line, "setparam %s %s%n", type, type2, &len);
			if (!strcmp(type, "colour"))
			{
				if (colset == NULL) continue;

				if (!strcmp(type2, "doclear"))
				{
					char doclear[32];
					sscanf(line+len, "%s", doclear);
					colset->m_DoClear = GetBool(doclear);
				}
				else if (!strcmp(type2, "mask"))
				{
					char r[32], g[32], b[32], a[32];
					sscanf(line+len, "%s %s %s %s", r, g, b, a);
					colset->m_UseR = GetBool(r);
					colset->m_UseG = GetBool(g);
					colset->m_UseB = GetBool(b);
					colset->m_UseA = GetBool(a);
				}
			}
			else if (!strcmp(type, "depth"))
			{
				if (depthset == NULL) continue;

				if (!strcmp(type2, "doclear"))
				{
					char doclear[32];
					sscanf(line+len, "%s", doclear);
					depthset->m_DoClear = GetBool(doclear);
				}
				else if (!strcmp(type2, "writedepth"))
				{
					char write[32];
					sscanf(line+len, "%s", write);
					depthset->m_WriteDepth = GetBool(write);
				}
				else if (!strcmp(type2, "test"))
				{
					char test[32];
					sscanf(line+len, "%s", test);
					depthset->m_Test = GetTest(test);
				}
			}
			else if (!strcmp(type, "stencil"))
			{
				if (stensilset == NULL) continue;
				
				if (!strcmp(type2, "doclear"))
				{
					char doclear[32];
					sscanf(line+len, "%s", doclear);
					stensilset->m_DoClear = GetBool(doclear);
				}
				else if (!strcmp(type2, "face"))
				{
					char face[32];
					sscanf(line+len, "%s", face);
					stensilset->m_Face = GetFace(face);
				}
				else if (!strcmp(type2, "otherface"))
				{
					char face[32];
					sscanf(line+len, "%s", face);
					stensilset->m_OtherFace = new CG3DStencilSet(0);
					stensilset = stensilset->m_OtherFace;
					stensilset->m_Face = GetFace(face);
				}
				else if (!strcmp(type2, "mask"))
				{
					unsigned int mask;
					sscanf(line+len, "%d", &mask);
					stensilset->m_Mask = mask;
				}
				else if (!strcmp(type2, "func"))
				{
					char test[32];
					int ref;
					unsigned int mask;
					sscanf(line+len, "%s %d %d", test, &ref, &mask);
					stensilset->m_Func = (CG3DStencilSet::Test)GetTest(test);
					stensilset->m_FuncRef = ref;
					stensilset->m_FuncMask = mask;
				}
				else if (!strcmp(type2, "op"))
				{
					char sf[32];
					char df[32];
					char dp[32];
					sscanf(line+len, "%s %s %s", sf, df, dp);
					stensilset->m_StencilFail = GetOp(sf);
					stensilset->m_DepthFail= GetOp(df);
					stensilset->m_DepthPass = GetOp(dp);
				}
			}
			else if (!strcmp(type, "cull"))
			{
				if (cullset == NULL) continue;
				
				if (!strcmp(type2, "frontisccw"))
				{
					char ccw[32];
					sscanf(line+len, "%s", ccw);
					cullset->m_FrontIsCCW = GetTripple(ccw);
				}
			}
			else if (!strcmp(type, "viewport"))
			{
				if (viewset == NULL) continue;

				if (!strcmp(type2, "z"))
				{
					float near, far;
					sscanf(line+len, "%f %f", &near, &far);
					viewset->m_NearZ = near;
					viewset->m_FarZ= far;
				}
			}
			else if (!strcmp(type, "scissor"))
			{
				if (scissorset == NULL) continue;

				if (!strcmp(type2, "do"))
				{
					char doscissor[32];
					sscanf(line+len, "%s", doscissor);
					scissorset->m_Do = GetBool(doscissor);
				}
			}
			else if (!strcmp(type, "blend"))
			{
				if (blendset == NULL) continue;

				if (!strcmp(type2, "func"))
				{
					char func[32];
					char afunc[32];
					sscanf(line+len, "%s %s", func, afunc);
					blendset->m_Func = GetFunc(func);
					blendset->m_FuncAlpha = GetFunc(afunc);
				}
				else if (!strcmp(type2, "param"))
				{
					char src[32], dest[32], asrc[32], adest[32];
					sscanf(line+len, "%s %s %s %s", src, dest, asrc, adest);
					blendset->m_Src = GetParam(src);
					blendset->m_Dest = GetParam(dest);
					blendset->m_SrcAlpha = GetParam(asrc);
					blendset->m_DestAlpha = GetParam(adest);
				}
				else if (!strcmp(type, "colour"))
				{
					char col[32];
					sscanf(line+len, "%s", col);
					blendset->m_Colour = CG3DColour(line+len);
				}
			}
			else if (!strcmp(type, "sample_coverage"))
			{
				if (sampleset == NULL) continue;
				
				if (!strcmp(type, "value"))
				{
					float value;
					char invert[32];
					sscanf(line+len, "%f %s", &value, invert);
					sampleset->m_Value = value;
					sampleset->m_Invert = GetBool(invert);
				}
			}
			else if (!strcmp(type, "polygon_offset"))
			{
				if (polyset == NULL) continue;
				
				if (!strcmp(type, "factor"))
				{
					float factor, units;
					sscanf(line+len, "%f %f", &factor, &units);
					polyset->m_Factor = factor;
					polyset->m_Units = units;
				}
			}
		}
	}
	fclose(fp);
}

CG3DTexture* CG3DMaterialLib::LoadTexture(const char* filename)
{
	FILE* fp = fopen(filename, "rb");
	if (fp == NULL) return NULL;

	CG3DTexture* text = NULL;
		
	int level;
	int width;
	int height;
	int bpp;
	ImageFormat format;
	ImageFormat currFormat;
	CG3DTexture::Target target;

	while (!feof(fp))
	{
		char line[1024];
		if (fgets(line, 1024, fp) == NULL)
			break;

		while (strlen(line) > 0 && line[strlen(line)-1] <= 32)
			line[strlen(line)-1] = 0;

		if (!strncmp(line, "texture ",8))
		{
			char type[256];
			char filter[256];
			char s[256];
			char t[256];
			int numMips;
			sscanf(line, "texture %s %s %s %s %d", type, filter, s, t, &numMips);

			text = m_Graphics->MakeTexture();
			text->Setup(strcmp(type, "2d") != 0, false, GetFilter(filter), GetWrap(s), GetWrap(t));
		}
		else if (!strncmp(line, "image ",6))
		{
			char type[256];
			sscanf(line, "image %s %d %d %d %d", type, &level, &width, &height, &bpp);

			target = GetTarget(type);
		}
		else if (!strncmp(line, "format ",7))
		{
			char formatDepth[256];
			char formatType[256];
			char formatLayout[256];
			char formatFlag[256];
			sscanf(line, "format %s %s %s %s", formatDepth, formatType, formatLayout, formatFlag);
			format.depth = GetDepth(formatDepth);
			format.type = GetType(formatType);
			format.layout = GetLayout(formatLayout);
			format.flag = GetFlag(formatFlag);

			if (currFormat.depth == ImageFormat::Unknown)
				if (text->ValidFormat(format))
					currFormat = format;
		}
		else if (!strncmp(line, "idata ",6))
		{
			unsigned int length = 0;
			sscanf(line, "idata %d", &length);
			
			if (currFormat != format)
				continue;
			
			unsigned char* data = (unsigned char*)text->AddImage(target, format, width, height, length, level);
			if (data != NULL)
				LoadFileBinary(fp, data, length);
		}
	}

	fclose(fp);
	return text;
}

void CG3DMaterialLib::LoadFileBinary(FILE* fp, unsigned char* data, unsigned int len)
{
	unsigned int count=0;
	data[len]=0;

	while(!feof(fp)  && count<len)
	{
		char line[1024];
		if (fgets(line, 1024, fp) == NULL)
			break;

		if (line[0]!='i' || line[1]!=' ')
			break;
		char* ptr = line+2;
		unsigned int byte = 0;
		int num, num2;
		sscanf(ptr, " %n%02x%n", &num2, &byte, &num);
		while (num-num2>0 && count<len)
		{
			data[count++] = byte;
			ptr+=num;
			sscanf(ptr, " %n%02x%n", &num2, &byte, &num);
		}
	}
}

#define SET_MIP_FILTER(result, v) result = (CG3DTexture::MipMap)(result | ((int)CG3DTexture::v))

CG3DTexture::MipMap CG3DMaterialLib::GetFilter(const char* str)
{
	CG3DTexture::MipMap result = (CG3DTexture::MipMap)0;
	char line[1024];
	strcpy(line, str);

	char* ptr = strchr(line, '|');
	if (ptr == NULL) return result;
	ptr[0] = 0;
	ptr++;

	char* ptr2 = strchr(ptr, '|');
	if (ptr2 == NULL) return result;
	ptr2[0] = 0;
	ptr2++;

	if (!strcmp(line, "NEAREST")) SET_MIP_FILTER(result, MIN_NEAREST);
	else if (!strcmp(line, "LINEAR")) SET_MIP_FILTER(result, MIN_LINEAR);

	if (!strcmp(ptr, "NEAREST")) SET_MIP_FILTER(result, MAG_NEAREST);
	else if (!strcmp(ptr, "LINEAR")) SET_MIP_FILTER(result, MAG_LINEAR);

	if (!strcmp(ptr2, "NEAREST")) SET_MIP_FILTER(result, MIP_NEAREST);
	else if (!strcmp(ptr2, "LINEAR")) SET_MIP_FILTER(result, MIP_LINEAR);
	return result;
}
CG3DTexture::Wrap CG3DMaterialLib::GetWrap(const char* str)
{
	if (!strcmp(str, "CLAMP")) return CG3DTexture::CLAMP;
	else if (!strcmp(str, "MIRRORED")) return CG3DTexture::MIRRORED;
	else if (!strcmp(str, "BORDER")) return CG3DTexture::BORDER;
	else if (!strcmp(str, "MIRROR_ONCE")) return CG3DTexture::MIRROR_ONCE;
	return CG3DTexture::REPEAT;
}
CG3DTexture::Target CG3DMaterialLib::GetTarget(const char* str)
{
	if (!strcmp(str, "-x")) return CG3DTexture::TARGET_NEG_X;
	else if (!strcmp(str, "-y")) return CG3DTexture::TARGET_NEG_Y;
	else if (!strcmp(str, "-z")) return CG3DTexture::TARGET_NEG_Z;
	else if (!strcmp(str, "+x")) return CG3DTexture::TARGET_POS_X;
	else if (!strcmp(str, "+y")) return CG3DTexture::TARGET_POS_Y;
	else if (!strcmp(str, "+z")) return CG3DTexture::TARGET_POS_Z;
	return CG3DTexture::TARGET_2D;
}
ImageFormat::Depth CG3DMaterialLib::GetDepth(const char* str)
{
	if (!strcmp(str, "D32_8_24")) return ImageFormat::D32_8_24;
	else if (!strcmp(str, "D32")) return ImageFormat::D32;
	else if (!strcmp(str, "D24_8")) return ImageFormat::D24_8;
	else if (!strcmp(str, "D11_11_10")) return ImageFormat::D11_11_10;
	else if (!strcmp(str, "D10_10_10_2")) return ImageFormat::D10_10_10_2;
	else if (!strcmp(str, "D16")) return ImageFormat::D16;
	else if (!strcmp(str, "D5_6_5")) return ImageFormat::D5_6_5;
	else if (!strcmp(str, "D5_5_5_1")) return ImageFormat::D5_5_5_1;
	else if (!strcmp(str, "D4_4_4_4")) return ImageFormat::D4_4_4_4;
	else if (!strcmp(str, "D8")) return ImageFormat::D8;
	else if (!strcmp(str, "Compressed")) return ImageFormat::Compressed;
	return ImageFormat::Unknown;
}
ImageFormat::Type CG3DMaterialLib::GetType(const char* str)
{
	if (!strcmp(str, "Float")) return ImageFormat::Float;
	else if (!strcmp(str, "Unsigned")) return ImageFormat::Unsigned;
	else if (!strcmp(str, "Signed")) return ImageFormat::Signed;
	else if (!strcmp(str, "Float_Half_Unsigned")) return ImageFormat::Float_Half_Unsigned;
	else if (!strcmp(str, "Float_Half_Signed")) return ImageFormat::Float_Half_Signed;
	return ImageFormat::Typeless;
}
ImageFormat::Layout CG3DMaterialLib::GetLayout(const char* str)
{
	if (!strcmp(str, "R")) return ImageFormat::R;
	else if (!strcmp(str, "RG")) return ImageFormat::RG;
	else if (!strcmp(str, "A")) return ImageFormat::A;
	else if (!strcmp(str, "RGBA")) return ImageFormat::RGBA;
	else if (!strcmp(str, "L")) return ImageFormat::L;
	else if (!strcmp(str, "LA")) return ImageFormat::LA;
	else if (!strcmp(str, "BGR")) return ImageFormat::BGR;
	else if (!strcmp(str, "BGRA")) return ImageFormat::BGRA;
	else if (!strcmp(str, "BGRX")) return ImageFormat::BGRX;
	else if (!strcmp(str, "D")) return ImageFormat::D;
	else if (!strcmp(str, "DS")) return ImageFormat::DS;
	else if (!strcmp(str, "BC1")) return ImageFormat::BC1;
	else if (!strcmp(str, "BC2")) return ImageFormat::BC2;
	else if (!strcmp(str, "BC3")) return ImageFormat::BC3;
	else if (!strcmp(str, "BC4")) return ImageFormat::BC4;
	else if (!strcmp(str, "BC5")) return ImageFormat::BC5;
	else if (!strcmp(str, "BC6H")) return ImageFormat::BC6H;
	else if (!strcmp(str, "BC7")) return ImageFormat::BC7;
	else if (!strcmp(str, "RGB_S3TC_DXT1")) return ImageFormat::RGB_S3TC_DXT1;
	else if (!strcmp(str, "RGBA_S3TC_DXT1")) return ImageFormat::RGBA_S3TC_DXT1;
	else if (!strcmp(str, "RGBA_S3TC_DXT3")) return ImageFormat::RGBA_S3TC_DXT3;
	else if (!strcmp(str, "RGBA_S3TC_DXT5")) return ImageFormat::RGBA_S3TC_DXT5;
	else if (!strcmp(str, "ETC1_RGB8")) return ImageFormat::ETC1_RGB8;
	else if (!strcmp(str, "RGB_PVRTC_4BPPV1")) return ImageFormat::RGB_PVRTC_4BPPV1;
	else if (!strcmp(str, "RGB_PVRTC_2BPPV1")) return ImageFormat::RGB_PVRTC_2BPPV1;
	else if (!strcmp(str, "RGBA_PVRTC_4BPPV1")) return ImageFormat::RGBA_PVRTC_4BPPV1;
	else if (!strcmp(str, "RGBA_PVRTC_2BPPV1")) return ImageFormat::RGBA_PVRTC_2BPPV1;
	else if (!strcmp(str, "ATC_RGB")) return ImageFormat::ATC_RGB;
	else if (!strcmp(str, "ATC_RGBA_EXPLICIT_ALPHA")) return ImageFormat::ATC_RGBA_EXPLICIT_ALPHA;
	else if (!strcmp(str, "ATC_RGBA_INTERPOLATED_ALPHA")) return ImageFormat::ATC_RGBA_INTERPOLATED_ALPHA;
	return ImageFormat::RGB;
}
ImageFormat::Flag CG3DMaterialLib::GetFlag(const char* str)
{
	if (!strcmp(str, "Norm")) return ImageFormat::Norm;
	else if (!strcmp(str, "Norm_SRGB")) return ImageFormat::Norm_SRGB;
	else if (!strcmp(str, "Alpha")) return ImageFormat::Alpha;
	return ImageFormat::None;
}

CG3DMaterial* CG3DMaterialLib::GetMaterial(const char* name)
{
	if (m_Materials.find(name) == m_Materials.end())
		return NULL;
	return m_Materials[name];
}

void CG3DMaterialLib::Release()
{
	std::map<std::string, CG3DMaterial*>::iterator it;
	for (it = m_Materials.begin(); it != m_Materials.end(); ++it)
		it->second->Release();
}