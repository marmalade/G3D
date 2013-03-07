#define _CRT_SECURE_NO_WARNINGS

#include "EffectLib.h"
#include <stdio.h>
#include <string.h>

struct Mapping {
	const char* m_Name;
	CG3DEffect::ParameterType m_Type;
};

Mapping s_Mapping[] = {
	{"float1", PARAMETER_TYPE(TYPE_FLOAT, COMPONENT_1)},
	{"float2", PARAMETER_TYPE(TYPE_FLOAT, COMPONENT_2)},
	{"float3", PARAMETER_TYPE(TYPE_FLOAT, COMPONENT_3)},
	{"float4", PARAMETER_TYPE(TYPE_FLOAT, COMPONENT_4)},

	{"int1", PARAMETER_TYPE(TYPE_INT, COMPONENT_1)},
	{"int2", PARAMETER_TYPE(TYPE_INT, COMPONENT_2)},
	{"int3", PARAMETER_TYPE(TYPE_INT, COMPONENT_3)},
	{"int4", PARAMETER_TYPE(TYPE_INT, COMPONENT_4)},

	{"matrix2x2", PARAMETER_TYPE(TYPE_MATRIX, COMPONENT_2x2)},
	{"matrix3x3", PARAMETER_TYPE(TYPE_MATRIX, COMPONENT_3x3)},
	{"matrix4x4", PARAMETER_TYPE(TYPE_MATRIX, COMPONENT_4x4)},

	{"tmatrix2x2", PARAMETER_TYPE(TYPE_TRANSPOSE_MATRIX, COMPONENT_2x2)},
	{"tmatrix3x3", PARAMETER_TYPE(TYPE_TRANSPOSE_MATRIX, COMPONENT_3x3)},
	{"tmatrix4x4", PARAMETER_TYPE(TYPE_TRANSPOSE_MATRIX, COMPONENT_4x4)},
	
	{"texture", PARAMETER_TYPE(TYPE_TEXTURE, COMPONENT_1)},
	{NULL, PARAMETER_TYPE(TYPE_TEXTURE, COMPONENT_1)}
};

CG3DEffectLib::CG3DEffectLib(CG3DGraphics* graphics) : m_Graphics(graphics), m_Data(NULL), m_Length(0)
{
}

void CG3DEffectLib::LoadText(const char* filename)
{
	char directory[256];
	strcpy(directory, filename);
	char* ptr = strrchr(directory, '\\');
	if (ptr == NULL) ptr = strrchr(directory, '/');
	if (ptr != NULL) ptr[1] = NULL;
	else directory[0] = NULL;

	FILE* fp = fopen(filename, "r");
	if (fp == NULL) return;

	CG3DEffect* effect = NULL;
	CG3DShader* shader[CG3DShader::MAX];
	bool done[CG3DShader::MAX];
	bool loadedProgram = false;

	while (!feof(fp))
	{
		char line[1024];
		if (fgets(line, 1024, fp) == NULL)
			break;

		while (strlen(line) > 0 && line[strlen(line)-1] <= 32)
			line[strlen(line)-1] = 0;
		
		if (!strncmp(line, "effect ",7))
		{
			char name[256];
			sscanf(line, "effect %s", name);

			effect = m_Graphics->MakeEffect();
			effect->AddRef();
			m_Effects[name] = effect;

			loadedProgram = false;
			for (int i=0; i<CG3DShader::MAX; i++)
			{
				shader[i] = NULL;
				done[i] = false;
			}
		}
		else if (!strncmp(line, "program ",8))
		{
			char file[256];
			unsigned int format;
			unsigned int length;
			strcpy(file, directory);
			sscanf(line, "program %x %s %d", &format, file + strlen(file), &length);

			if (!strcmp(file + strlen(directory), "inline"))
				LoadFileBinary(fp, length);
			else
				LoadFile(file);

			if (!loadedProgram  && effect->ValidProgramFormat(format))
				loadedProgram = LoadProgram(effect, format);
			
			delete m_Data;
			m_Data = NULL;
		}
		else if (!strncmp(line, "glsl ",5))
		{
			char typeStr[256];
			char file[256];
			unsigned int length;
			strcpy(file, directory);
			sscanf(line, "glsl %s %s %d", typeStr, file + strlen(file), &length);
			
			CG3DShader::Type type = CG3DShader::VERTEX;
			if (!strcmp("pixel", typeStr))
				type = CG3DShader::PIXEL;
			else
				type = CG3DShader::VERTEX;

			if (shader[type] == NULL)
			{
				shader[type] = effect->MakeShader();
			}
			
			if (!strcmp(file + strlen(directory), "inline"))
				LoadFileText(fp, length);
			else
				LoadFile(file);

			if (!done[type] && !loadedProgram)
				done[type] = LoadShader(shader[type], type);
			
			delete m_Data;
			m_Data = NULL;
		}
		else if (!strncmp(line, "hlsl ",5))
		{
			char typeStr[256];
			char entry[256];
			char file[256];
			unsigned int length;
			strcpy(file, directory);
			sscanf(line, "hlsl %s %s %s %d", typeStr, entry, file + strlen(file), &length);
			
			CG3DShader::Type type = CG3DShader::VERTEX;
			if (typeStr[0] == 'p')
				type = CG3DShader::PIXEL;
			else
				type = CG3DShader::VERTEX;

			if (shader[type] == NULL)
			{
				shader[type] = effect->MakeShader();
			}
			
			if (!strcmp(file + strlen(directory), "inline"))
				LoadFileText(fp, length);
			else
				LoadFile(file);

			if (!done[type] && !loadedProgram)
				done[type] = LoadShader(shader[type], file, typeStr, entry);
			
			delete m_Data;
			m_Data = NULL;
		}
		else if (!strncmp(line, "binary ",7))
		{
			char typeStr[256];
			char file[256];
			unsigned int format;
			unsigned int length;
			strcpy(file, directory);
			sscanf(line, "binary %s %x %s %d", &typeStr, &format, file + strlen(file), &length);
			
			CG3DShader::Type type = CG3DShader::VERTEX;
			if (!strcmp("pixel", typeStr))
				type = CG3DShader::PIXEL;
			else
				type = CG3DShader::VERTEX;

			if (shader[type] == NULL)
			{
				shader[type] = effect->MakeShader();
			}

			if (!strcmp(file + strlen(directory), "inline"))
				LoadFileBinary(fp, length);
			else
				LoadFile(file);

			if (!done[type] && !loadedProgram && shader[type]->ValidFormat(format))
				done[type] = LoadBinaryShader(shader[type], type, format);
			
			delete m_Data;
			m_Data = NULL;
		}
		else if (!strncmp(line, "define ",7))
		{
			char typeStr[256];
			char name[256];
			char value[256] = "";
			sscanf(line, "define %s %s %s", typeStr, name, value);

			if (loadedProgram) continue;
			
			CG3DShader::Type type = CG3DShader::VERTEX;
			if (!strcmp("pixel", typeStr))
				type = CG3DShader::PIXEL;
			else
				type = CG3DShader::VERTEX;

			if (shader[type] == NULL)
			{
				shader[type] = effect->MakeShader();
			}

			shader[type]->AddDefine(name, value);
		}
		else if (!strncmp(line, "dynamic ",8))
		{
			char name[256];
			char type[256];
			char aux[256] = "";
			sscanf(line, "dynamic %s %s %s", name, type, aux);

			for (int i=0; s_Mapping[i].m_Name!=NULL; i++)
				if (!strcmp(s_Mapping[i].m_Name, type))
				{
					effect->AddParameter(name, s_Mapping[i].m_Type, aux);
					break;
				}
		}
		else if (!strncmp(line, "static ",7))
		{
			char name[256];
			char type[256];
			char aux[256] = "";
			sscanf(line, "static %s %s %s", name, type, aux);

			for (int i=0; s_Mapping[i].m_Name!=NULL; i++)
				if (!strcmp(s_Mapping[i].m_Name, type))
				{
					effect->AddParameter(name, s_Mapping[i].m_Type, aux);
					break;
				}
		}
	}
	fclose(fp);
}

