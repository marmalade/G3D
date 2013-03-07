#define _CRT_SECURE_NO_WARNINGS

#include "Mesh.h"
#include <stdio.h>
#include <string.h>

void CG3DMesh::LoadText(const char* filename, CG3DGraphics* graphics)
{
	FILE* fp = fopen(filename, "r");
	if (fp == NULL) return;

	CG3DVertexBuffer* vb = NULL;
	CG3DIndexBuffer* ib = NULL;

	while (!feof(fp))
	{
		char line[1024];
		if (fgets(line, 1024, fp) == NULL)
			break;

		while (strlen(line) > 0 && line[strlen(line)-1] <= 32)
			line[strlen(line)-1] = 0;
		
		if (!strncmp(line, "material ",9))
		{
			int num;
			sscanf(line, "material %d", &num);
			m_Map.push_back((unsigned char)num | MAP_MATERIAL);
		}
		else if (!strncmp(line, "vb ",3))
		{
			int a, b;
			sscanf(line, "vb %d %d", &a, &b);
			vb = graphics->MakeVertexBuffer();
			vb->CreateBuffer(a, b);
			m_Map.push_back((unsigned char)m_List.size());
			Add(vb);
		}
		else if (!strncmp(line, "v ",2))
		{
			if (vb == NULL) continue;

			float value;
			int offset = 1, num;
			while (line[offset] != 0)
			{
				if (!sscanf(line + offset, " %f%n", &value, &num))
					break;
				offset += num;
				vb->Add(value);
			}
		}
		else 
		if (!strncmp(line, "ve ",3))
		{
			if (vb == NULL) continue;

			char name[64];
			char typeStr[64];
			int num, size, offset;
			sscanf(line, "ve %s %d %d %s %d", name, &num, &size, typeStr, &offset);
			int type = 0;
			switch(num) {
				case 1: type |= CG3DVertexBuffer::COMPONENT_1; break;
				case 2: type |= CG3DVertexBuffer::COMPONENT_2; break;
				case 3: type |= CG3DVertexBuffer::COMPONENT_3; break;
				default: type |= CG3DVertexBuffer::COMPONENT_4; break;
			}
			switch(size) {
				case 8: type |= CG3DVertexBuffer::SIZE_8; break;
				case 16: type |= CG3DVertexBuffer::SIZE_16; break;
				default: type |= CG3DVertexBuffer::SIZE_32; break;
			}
			if (!strcmp(typeStr, "unsigned")) type |= CG3DVertexBuffer::TYPE_UNSIGNED;
			else if (!strcmp(typeStr, "signed")) type |= CG3DVertexBuffer::TYPE_SIGNED;
			else type |= CG3DVertexBuffer::TYPE_FLOAT;

			vb->AddElement(name, (CG3DVertexBuffer::ElementType)type, offset);
		}
		else if (!strncmp(line, "ib ",3))
		{
			int a, b;
			sscanf(line, "ib %d %d", &a, &b);
			ib = graphics->MakeIndexBuffer();
			ib->CreateBuffer(a, b);
			m_Map.push_back((unsigned char)m_List.size());
			Add(ib);
		}
		else if (!strncmp(line, "i ",2))
		{
			if (ib == NULL) continue;

			unsigned short value;
			int offset = 1, num;
			while (line[offset] != 0)
			{
				if (!sscanf(line + offset, " %hd%n", &value, &num))
					break;
				offset += num;
				ib->Add(value);
			}
		}
		else if (!strncmp(line, "is ",3))
		{
			if (ib == NULL) continue;

			char typeStr[64];
			int size;
			sscanf(line, "is %s %d", typeStr, &size);
			int type = 0;
			if (!strcmp(typeStr, "Lines")) type |= CG3DIndexBuffer::LINES;
			else if (!strcmp(typeStr, "LineStrips")) type |= CG3DIndexBuffer::LINESTRIP;
			else if (!strcmp(typeStr, "TriFans")) type |= CG3DIndexBuffer::TRIANGLEFAN;
			else if (!strcmp(typeStr, "TriStrip")) type |= CG3DIndexBuffer::TRIANGLESTRIP;
			else type |= CG3DIndexBuffer::TRIANGLES;
			switch(size) {
				case 8: type |= CG3DIndexBuffer::SIZE_8; break;
				case 16: type |= CG3DIndexBuffer::SIZE_16; break;
				default: type |= CG3DIndexBuffer::SIZE_32; break;
			}

			ib->Setup((CG3DIndexBuffer::Type)type);
		}
	}
	fclose(fp);
}

void CG3DMesh::AddSubModel(int material, CG3DVertexBuffer* vertex, CG3DIndexBuffer* index)
{
	m_Map.push_back((unsigned char)material | MAP_MATERIAL);
	m_Map.push_back((unsigned char)m_List.size());
	Add(vertex);
	m_Map.push_back((unsigned char)m_List.size());
	Add(index);
}

void CG3DMesh::AddSubModel(int material, int* parts, int numParts)
{
	m_Map.push_back((unsigned char)material | MAP_MATERIAL);

	for (int i=0; i<numParts; i++)
		m_Map.push_back((unsigned char)parts[i]);
}

void CG3DMesh::SetMap(unsigned char* parts, int numParts)
{
	for (int i=0; i<numParts; i++)
		m_Map.push_back((unsigned char)parts[i]);
}

int CG3DMesh::NumSubModels()
{
	int numSubModels = 0;

	for (int i=0; i<(int)m_Map.size(); i++)
		if (m_Map[i] & MAP_MATERIAL)
			numSubModels++;

	return numSubModels;
}

void CG3DMesh::MakeSubModels(CG3DPositioner* pos, CG3DMaterial** materials, CG3DSubModel** subModels)
{
	CG3DSubModel* sub = NULL;
	int numSub = 0;
	
	for (int i=0; i<(int)m_Map.size(); i++)
		if (m_Map[i] & MAP_MATERIAL)
		{
			sub = new CG3DSubModel(pos);
			sub->Add(materials[m_Map[i] & MAP_MASK]);
			subModels[numSub++] = sub;
		}
		else if (sub != NULL)
		{
			sub->Add(m_List[m_Map[i]]);
		}
}

CG3DModel* CG3DMesh::MakeInstance(CG3DMaterial** materials)
{
	CG3DModel* inst = new CG3DModel(materials[0]->GetEffect());
	CG3DSubModel* sub = NULL;
	
	for (int i=0; i<(int)m_Map.size(); i++)
		if (m_Map[i] & MAP_MATERIAL)
		{
			sub = new CG3DSubModel(inst->m_Pos.get());
			sub->Add(materials[m_Map[i] & MAP_MASK]);
			inst->Add(sub);
		}
		else if (sub != NULL)
		{
			sub->Add(m_List[m_Map[i]]);
		}

	return inst;
}