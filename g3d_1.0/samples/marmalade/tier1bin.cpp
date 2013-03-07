#include "G3DGLES.h"
#include "G3DCore.h"
#include "G3DMarmalade.h"
#include "G3DMaths.h"
#include "G3DHelper.h"
#include "G3DBinary.h"

//encapsulate the games loop
class CGame : public CG3DApp
{
public:
	CGame(CG3DSystem* system) : CG3DApp(system), m_Angle(0) {}
protected:
	virtual void InitInternal()
	{
		//standard global state
		m_Graphics->SetGlobalSet(CG3DGlobalSet::COLOUR, new CG3DColourSet(CG3DColour(0, 255, 0)));
		m_Graphics->SetGlobalSet(CG3DGlobalSet::CULL, new CG3DCullSet(CG3DCullSet::CULL_BACK));
		m_Graphics->SetGlobalSet(CG3DGlobalSet::DEPTH, new CG3DDepthSet(1));

		//load the binary file
		CG3DPtr<CG3DBinaryLoader> binary = new CG3DBinaryLoader(m_Graphics.get());
		binary->InitFile("eva.g3d");

		//get an effect from the file
		CG3DPtr<CG3DEffect> effect = binary->GetData<CG3DEffect>("effects.simple");
		m_Camera = new CG3DCamera(effect.get());
		
		//get a mesh from the file
		CG3DPtr<CG3DMesh> mesh = binary->GetData<CG3DMesh>("eva.combatEvaShape");
		
		//get materials from the file
		CG3DMaterial* mats[3];
		mats[0] = binary->GetData<CG3DMaterial>("eva.face_simple");
		mats[1] = binary->GetData<CG3DMaterial>("eva.body_simple");
		mats[2] = binary->GetData<CG3DMaterial>("eva.hair_simple");

		//make a model from the mesh and materials
		m_Model = mesh->MakeInstance(mats);

		//set up a camera
		CG3DVector2 size = m_System->GetSize();
		CG3DMatrix persp = CG3DMatrix::CreatePerspectiveFOV((float)M_PI/2.0f, size.x/size.y, 0.01f, 100.0f);
		CG3DMatrix view = CG3DMatrix::CreateTranslation(CG3DVector3(0,-0.5f,0.25f));

		m_Camera->SetView(view);
		m_Camera->SetProj(persp);

		//upload the model to the gpu
		m_Model->Upload();

		m_Angle=0;
	}
	virtual void ShutdownInternal()
	{
	}
	virtual bool UpdateInternal(float timeStep)
	{
		//update the models rotation
		m_Model->SetWorld(CG3DMatrix::CreateRotationEuler(0, m_Angle, 0));
		m_Angle+=(float)M_PI*40.0f*timeStep/180.0f;

		return true;
	}
	virtual void RenderInternal(float timeStep)
	{
		//render the camera and the model
		m_Camera->Apply();
		m_Model->Apply();
	}
private:
	CG3DPtr<CG3DModel> m_Model;
	float m_Angle;
	CG3DPtr<CG3DCamera> m_Camera;
};

int main()
{
	//configure the screen and graphics
	CG3DSystemConfigIW config;
	//config.m_GLOnly = false;
	CGame game(MakeG3DSystemMarmalade(&config));

	game.Run();
}