#ifndef G3D_HIERARCHY_GAMEOBJECT_H
#define G3D_HIERARCHY_GAMEOBJECT_H

#include "G3DCore.h"
#include "G3DMaths.h"
#include <vector>
#include <map>

#include "Util.h"
#include "Event.h"
#include "Module.h"

#include <string>

//a game object is an object in 3D space, in a hierarchy, that has a set of modules that have data and define behavour
//this manages events

class CG3DGameObject : public CG3DRefCount {
	struct EventHandle {
		CG3DModule* m_Module;
		int m_Priority;
		EventHandle(CG3DModule* module, int priority) : m_Module(module), m_Priority(priority) {}
	};
	typedef std::map<unsigned int, std::vector<EventHandle> > EventMap;
	typedef std::map<std::string, CG3DModule*> ModuleMap;
private:
	ModuleMap m_Modules;
	std::vector<CG3DEvent*> m_Pending;
	EventMap m_EventHandlers;
public:
	CG3DMatrix m_WorldMatrix;					//world position
	std::vector<CG3DGameObject*> m_Children;
	CG3DGameObject* m_Parent;
public:
	CG3DGameObject() : m_WorldMatrix(CG3DMatrix::s_Identity), m_Parent(NULL) {}
	~CG3DGameObject();

	//deal with the hierarchy
	void AddChild(CG3DGameObject* child);
	void RemoveChild(CG3DGameObject* child);
	CG3DGameObject* GetRoot();

	//deal with the list of modules
	void AddModule(const char* name, CG3DModule* module,bool enable);
	void RemoveModule(const char* name);
	void EnableModule(const char* name);
	void DisableModule(const char* name);

	//get data in the form or <module name>.<data name>
	template<class T> T* GetData(const char* name) {
		char dataName[64];
		CG3DModule* module = GetModuleAndName(name, dataName);
		if(module==NULL) return NULL;

		return module->GetData<T>(dataName);
	}
	
	//deal with event handlers
	void AddHandler(const char* event, CG3DModule* module, int priority = 0);
	void AddHandler(unsigned int event, CG3DModule* module, int priority = 0);
	void RemoveHandler(const char* event, CG3DModule* module);
	void RemoveHandler(unsigned int event, CG3DModule* module);

	void DoEvent(CG3DEvent* event); //send an event
	void DoFrame(float timeStep);	//handle the frame event and post due events
private:
	CG3DModule* GetModuleAndName(const char* name, char* dataName);
};

#endif