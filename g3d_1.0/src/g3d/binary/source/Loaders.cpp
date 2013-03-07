#include "Loaders.h"
#include "G3DHelper.h"
#include "G3DHierarchy.h"

CG3DRefCount* TextureHandler::Load(BinaryFP* fp)
{
	CG3DTexture* text = m_Loader->m_Graphics->MakeTexture();
	bool cube = false;
	unsigned char filter = fp->ReadUInt8();
	unsigned char s = fp->ReadUInt8();
	unsigned char t = fp->ReadUInt8();
	unsigned char numMips = fp->ReadUInt8();
	int num = fp->ReadInt32();
	ImageFormat currFormat;

	if((filter & 128) == 128)
	{
		filter &= 127;
		cube = true;
	}

	text->Setup(cube, false, (CG3DTexture::MipMap)filter, (CG3DTexture::Wrap)s, (CG3DTexture::Wrap)t);

	fp = m_Loader->OpenFile();
	for (int i=0; i<num; i++)
	{
		if (i>0)
			m_Loader->NextFile();
		if (fp->m_Ident == IDENT('I','M','A','G'))
		{
			ImageFormat format;
			format.depth = (ImageFormat::Depth)fp->ReadUInt8();
			format.type = (ImageFormat::Type)fp->ReadUInt8();
			format.layout = (ImageFormat::Layout)fp->ReadUInt8();
			format.flag = (ImageFormat::Flag)fp->ReadUInt8();
			char type[2];
			type[0] = fp->ReadInt8();
			type[1] = fp->ReadInt8();
			unsigned char level = fp->ReadUInt8();
			unsigned char bpp = fp->ReadUInt8();
			unsigned short w = fp->ReadUInt16();
			unsigned short h = fp->ReadUInt16();
			unsigned int length = fp->ReadUInt32();
			
			if (currFormat.depth == ImageFormat::Unknown)
				if (text->ValidFormat(format))
					currFormat = format;
			
			if (currFormat != format)
				continue;
			
			unsigned char* buffer = (unsigned char*)text->AddImage(GetTarget(type), format, w, h, length, level);
			if (buffer != NULL)
				fp->Read(buffer, length);
		}
	}
	m_Loader->CloseFile();

	return text;
}
CG3DTexture::Target TextureHandler::GetTarget(const char* str)
{
	if (!strcmp(str, "-x")) return CG3DTexture::TARGET_NEG_X;
	else if (!strcmp(str, "-y")) return CG3DTexture::TARGET_NEG_Y;
	else if (!strcmp(str, "-z")) return CG3DTexture::TARGET_NEG_Z;
	else if (!strcmp(str, "+x")) return CG3DTexture::TARGET_POS_X;
	else if (!strcmp(str, "+y")) return CG3DTexture::TARGET_POS_Y;
	else if (!strcmp(str, "+z")) return CG3DTexture::TARGET_POS_Z;
	return CG3DTexture::TARGET_2D;
}

CG3DRefCount* EffectHandler::Load(BinaryFP* fp)
{
	CG3DEffect* effect = m_Loader->m_Graphics->MakeEffect();
	unsigned int numVar = fp->ReadUInt32();
	unsigned int numShdr = fp->ReadUInt32();

	bool gotProgram = false;
	bool gotShader[2] = {false, false};
	CG3DShader* shader[2] = {NULL, NULL};

	fp = m_Loader->OpenFile();
	for (int i=0; i<(int)numShdr; i++)
	{
		if (i>0)
			m_Loader->NextFile();
		if (fp->m_Ident == IDENT('S','H','D','R'))
		{
			Flags flags = (Flags)fp->ReadUInt8();
			unsigned int format;
			unsigned int length;

			if ((flags&Source) != Source)
			{
				format = fp->ReadUInt32();
				length = fp->ReadUInt32();
				
				unsigned char* buffer = NULL;

				if((flags&Shader) == Shader)
				{
					CG3DShader::Type type = ((flags&Vertex)==Vertex)?CG3DShader::VERTEX:CG3DShader::PIXEL;

					if (shader[type] == NULL)
						shader[type] = effect->MakeShader();
					if (shader[type]->ValidFormat(format) && !gotProgram && !gotShader[type])
						buffer = (unsigned char*)shader[type]->MakeBinaryBuffer(type, length, format);
					if (buffer != NULL)
					{
						gotShader[type] = true;
						fp->Read(buffer, length);
					}
				}
				else
				{
					if (effect->ValidProgramFormat(format) && !gotProgram)
					{
						buffer = new unsigned char[length];
						gotProgram = effect->SetProgramBinary(buffer, length, format);
						fp->Read(buffer, length);
						delete[] buffer;
					}
				}
			}
			else
			{
				CG3DShader::Type type = ((flags&Vertex)==Vertex)?CG3DShader::VERTEX:CG3DShader::PIXEL;
				std::string target, entry;

				if (shader[type] == NULL)
					shader[type] = effect->MakeShader();

				if((flags&GLSL) != GLSL)
				{
					target = fp->ReadString();
					entry = fp->ReadString();
				}
				unsigned int num = fp->ReadUInt32();
				for (int i=0; i<(int)num; i++) {
					std::string key = fp->ReadString();
					std::string value = fp->ReadString();
					
					if (!gotProgram && !gotShader[type])
						shader[type]->AddDefine(key.c_str(), value.c_str());
				}
				std::string source = fp->ReadString();

				if (!gotProgram && !gotShader[type])
				{
					if((flags&GLSL) == GLSL)
						gotShader[type] = shader[type]->SetSource(type, source.c_str());
					else
						gotShader[type] = shader[type]->SetSource(target.c_str(), entry.c_str(), source.c_str(), "binary");
				}
			}
		}
	}
	fp = m_Loader->CloseFile();
	
	for (int i=0; i<(int)numVar; i++)
	{
		unsigned char type = fp->ReadUInt8();
		std::string name = fp->ReadString();
		std::string aux = fp->ReadString();

		if (aux.empty())
			effect->AddParameter(name.c_str(), (CG3DEffect::ParameterType)type);
		else
			effect->AddParameter(name.c_str(), (CG3DEffect::ParameterType)type, aux.c_str());
	}

	return effect;
}

