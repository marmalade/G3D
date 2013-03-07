#include "G3DGLES.h"
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
		//set standard global state
		m_Graphics->SetGlobalSet(CG3DGlobalSet::COLOUR, new CG3DColourSet(CG3DColour(0, 255, 0)));
		m_Graphics->SetGlobalSet(CG3DGlobalSet::CULL, new CG3DCullSet(CG3DCullSet::CULL_BACK));
		m_Graphics->SetGlobalSet(CG3DGlobalSet::DEPTH, new CG3DDepthSet(1));
		
		//load an effect from a text file
		CG3DPtr<CG3DEffectLib> effectLib = new CG3DEffectLib(m_Graphics.get());
		effectLib->LoadText("cube\\cube.g3d.effect");
		
		//load a model from a text file
		CG3DPtr<CG3DMesh> mesh = new CG3DMesh;
		mesh->LoadText("cube\\pCubeShape1.g3d.model", m_Graphics.get());
		
		//load materials from a text file
		CG3DMaterialLib* matLib = new CG3DMaterialLib(effectLib.get());
		matLib->LoadText("cube\\cube.g3d.material");
		
		CG3DMaterial* mats[2];
		mats[0] = matLib->GetMaterial("test_plaintext");
		mats[1] = matLib->GetMaterial("buffer");
		
		//create the rendertexture
		CG3DPtr<CG3DTexture> renderText = m_Graphics->MakeTexture();
		renderText->AddImage(CG3DTexture::TARGET_2D, ImageFormat(ImageFormat::D8, ImageFormat::Unsigned, ImageFormat::RGB, ImageFormat::None), 256, 256);
		mats[1]->SetTexture("Texture", renderText.get());

		//make to models, one with each texture
		m_Model = mesh->MakeInstance(mats);
		m_Model2 = mesh->MakeInstance(mats+1);
		
		//set up the camera
		m_Camera = new CG3DCamera(mats[0]->GetEffect());

		CG3DVector2 size = m_System->GetSize();
		CG3DMatrix persp = CG3DMatrix::CreatePerspectiveFOV((float)M_PI/2.0f, size.x/size.y, 0.01f, 100.0f);
		CG3DMatrix view = CG3DMatrix::CreateTranslation(CG3DVector3(0,-0.5f,-5));

		m_Camera->SetView(view);
		m_Camera->SetProj(persp);
		
		//set up the render buffer, using the rendertexture for the colour buffer
		m_Render = m_Graphics->MakeRenderTo();
		m_Render->SetTexture(CG3DRenderTo::COLOUR, renderText.get());
		m_Render->SetBuffer(CG3DRenderTo::DEPTH, CG3DRenderTo::FORMAT_DEPTH_16, 256, 256);
		m_Render->AddSet(CG3DGlobalSet::VIEWPORT, new CG3DViewportSet(CG3DVector4(0, 0, 256, 256)));
		m_Render->AddSet(CG3DGlobalSet::COLOUR, new CG3DColourSet(CG3DColour(0,0,255,255)));

		//upload all to the gpu
		m_Model->Upload();
		m_Model2->Upload();
		m_Render->Upload();

		m_Angle=0;
	}
	virtual void ShutdownInternal()
	{
	}
	virtual bool UpdateInternal(float timeStep)
	{
		//rotate the 2 models
		m_Model->SetWorld(CG3DMatrix::CreateRotationEuler(0, m_Angle, 0));
		m_Model2->SetWorld(CG3DMatrix::CreateRotationEuler(0, m_Angle, 0));

		m_Angle+=(float)M_PI*40.0f*timeStep/180.0f;

		return true;
	}
	virtual void RenderInternal(float timeStep)
	{
		m_Camera->Apply();	//draw the camera
		m_Render->Apply();	//start using the render buffer
		m_Graphics->Clear();//clear the render buffer

		m_Model->Apply();	//draw the model with a normal texture

		m_Render->DoneApply();	//finish using the render buffer

		m_Model2->Apply();	//draw the second model with a render texture
	}
private:
	CG3DPtr<CG3DModel> m_Model;
	CG3DPtr<CG3DModel> m_Model2;
	CG3DPtr<CG3DRenderTo> m_Render;
	float m_Angle;
	CG3DPtr<CG3DCamera> m_Camera;
};

int main()
{
	CGame game;

	game.Run();
}