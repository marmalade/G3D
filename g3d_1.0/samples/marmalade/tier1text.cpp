#include "G3DCore.h"
#include "G3DMarmalade.h"
#include "G3DMaths.h"
#include "G3DHelper.h"

//encapsulate the game loop
class CGame : public CG3DApp
{
public:
	CGame() : CG3DApp(MakeG3DSystemMarmalade()), m_Angle(0) {}
protected:
	virtual void InitInternal()
	{
		//standard global state
		m_Graphics->SetGlobalSet(CG3DGlobalSet::COLOUR, new CG3DColourSet(CG3DColour(0, 255, 0)));
		m_Graphics->SetGlobalSet(CG3DGlobalSet::CULL, new CG3DCullSet(CG3DCullSet::CULL_BACK));
		m_Graphics->SetGlobalSet(CG3DGlobalSet::DEPTH, new CG3DDepthSet(1));

		//load the effects from a text file
		CG3DPtr<CG3DEffectLib> effectLib = new CG3DEffectLib(m_Graphics.get());
		effectLib->LoadText("eva\\eva.g3d.effect");
		
		//make a sub material to set the camera on
		m_ConstantBuffer = effectLib->MakeSubMaterial("simple", "ConstantBuffer");
		
		//load the mesh from a text file
		CG3DPtr<CG3DMesh> mesh = new CG3DMesh;
		mesh->LoadText("eva\\combatEvaShape.g3d.model", m_Graphics.get());

		//load the materials from a text file
		CG3DMaterialLib* matLib = new CG3DMaterialLib(effectLib.get());
		matLib->LoadText("eva\\eva.g3d.material");
		
		CG3DMaterial* mats[3];
		mats[0] = matLib->GetMaterial("face_simple");
		mats[1] = matLib->GetMaterial("body_simple");
		mats[2] = matLib->GetMaterial("hair_simple");

		//make a model from the mesh and the materials
		m_Model = mesh->MakeInstance(mats);

		//set up the camera variables
		CG3DVector2 size=m_System->GetSize();
		m_ConstantBuffer->SetParameter("Proj", CG3DMatrix::CreatePerspectiveFOV((float)M_PI/2.0f, size.x/size.y, 0.01f, 100.0f));
		m_ConstantBuffer->SetParameter("View", CG3DMatrix::CreateTranslation(0,-0.5f,0.25f));

		//upload the model to the gpu
		m_Model->Upload();

		m_Angle=0;
	}
	virtual void ShutdownInternal()
	{
	}
	virtual bool UpdateInternal(float timeStep)
	{
		//set the rotation on the model
		m_Model->SetWorld(CG3DMatrix::CreateRotationEuler(0, m_Angle, 0));

		m_Angle+=(float)M_PI*40.0f*timeStep/180.0f;
		return true;
	}
	virtual void RenderInternal(float timeStep)
	{
		//render the camera and model
		m_ConstantBuffer->Apply();
		m_Model->Apply();
	}
private:
	CG3DPtr<CG3DModel> m_Model;
	float m_Angle;
	CG3DMatrix m_Persp;
	CG3DPtr<CG3DMaterial> m_ConstantBuffer;
};

int main()
{
	CGame game;

	game.Run();
}