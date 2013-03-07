#ifndef G3D_HIERARCHY_EVENT_H
#define G3D_HIERARCHY_EVENT_H

#include "G3DCore.h"
#include "Util.h"

//basic events used by game objects

#define G3D_UPDATE_EVENT 0x20edd0a8			//"event"
//#define G3D_RENDER_EVENT 0x192ccfe5			//"render"
#define G3D_SET_LOCAL_EVENT 0x127732bc		//"setlocal"

//base class for all events
class CG3DEvent : public CG3DRefCount {
public:
	unsigned int m_Event;
	float m_TimeToFire;
public:
	CG3DEvent(unsigned int event) : m_Event(event), m_TimeToFire(0) {}
	CG3DEvent(const char* event) : m_Event(G3DHashString(event)), m_TimeToFire(0) {}
};

//event called on each update of the frame
class CG3DUpdateEvent : public CG3DEvent {
public:
	float m_TimeStep;
public:
	CG3DUpdateEvent(float timeStep, unsigned int event = G3D_UPDATE_EVENT) : m_TimeStep(timeStep), CG3DEvent(event) {}
};

//event called to send the local matrix to interested modules, there is usually a module the combines this wtih the
//parents world matrix to generate the world matrix
class CG3DSetLocalEvent : public CG3DEvent {
public:
	CG3DMatrix m_Local;
public:
	CG3DSetLocalEvent(CG3DMatrix local, unsigned int event = G3D_SET_LOCAL_EVENT) : m_Local(local), CG3DEvent(event) {}
};

#endif