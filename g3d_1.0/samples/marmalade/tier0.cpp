#include "G3DCore.h"
#include "G3DMarmalade.h"
#include "G3DMaths.h"

/*
 * This example demonstrates only tier 0 functionality of G3D.
 * That is using just G3DCore, G3DMaths and the implementations of G3DCore: G3DMarmalade and G3DGLES.
 * This is the lowest level usage of G3D, more suitible for advanced use and investigating how G3D works.
 */

int main()
{
	// create system and graphics libraries
	CG3DPtr<CG3DSystem> system = MakeG3DSystemMarmalade();
	CG3DPtr<CG3DGraphics> graphics = system->Init();
	
	// set the viewport
	CG3DVector2 size = system->GetSize();
	CG3DPtr<CG3DViewportSet> viewport = new CG3DViewportSet(CG3DVector4(0, 0, size.x, size.y));
	graphics->SetGlobalSet(CG3DGlobalSet::VIEWPORT, viewport.get());
	
	// set other global state
	graphics->SetGlobalSet(CG3DGlobalSet::COLOUR, new CG3DColourSet(CG3DColour(0, 255, 0)));
	graphics->SetGlobalSet(CG3DGlobalSet::CULL, new CG3DCullSet(CG3DCullSet::CULL_BACK));
	graphics->SetGlobalSet(CG3DGlobalSet::DEPTH, new CG3DDepthSet(1));

	// make a vertex buffer and set its format
	CG3DPtr<CG3DVertexBuffer> vbuffer = graphics->MakeVertexBuffer();
	vbuffer->AddElement("POSITION", ELEMENT_TYPE(COMPONENT_3, SIZE_32, TYPE_FLOAT));
	vbuffer->AddElement("COLOR0", ELEMENT_TYPE(COMPONENT_4, SIZE_32, TYPE_FLOAT));

	// create the buffer and add data to it
	vbuffer->CreateBuffer(8);
	vbuffer->Add(CG3DVector3(-1, -1, 1)); vbuffer->Add(CG3DVector4(0, 0, 0, 1));
	vbuffer->Add(CG3DVector3(1, -1, 1)); vbuffer->Add(CG3DVector4(0, 0, 1, 1));
	vbuffer->Add(CG3DVector3(-1, 1, 1)); vbuffer->Add(CG3DVector4(0, 1, 0, 1));
	vbuffer->Add(CG3DVector3(1, 1, 1)); vbuffer->Add(CG3DVector4(1, 0, 0, 1));
	vbuffer->Add(CG3DVector3(-1, 1, -1)); vbuffer->Add(CG3DVector4(1, 1, 0, 1));
	vbuffer->Add(CG3DVector3(1, 1, -1)); vbuffer->Add(CG3DVector4(1, 0, 1, 1));
	vbuffer->Add(CG3DVector3(-1, -1, -1)); vbuffer->Add(CG3DVector4(0, 1, 1, 1));
	vbuffer->Add(CG3DVector3(1, -1, -1)); vbuffer->Add(CG3DVector4(1, 1, 1, 1));

	// make an index buffer and set its format
	CG3DPtr<CG3DIndexBuffer> ibuffer=graphics->MakeIndexBuffer();
	ibuffer->Setup(INDEX_TYPE(TRIANGLES, SIZE_16));
	
	// create the buffer and add data to it
	ibuffer->CreateBuffer(6 * 6);
	ibuffer->Add((short)0); ibuffer->Add((short)1); ibuffer->Add((short)3); ibuffer->Add((short)0); ibuffer->Add((short)3); ibuffer->Add((short)2);
	ibuffer->Add((short)2); ibuffer->Add((short)3); ibuffer->Add((short)5); ibuffer->Add((short)2); ibuffer->Add((short)5); ibuffer->Add((short)4);
	ibuffer->Add((short)4); ibuffer->Add((short)5); ibuffer->Add((short)7); ibuffer->Add((short)4); ibuffer->Add((short)7); ibuffer->Add((short)6);
	ibuffer->Add((short)6); ibuffer->Add((short)7); ibuffer->Add((short)1); ibuffer->Add((short)6); ibuffer->Add((short)1); ibuffer->Add((short)0);
	ibuffer->Add((short)1); ibuffer->Add((short)7); ibuffer->Add((short)5); ibuffer->Add((short)1); ibuffer->Add((short)5); ibuffer->Add((short)3);
	ibuffer->Add((short)6); ibuffer->Add((short)0); ibuffer->Add((short)2); ibuffer->Add((short)6); ibuffer->Add((short)2); ibuffer->Add((short)4);
	
	// make an effect
	CG3DPtr<CG3DEffect> effect=graphics->MakeEffect();

	// add a vertex shader to the effect
	CG3DPtr<CG3DShader> vshader=effect->MakeShader();
	vshader->SetSource(CG3DShader::VERTEX,
		"attribute vec4 POSITION;"
		"attribute vec4 COLOR0;"
		"varying vec4 ColourConnect;"
		"uniform mat4 WV;"
		"uniform mat4 P;"
		"void main(void) {"
		"    ColourConnect = COLOR0;"
		"    gl_Position = P * WV * POSITION;"
		"}");

	// add a pixel shader to the effect
	CG3DPtr<CG3DShader> pshader=effect->MakeShader();
	pshader->SetSource(CG3DShader::PIXEL,
		"uniform mediump vec4 MatColour;"
		"varying lowp vec4 ColourConnect;"
		"void main(void) {"
		"    gl_FragColor = MatColour * ColourConnect;"
		"}");

	// set up the parameters on the effect
	effect->AddParameter("WV", PARAMETER_TYPE(COMPONENT_4x4, TYPE_MATRIX));
	effect->AddParameter("P", PARAMETER_TYPE(COMPONENT_4x4, TYPE_MATRIX));
	effect->AddParameter("MatColour", PARAMETER_TYPE(COMPONENT_4, TYPE_FLOAT));
		
	// make a material
	CG3DPtr<CG3DMaterial> mat=effect->MakeMaterial();
	
	// upload the date we have created to the graphics system
	mat->Upload();
	vbuffer->Upload();
	ibuffer->Upload();

	// set the vertex buffer up, based on the material
	vbuffer->PatchUp(mat.get());
		
	// set a parameter on the material, this is the overall colour
	mat->SetParameter("MatColour", CG3DVector4(0.5f, 0.5f, 1.0f, 1.0f));
	
	// set a parameter on the material, this is the perspective matrix
	size=system->GetSize();
	CG3DMatrix persp=CG3DMatrix::CreatePerspectiveFOV((float)M_PI/2.0f, size.x/size.y, 0.01f, 100.0f);
	mat->SetParameter("P", &persp.m11);

	float angle=0;
	CG3DMatrix world;
	long long lastTime = system->GetTime();

	// render each frame
	while(true) {
		// work out the interval from the last frame
		long long time = system->GetTime();
		float timeStep = (float)(time - lastTime) / 1000.0f;

		// update the system
		if (!system->Update())
			return false;
		
		// set a parameter on the material, this is the world and view matrix
		world = CG3DMatrix::CreateRotationEuler(0, angle, 0);
		world.SetTranslation(CG3DVector3(0,-0.5f,-5.0f));
		mat->SetParameter("WV", &world.m11);

		// update the rotation of the model
		angle+=(float)M_PI*40.0f*timeStep/180.0f;
		
		// change the viewport dependant on the screen size
		size = system->GetSize();
		viewport->m_Rect = CG3DVector4(0, 0, size.x, size.y);
		graphics->ApplySet(CG3DGlobalSet::VIEWPORT);

		// clear the screen to the current colour
		graphics->Clear();
		
		// set the material and draw the model
		mat->Apply();
		vbuffer->Apply();
		ibuffer->Apply();
		mat->DoneApply();

		//swap the front and back buffers
		system->EndRender();
		lastTime = time;
	}
}