#ifndef G3D_CORE_BASE_H
#define G3D_CORE_BASE_H

// Base class for G3D classes
// You cannot call delete on this class, you have to call Release(), this allows
// derived classes to implement reference counting and is safe across extension boundaries.

class CG3DBase
{
public:
	virtual void Release() { delete this; }
protected:
    virtual ~CG3DBase() {}
};

// Reference counting class for G3D classes
// weakptr is used by the binary loader system and should not be used

class CG3DRefCount : public CG3DBase {
public:
	CG3DRefCount() : m_RefCount(0), m_WeakPtr(0) {}
	
	void AddRef();
	virtual void Release();

	void SetWeakPtr(CG3DRefCount** ptr) { m_WeakPtr = ptr; }
protected:
	int m_RefCount;
	CG3DRefCount** m_WeakPtr;
};

// Pointer class pointing to CG3DRefCount classes
// use this in place of T* to automatically handle ref counting

#include <memory>

template<class T>
class CG3DPtr
{
private:
	void set(T* ptr) { m_Ptr = ptr; if (m_Ptr != 0) m_Ptr->AddRef(); }
public:
	T* get() const { return m_Ptr; }
	void release() {
		if (m_Ptr != 0)
			m_Ptr->Release();
		m_Ptr = 0;
	}
	void reset(T* ptr = 0) {
		release();
		set(ptr);
	}
	
	CG3DPtr() : m_Ptr(0) {}
	CG3DPtr(T* ptr) { set(ptr); }
	CG3DPtr(CG3DPtr const& ref) { set(ref.get()); }
	template<class T2>
	CG3DPtr(CG3DPtr<T2> const& ref) {
		T* ptr = ref.get();
		set(ptr);
	}
	~CG3DPtr() { release(); }

	T* operator->() const { return get(); }
	T& operator*() const { return *get(); }
	
	CG3DPtr& operator=(T* ptr) {
		reset(ptr);
		return *this;
	}
	CG3DPtr& operator=(CG3DPtr const& ref) {
		reset(ref.get());
		return *this;
	}
	template<class T2>
	CG3DPtr<T>& operator=(CG3DPtr<T2> const& ref) {
		T* ptr = ref.get();
		reset(ptr);
		return *this;
	}
	operator bool() const { return m_Ptr != 0; }
private:
	T* m_Ptr;
};

#endif