void CG3DEffectLib::LoadFile(const char* filename)
{
	m_Data = NULL;

	FILE* fp = fopen(filename, "r");
	if (fp == NULL) return;

	fseek(fp, 0, SEEK_END);
	m_Length = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	m_Data = new unsigned char[m_Length+1];
	fread(m_Data, m_Length, 1,fp);
	m_Data[m_Length]=0;

	fclose(fp);
}

void CG3DEffectLib::LoadFileBinary(FILE* fp, unsigned int len)
{
	m_Length = len;

	unsigned int count=0;
	m_Data = new unsigned char[len+1];
	m_Data[len]=0;

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
			m_Data[count++] = byte;
			ptr+=num;
			sscanf(ptr, " %n%02x%n", &num2, &byte, &num);
		}
	}
}

void CG3DEffectLib::LoadFileText(FILE* fp, unsigned int len)
{
	std::string store;
	
	for (unsigned int count=0;!feof(fp)  && count<len; count++)
	{
		char line[1024];
		if (fgets(line, 1024, fp) == NULL)
			break;

		store+=line;
	}
	m_Length = store.size();
	m_Data = new unsigned char[m_Length + 1];
	strcpy((char*)m_Data, store.c_str());
	m_Data[m_Length] = 0;
}

bool CG3DEffectLib::LoadShader(CG3DShader* shader, CG3DShader::Type type)
{
	if(m_Data == NULL) return false;

	return shader->SetSource(type, (char*)m_Data);
}

bool CG3DEffectLib::LoadBinaryShader(CG3DShader* shader, CG3DShader::Type type, unsigned int format)
{
	if(m_Data == NULL) return false;

	void* buffer = shader->MakeBinaryBuffer(type, m_Length, format);
	if (buffer == NULL)
		return false;

	memcpy(buffer,m_Data, m_Length);

	return true;
}

bool CG3DEffectLib::LoadShader(CG3DShader* shader, const char* filename, const char* type, const char* entry)
{
	if(m_Data == NULL) return false;

	return shader->SetSource(type, entry, (char*)m_Data, filename);
}

bool CG3DEffectLib::LoadProgram(CG3DEffect* effect, unsigned int format)
{
	if(m_Data == NULL) return false;

	return effect->SetProgramBinary(m_Data, m_Length, format);
}

CG3DMaterial* CG3DEffectLib::MakeMaterial(const char* name)
{
	if (m_Effects.find(name) == m_Effects.end())
		return NULL;
	return m_Effects[name]->MakeMaterial();
}

CG3DMaterial* CG3DEffectLib::MakeSubMaterial(const char* name, const char* buffer)
{
	if (m_Effects.find(name) == m_Effects.end())
		return NULL;
	return m_Effects[name]->MakeSubMaterial(buffer);
}

void CG3DEffectLib::Release()
{
	std::map<std::string, CG3DEffect*>::iterator it;
	for (it = m_Effects.begin(); it != m_Effects.end(); ++it)
		it->second->Release();
}