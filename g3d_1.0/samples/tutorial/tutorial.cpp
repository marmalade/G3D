#include "G3DCore.h"
#include "G3DMarmalade.h"
#include "G3DHelper.h"
#include "G3DBinary.h"
#include "G3DHierarchy.h"

//Control module that rotates the game object
class CMyFirstModule : public CG3DUpdateModule
{
public:
	CMyFirstModule() : CG3DUpdateModule(0), m_Angle(0), m_Speed(1) {}
	virtual void DoUpdate(float timeStep) {
		CG3DVector3* rot = m_Parent->GetData<CG3DVector3>("frame.rot");
		rot->y = m_Angle;

		m_Angle+=(float)M_PI*40.0f*timeStep*m_Speed/180.0f;
	}
public:
	float m_Speed;
	float m_Angle;
};

//load the control module from the file
class ControlModuleHandler : public CG3DBinaryHandler {
public:
	virtual CG3DRefCount* Load(BinaryFP* fp) {
		CMyFirstModule* module = new CMyFirstModule;
		module->m_Speed = fp->ReadFloat();
		return module;
	}
};

//encapsulates the game loop
class CGame : public CG3DApp
{
public:
	CGame() : CG3DApp(MakeG3DSystemMarmalade()) {}
protected:
	virtual void InitInternal() {
		//standard global state sets
		m_Graphics->SetGlobalSet(CG3DGlobalSet::COLOUR, new CG3DColourSet(CG3DColour(0, 255, 0)));
		m_Graphics->SetGlobalSet(CG3DGlobalSet::DEPTH, new CG3DDepthSet(1));
		m_Graphics->SetGlobalSet(CG3DGlobalSet::CULL, new CG3DCullSet(CG3DCullSet::CULL_BACK));

		//load the binary file
		CG3DPtr<CG3DBinaryLoader> binary = new CG3DBinaryLoader(m_Graphics.get());
		binary->InitFile("test.g3d");

		//add the AI module loader
		binary->AddModuleHandler("control",new ControlModuleHandler());
		
		//fetch the scene and game object hierarchy from the file
		m_Scene = binary->GetData<CG3DScene>("test.testScene");
		m_Hier = binary->GetData<CG3DGameObject>("test.root");
		
		//upload to the gpu
		m_Scene->Upload();
	}
	virtual bool UpdateInternal(float timeStep)
	{
		//update the frame
		m_Hier->DoFrame(timeStep);
		return true;
	}
	virtual void RenderInternal(float timeStep)
	{
		//render the scene
		m_Scene->Apply();
	}
	virtual void ShutdownInternal() { }
private:
	CG3DPtr<CG3DGameObject> m_Hier;
	CG3DPtr<CG3DScene> m_Scene;
};

int main()
{
	//init and run the game loop
	CGame game;
	game.Run();
}