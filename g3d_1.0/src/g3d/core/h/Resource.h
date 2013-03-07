#ifndef G3D_CORE_RESOURCE_H
#define G3D_CORE_RESOURCE_H

// This is the base of all classes that upload data to the graphics system
//
// This implements reference counting, call AddRef() if you wish to retain a pointer to
// an instance of this class.
// By default data is deleted after upload, call SetFlag(CG3DResource::DYNAMIC, true) to
// retain the data, then call Upload() again when you want to refresh the data.
// CG3DResource::TRANSIENT and CG3DResource::DYNAMIC are also hints to the system about the
// usage of this data.

#include "Base.h"

class CG3DResource : public CG3DRefCount {
public:
	enum Flag {
		DYNAMIC = 1,	// if we are dynamic we need to retain the data in memory
		TRANSIENT = 2,	// if we are transient we are recreating the data on a regular basis
	};
protected:
	enum InternalFlag {
		UPLOADED = 1,
	};
public:
	CG3DResource() : m_Flags(0), m_InternalFlags(0) {}
	// Upload data to the graphics system
	virtual void Upload();
	// Re-upload data to the graphics system, this can be re-called if you change the data
	virtual void Reload();
	// Remove the data from the graphics system
	virtual void Unload();
	// Apply the data, i.e. set the graphics state or draw
	virtual void Apply();

	virtual void Release();

	bool IsUploaded() { return GetInternalFlag(UPLOADED); }

	// get or set flags
	bool GetFlag(unsigned int flag) { return (m_Flags & flag) == flag; }
	void SetFlag(unsigned int flag, bool value) { if (value) m_Flags |= flag; else m_Flags &= ~flag; }
protected:
	bool GetInternalFlag(unsigned int flag) { return (m_InternalFlags & flag) == flag; }
	void SetInternalFlag(unsigned int flag, bool value) { if (value) m_InternalFlags |= flag; else m_InternalFlags &= ~flag; }

	virtual void UploadInternal() = 0;
	// ReloadInternal may be called multiple times
	virtual void ReloadInternal() = 0;
	virtual void UnloadInternal() = 0;
	virtual void ApplyInternal() = 0;
public:
	virtual void ReleaseInternal() = 0;
protected:
	unsigned int m_Flags;
	unsigned int m_InternalFlags;
};

#endif