CG3DRefCount* MaterialHandler::Load(BinaryFP* fp)
{
	CG3DMaterial* material = NULL;

	CG3DEffect* effect = (CG3DEffect*)m_Loader->GetReference();
	if(effect == NULL) return NULL;

	material = effect->MakeMaterial();
	
	unsigned int num = fp->ReadUInt32();
	
	for (int i=0; i<(int)num; i++)
	{
		float fVars[16];
		int iVars[4];
		CG3DTexture* text;

		std::string name = fp->ReadString();
		CG3DEffect::ParameterType type = (CG3DEffect::ParameterType)fp->ReadUInt8();

		switch(type)
		{
			case CG3DEffect::COMPONENT_1 | CG3DEffect::TYPE_FLOAT:
				fVars[0] = fp->ReadFloat();
				material->SetParameter(name.c_str(), fVars);
                break;
            case CG3DEffect::COMPONENT_2 | CG3DEffect::TYPE_FLOAT:
				fVars[0] = fp->ReadFloat();
				fVars[1] = fp->ReadFloat();
				material->SetParameter(name.c_str(), fVars);
                break;
            case CG3DEffect::COMPONENT_3 | CG3DEffect::TYPE_FLOAT:
				fVars[0] = fp->ReadFloat();
				fVars[1] = fp->ReadFloat();
				fVars[2] = fp->ReadFloat();
				material->SetParameter(name.c_str(), fVars);
                break;
            case CG3DEffect::COMPONENT_4 | CG3DEffect::TYPE_FLOAT:
				fVars[0] = fp->ReadFloat();
				fVars[1] = fp->ReadFloat();
				fVars[2] = fp->ReadFloat();
				fVars[3] = fp->ReadFloat();
				material->SetParameter(name.c_str(), fVars);
                break;
            case CG3DEffect::COMPONENT_1 | CG3DEffect::TYPE_INT:
				iVars[0] = fp->ReadInt32();
				material->SetParameter(name.c_str(), iVars);
                break;
            case CG3DEffect::COMPONENT_2 | CG3DEffect::TYPE_INT:
				iVars[0] = fp->ReadInt32();
				iVars[1] = fp->ReadInt32();
				material->SetParameter(name.c_str(), iVars);
                break;
            case CG3DEffect::COMPONENT_3 | CG3DEffect::TYPE_INT:
				iVars[0] = fp->ReadInt32();
				iVars[1] = fp->ReadInt32();
				iVars[2] = fp->ReadInt32();
				material->SetParameter(name.c_str(), iVars);
                break;
            case CG3DEffect::COMPONENT_4 | CG3DEffect::TYPE_INT:
				iVars[0] = fp->ReadInt32();
				iVars[1] = fp->ReadInt32();
				iVars[2] = fp->ReadInt32();
				iVars[3] = fp->ReadInt32();
				material->SetParameter(name.c_str(), iVars);
                break;
			case CG3DEffect::COMPONENT_2x2 | CG3DEffect::TYPE_MATRIX:
			case CG3DEffect::COMPONENT_2x2 | CG3DEffect::TYPE_TRANSPOSE_MATRIX:
				fVars[0] = fp->ReadFloat();
				fVars[1] = fp->ReadFloat();
				fVars[2] = fp->ReadFloat();
				fVars[3] = fp->ReadFloat();
				material->SetParameter(name.c_str(), fVars);
                break;
			case CG3DEffect::COMPONENT_3x3 | CG3DEffect::TYPE_MATRIX:
			case CG3DEffect::COMPONENT_3x3 | CG3DEffect::TYPE_TRANSPOSE_MATRIX:
				fVars[0] = fp->ReadFloat();
				fVars[1] = fp->ReadFloat();
				fVars[2] = fp->ReadFloat();
				fVars[3] = fp->ReadFloat();
				fVars[4] = fp->ReadFloat();
				fVars[5] = fp->ReadFloat();
				fVars[6] = fp->ReadFloat();
				fVars[7] = fp->ReadFloat();
				fVars[8] = fp->ReadFloat();
				fVars[9] = fp->ReadFloat();
				material->SetParameter(name.c_str(), fVars);
                break;
			case CG3DEffect::COMPONENT_4x4 | CG3DEffect::TYPE_MATRIX:
			case CG3DEffect::COMPONENT_4x4 | CG3DEffect::TYPE_TRANSPOSE_MATRIX:
				fVars[0] = fp->ReadFloat();
				fVars[1] = fp->ReadFloat();
				fVars[2] = fp->ReadFloat();
				fVars[3] = fp->ReadFloat();
				fVars[4] = fp->ReadFloat();
				fVars[5] = fp->ReadFloat();
				fVars[6] = fp->ReadFloat();
				fVars[7] = fp->ReadFloat();
				fVars[8] = fp->ReadFloat();
				fVars[9] = fp->ReadFloat();
				fVars[10] = fp->ReadFloat();
				fVars[11] = fp->ReadFloat();
				fVars[12] = fp->ReadFloat();
				fVars[13] = fp->ReadFloat();
				fVars[14] = fp->ReadFloat();
				fVars[15] = fp->ReadFloat();
				material->SetParameter(name.c_str(), fVars);
                break;
			case CG3DEffect::COMPONENT_1 | CG3DEffect::TYPE_TEXTURE:
				text = (CG3DTexture*)m_Loader->GetReference();
				material->SetTexture(name.c_str(), text);
				break;
		}
	}

	return material;
}

