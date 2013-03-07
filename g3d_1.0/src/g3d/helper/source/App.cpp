#include "App.h"

void CG3DApp::Run()
{
	Init();
	while (RunOne());
	Shutdown();
}

void CG3DApp::Init()
{
	m_Graphics = m_System->Init();
	
	m_Size = m_System->GetSize();
	m_Viewport = new CG3DViewportSet(CG3DVector4(0, 0, m_Size.x, m_Size.y));
	m_Graphics->SetGlobalSet(CG3DGlobalSet::VIEWPORT, m_Viewport.get());

	InitInternal();

	m_LastTime = m_System->GetTime();
}

void CG3DApp::Shutdown()
{
	ShutdownInternal();
}

bool CG3DApp::RunOne()
{
	long long time = m_System->GetTime();
	float delta = (float)(time - m_LastTime) / 1000.0f;

	if (!Update(delta))
		return false;

	Render(delta);

	m_LastTime = time;
	return true;
}

bool CG3DApp::Update(float timeStep)
{
	if (!m_System->Update())
		return false;

	return UpdateInternal(timeStep);
}

void CG3DApp::SizeChanged(CG3DVector2 size)
{
	m_Viewport->m_Rect = CG3DVector4(0, 0, size.x, size.y);
	m_Graphics->ApplySet(CG3DGlobalSet::VIEWPORT);
}

void CG3DApp::Render(float timeStep)
{
	CG3DVector2 size = m_System->GetSize();
	if (m_Size.x != size.x || m_Size.y != size.y)
	{
		m_Size = size;
		SizeChanged(size);
	}

	m_Graphics->Clear();

	RenderInternal(timeStep);

	m_System->EndRender();
}