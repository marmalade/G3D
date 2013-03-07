#include "Scene.h"

void CG3DScene::Add(CG3DSubModel** subModels, int num)
{
	for (int i=0; i<num; i++)
		Add(subModels[i]);
}

void CG3DScene::Remove(CG3DSubModel** subModels, int num)
{
	for (int i=0; i<num; i++)
		Remove(subModels[i]);
}

void CG3DScene::UploadInternal()
{
	Traverse(&CG3DResource::Upload);
}

void CG3DScene::ReloadInternal()
{
	Traverse(&CG3DResource::Reload);
}

void CG3DScene::UnloadInternal()
{
	Traverse(&CG3DResource::Unload);
}

void CG3DScene::ReleaseInternal()
{
	Traverse(&CG3DResource::Release);
}

void CG3DSimpleScene::Add(CG3DSubModel* subModel)
{
	Map::iterator it = m_Map.find(subModel->m_Material);
	if (it == m_Map.end())
	{
		m_Map[subModel->m_Material]=std::vector<CG3DSubModel*>();
		it = m_Map.find(subModel->m_Material);
	}
	it->second.push_back(subModel);
	subModel->AddRef();

	if(IsUploaded() && !subModel->IsUploaded())
		subModel->Upload();
}

void CG3DSimpleScene::Remove(CG3DSubModel* subModel)
{
	Map::iterator it = m_Map.find(subModel->m_Material);
	if (it == m_Map.end())
		return;

	if(!IsUploaded() && subModel->IsUploaded())
		subModel->Unload();

	for (int i=0; i<(int)it->second.size(); i++)
		if (it->second[i] == subModel)
		{
			it->second.erase(it->second.begin() + i);
			if(it->second.empty())
				m_Map.erase(it);
			subModel->Release();
			return;
		}
}

void CG3DSimpleScene::Traverse(SceneFunction function)
{
	if (m_Camera != NULL)
		(m_Camera->*function)();

	for (Map::iterator it = m_Map.begin(); it != m_Map.end(); ++it)
		for (int i=0; i<(int)it->second.size(); i++)
			(it->second[i]->*function)();
}

void CG3DSimpleScene::Set(CG3DCamera* camera)
{
	if (m_Camera != NULL)
		m_Camera->Release();
	m_Camera = camera;
	if (camera != NULL)
		camera->AddRef();
}

void CG3DSimpleScene::Apply()
{
	if (m_Camera != NULL)
		m_Camera->Apply();

	for (Map::iterator it = m_Map.begin(); it != m_Map.end(); ++it)
	{
		CG3DMaterial* mat = it->first;
		mat->Apply();

		for (int i=0; i<(int)it->second.size(); i++)
		{
			if (it->second[i]->m_Pos != m_CurrPos)
			{
				m_CurrPos = it->second[i]->m_Pos;
				m_CurrPos->Apply();
			}
			it->second[i]->ApplyMesh();
		}

		mat->DoneApply();
	}
}