CG3DRefCount* ModelHandler::Load(BinaryFP* fp)
{
	CG3DMesh* mesh = new CG3DMesh;
	unsigned char mapSize = fp->ReadUInt8();
	unsigned char buffers = fp->ReadUInt8();
	
	unsigned char* map = new unsigned char[mapSize];
	fp->Read(map, mapSize);

	mesh->SetMap(map, mapSize);
	delete[] map;
	
	fp = m_Loader->OpenFile();
	for (int i=0; i<buffers; i++)
	{
		if (i>0)
			m_Loader->NextFile();
		if (fp->m_Ident == IDENT('V','E','R','T'))
		{
			CG3DVertexBuffer* buffer = m_Loader->m_Graphics->MakeVertexBuffer();
			
			unsigned int size = fp->ReadUInt32();
			unsigned int stride = fp->ReadUInt32();
			unsigned int num = fp->ReadUInt32();

			unsigned char* data = (unsigned char*)buffer->CreateBuffer(size, stride);
			fp->Read(data, stride*size); //TODO endian?

			for(int j=0; j<(int)num; j++)
			{
				std::string name = fp->ReadString();
				CG3DVertexBuffer::ElementType type = (CG3DVertexBuffer::ElementType)fp->ReadUInt32();
				unsigned int offset = fp->ReadUInt32();

				buffer->AddElement(name.c_str(), type, offset);
			}
			mesh->Add(buffer);
		}
		else if (fp->m_Ident == IDENT('I','N','D','X'))
		{
			CG3DIndexBuffer* buffer = m_Loader->m_Graphics->MakeIndexBuffer();
			
			unsigned int size = fp->ReadUInt32();
			unsigned int stride = fp->ReadUInt32();
			unsigned char type = fp->ReadUInt8();
			
			buffer->Setup((CG3DIndexBuffer::Type)type);
			
			unsigned char* data = (unsigned char*)buffer->CreateBuffer(size, stride);
			fp->Read(data, stride*size); //TODO endian?
			
			mesh->Add(buffer);
		}
	}
	m_Loader->CloseFile();

	return mesh;
}

CG3DRefCount* SceneHandler::Load(BinaryFP* fp)
{
	return m_Loader->GetModule();
}

CG3DRefCount* SimpleSceneHandler::Load(BinaryFP* fp)
{
	return new CG3DSimpleScene();
}

