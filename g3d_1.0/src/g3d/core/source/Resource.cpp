#include "Resource.h"
#include "System.h"

#include <stddef.h>

void CG3DResource::Upload()
{
	if (GetInternalFlag(UPLOADED))
		return;

	UploadInternal();
	SetInternalFlag(UPLOADED, true);

	if (!GetFlag(DYNAMIC))
		ReleaseInternal();
}
void CG3DResource::Reload()
{
	if (!GetInternalFlag(UPLOADED) || !GetFlag(DYNAMIC))
		return;

	ReloadInternal();
}

void CG3DResource::Unload()
{
	if (!GetInternalFlag(UPLOADED))
		return;

	UnloadInternal();
	SetInternalFlag(UPLOADED, false);
}

void CG3DResource::Apply()
{
	if (!GetInternalFlag(UPLOADED))
		Upload();

	ApplyInternal();
}

void CG3DRefCount::AddRef()
{
	m_RefCount++;
}

void CG3DRefCount::Release()
{
	m_RefCount--;
	if(m_RefCount>0)
		return;

	if (m_WeakPtr != NULL)
		(*m_WeakPtr) = NULL;
	CG3DBase::Release();
}

void CG3DResource::Release()
{
	m_RefCount--;
	if(m_RefCount>0)
		return;

	if (GetInternalFlag(UPLOADED))
		Unload();

	ReleaseInternal();

	if (m_WeakPtr != NULL)
		(*m_WeakPtr) = NULL;
	CG3DBase::Release();
}