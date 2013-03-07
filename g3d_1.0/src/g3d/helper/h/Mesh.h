#ifndef G3D_HELPER_MESH_H
#define G3D_HELPER_MESH_H

#include "G3DCore.h"
#include "SubModel.h"
#include "Model.h"

//encapsulates a mesh (that is a model without materials or positioner).
//optionally load from text based file

class CG3DMesh : public CG3DResourceList<CG3DResource>
{
	enum MapType {
		MAP_MASK = 127,
		MAP_MATERIAL = 128,
	};
public:
	void LoadText(const char* filename, CG3DGraphics* graphics);	//load the file

	void AddSubModel(int material, CG3DVertexBuffer* vertex, CG3DIndexBuffer* index);	//add a sub model
	void AddSubModel(int material, int* parts, int numParts);	//set up the map of the elements of a submodel (add Add to add the parts referenced)
	void SetMap(unsigned char* parts, int numParts);			//set up the map of the elements of a submodel (add Add to add the parts referenced)

	int NumSubModels();
	void MakeSubModels(CG3DPositioner* pos, CG3DMaterial** materials, CG3DSubModel** subModels);	//make a set of sub models from this mesh
	CG3DModel* MakeInstance(CG3DMaterial** materials);	//make a model from this mesh

	virtual void ReleaseInternal() {}
protected:
	virtual void AddInternal(CG3DResource* item) {}
	virtual void RemoveInternal(CG3DResource* item) {}
	virtual void UploadInternal() {}
	virtual void ReloadInternal() {}
	virtual void UnloadInternal() {}
	virtual void ApplyInternal() {}
private:
	std::vector<unsigned char> m_Map;
};

#endif