CG3DRefCount* GOBHandler::Load(BinaryFP* fp)
{
	CG3DGameObject* gob = new CG3DGameObject;
	unsigned int numChildren = fp->ReadUInt32();
	unsigned int numModules = fp->ReadUInt32();

	for (int i=0; i<(int)numChildren; i++) {
		 CG3DGameObject* child = (CG3DGameObject*)m_Loader->GetReference();
		 if (child == NULL) continue;

		 gob->AddChild(child);
	}
	
	fp = m_Loader->OpenFile();
	for (int i=0; i<(int)numModules; i++)
	{
		if (i>0)
			m_Loader->NextFile();
		
		if (fp->m_Ident != IDENT('M','O','D','L'))
			continue;

		std::string name = fp->ReadString();
		bool enable = fp->ReadBool();
		
		CG3DModule* module = (CG3DModule*)m_Loader->GetModule();
		if (module != NULL)
			gob->AddModule(name.c_str(), module, enable);
	}
	m_Loader->CloseFile();

	return gob;
}

CG3DRefCount* DummyModuleHandler::Load(BinaryFP* fp)
{
	return new CG3DDummyModule();
}

CG3DRefCount* ModelModuleHandler::Load(BinaryFP* fp)
{
	CG3DScene* scene = (CG3DScene*)m_Loader->GetReference();
	CG3DMesh* mesh = (CG3DMesh*)m_Loader->GetReference();

	unsigned int numMaterials = fp->ReadUInt32();

	CG3DMaterial** mats = new CG3DMaterial*[numMaterials];
	for (int i=0; i<(int)numMaterials; i++)
		mats[i] = (CG3DMaterial*)m_Loader->GetReference();

	CG3DModelModule* module = new CG3DModelModule(scene, mesh, mats);

	delete[] mats;
	return module;
}

CG3DRefCount* CameraModuleHandler::Load(BinaryFP* fp)
{
	CG3DScene* scene = (CG3DScene*)m_Loader->GetReference();
	CG3DEffect* effect = (CG3DEffect*)m_Loader->GetReference();

	bool isOrtho = fp->ReadBool();
	float values[4];
	values[0] = fp->ReadFloat();
	values[1] = fp->ReadFloat();
	values[2] = fp->ReadFloat();
	values[3] = fp->ReadFloat();

	CG3DMatrix mat;
	if (isOrtho)
		mat = CG3DMatrix::CreateOrthographic(values[0], values[1], values[2], values[3]);
	else
		mat = CG3DMatrix::CreatePerspectiveFOV(values[0], values[1], values[2], values[3]);

	return new CG3DCameraModule(scene, effect, mat);
}

CG3DRefCount* FrameMModuleHandler::Load(BinaryFP* fp)
{
	CG3DMatrix mat;
	mat.m11 = fp->ReadFloat();
	mat.m12 = fp->ReadFloat();
	mat.m13 = fp->ReadFloat();
	mat.m14 = fp->ReadFloat();
	mat.m21 = fp->ReadFloat();
	mat.m22 = fp->ReadFloat();
	mat.m23 = fp->ReadFloat();
	mat.m24 = fp->ReadFloat();
	mat.m31 = fp->ReadFloat();
	mat.m32 = fp->ReadFloat();
	mat.m33 = fp->ReadFloat();
	mat.m34 = fp->ReadFloat();
	mat.m41 = fp->ReadFloat();
	mat.m42 = fp->ReadFloat();
	mat.m43 = fp->ReadFloat();
	mat.m44 = fp->ReadFloat();

	return new CG3DFrameModuleM(mat);
}

CG3DRefCount* FramePRSModuleHandler::Load(BinaryFP* fp)
{
	CG3DVector3 pos;
	CG3DVector3 rot;
	CG3DVector3 scale;

	pos.x = fp->ReadFloat();
	pos.y = fp->ReadFloat();
	pos.z = fp->ReadFloat();
	rot.x = fp->ReadFloat();
	rot.y = fp->ReadFloat();
	rot.z = fp->ReadFloat();
	scale.x = fp->ReadFloat();
	scale.y = fp->ReadFloat();
	scale.z = fp->ReadFloat();

	return new CG3DFrameModulePRS(pos, rot, scale);
}

CG3DRefCount* FramePQSModuleHandler::Load(BinaryFP* fp)
{
	CG3DVector3 pos;
	CG3DQuaternion rot;
	CG3DVector3 scale;
	
	pos.x = fp->ReadFloat();
	pos.y = fp->ReadFloat();
	pos.z = fp->ReadFloat();
	rot.x = fp->ReadFloat();
	rot.y = fp->ReadFloat();
	rot.z = fp->ReadFloat();
	rot.w = fp->ReadFloat();
	scale.x = fp->ReadFloat();
	scale.y = fp->ReadFloat();
	scale.z = fp->ReadFloat();

	return new CG3DFrameModulePQS(pos, rot, scale);
}

CG3DRefCount* FrameIdentModuleHandler::Load(BinaryFP* fp)
{
	return new CG3DFrameModuleIdentity;
}