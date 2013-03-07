#define _CRT_SECURE_NO_WARNINGS

#include "GameObject.h"
#include <stdio.h>
#include <string.h>

CG3DGameObject::~CG3DGameObject()
{
	for(ModuleMap::iterator it=m_Modules.begin(); it!=m_Modules.end(); ++it)
		it->second->Release();
	for(int i=0;i<(int)m_Children.size();i++)
		m_Children[i]->Release();
}

void CG3DGameObject::AddChild(CG3DGameObject* child)
{
	if(child==NULL) return;

	child->AddRef();
	m_Children.push_back(child);
	child->m_Parent = this;
}
void CG3DGameObject::RemoveChild(CG3DGameObject* child)
{
	if(child==NULL) return;

	for(int i=0;i<(int)m_Children.size();i++)
		if(m_Children[i]==child) {
			m_Children.erase(m_Children.begin()+i);
			break;
		}
	child->m_Parent = NULL;
	child->Release();
}

CG3DGameObject* CG3DGameObject::GetRoot()
{
	CG3DGameObject* parent;
	for(parent=this; parent->m_Parent!=NULL; parent=parent->m_Parent);
	return parent;
}

void CG3DGameObject::AddModule(const char* name, CG3DModule* module,bool enable)
{
	if(m_Modules.find(name) != m_Modules.end())
		RemoveModule(name);

	if(module==NULL) return;

	module->AddRef();
	if (m_Modules[name] != NULL)
		m_Modules[name]->Release();
	m_Modules[name] = module;
	module->m_Parent = this;

	if(enable)
		module->Enable();
}
void CG3DGameObject::RemoveModule(const char* name)
{
	ModuleMap::iterator it = m_Modules.find(name);
	if (it == m_Modules.end())
		return;
	
	it->second->Disable();
	it->second->Release();
	m_Modules.erase(it);
}
void CG3DGameObject::EnableModule(const char* name)
{
	ModuleMap::iterator it = m_Modules.find(name);
	if (it == m_Modules.end())
		return;
	it->second->Enable();
}
void CG3DGameObject::DisableModule(const char* name)
{
	ModuleMap::iterator it = m_Modules.find(name);
	if (it == m_Modules.end())
		return;
	it->second->Disable();
}
CG3DModule* CG3DGameObject::GetModuleAndName(const char* name, char* dataName)
{
	char modName[64];
	const char* ptr=strchr(name, '.');
	if(ptr==NULL) return NULL;
	
	strncpy(modName, name, ptr-name);
	modName[ptr-name] = 0;
	strncpy(dataName, ptr+1, 64);
	
	ModuleMap::iterator it = m_Modules.find(modName);
	if (it == m_Modules.end())
		return NULL;
	return it->second;
}

void CG3DGameObject::AddHandler(const char* event, CG3DModule* module, int priority)
{
	AddHandler(G3DHashString(event), module, priority);
}
void CG3DGameObject::AddHandler(unsigned int event, CG3DModule* module, int priority)
{
	EventMap::iterator it=m_EventHandlers.find(event);
	if(it==m_EventHandlers.end())
	{
		m_EventHandlers[event]=std::vector<EventHandle>();
		it=m_EventHandlers.find(event);
	}
	for(int i=0;i<(int)it->second.size();i++)
		if(it->second[i].m_Module==module)
			return;
	for(int i=0;i<(int)it->second.size();i++)
		if(priority < it->second[i].m_Priority)
		{
			it->second.insert(it->second.begin()+i, EventHandle(module, priority));
			return;
		}
	it->second.push_back(EventHandle(module, priority));
}

void CG3DGameObject::RemoveHandler(const char* event, CG3DModule* module)
{
	RemoveHandler(G3DHashString(event), module);
}
void CG3DGameObject::RemoveHandler(unsigned int event, CG3DModule* module)
{
	EventMap::iterator it=m_EventHandlers.find(event);
	if(it==m_EventHandlers.end())
		return;

	for(int i=0;i<(int)it->second.size();i++)
		if(it->second[i].m_Module==module)
		{
			it->second.erase(it->second.begin()+i);
			break;
		}
	if(it->second.empty())
		m_EventHandlers.erase(it);
}

void CG3DGameObject::DoEvent(CG3DEvent* event)
{
	event->AddRef();
	if(event->m_TimeToFire>0)
	{
		m_Pending.push_back(event);
		return;
	}

	EventMap::iterator it=m_EventHandlers.find(event->m_Event);

	if(it!=m_EventHandlers.end())
		for(int i=0;i<(int)it->second.size();i++)
			it->second[i].m_Module->DoEvent(event);

	event->Release();
}
void CG3DGameObject::DoFrame(float timeStep)
{
	for(int i=0;i<(int)m_Pending.size();)
	{
		m_Pending[i]->m_TimeToFire -= timeStep;
		if(m_Pending[i]->m_TimeToFire < 0)
		{
			DoEvent(m_Pending[i]);
			m_Pending[i]->Release();
			m_Pending.erase(m_Pending.begin()+i);
		}
		else
			i++;
	}

	DoEvent(new CG3DUpdateEvent(timeStep));
	
	for(int i=0;i<(int)m_Children.size();i++)
		m_Children[i]->DoFrame(timeStep);
}