#ifndef G3D_CORE_MUTEX_H
#define G3D_CORE_MUTEX_H

// Mutex class for thread locking

class CG3DMutex : public CG3DRefCount
{
public:
	virtual bool Lock() = 0;
	virtual bool TryLock() = 0;
	virtual bool UnLock() = 0;
};

#endif