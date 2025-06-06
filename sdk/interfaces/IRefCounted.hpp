#pragma once
#include <Windows.h>
class IRefCounted
{
public:
	virtual int AddReference() = 0;
	virtual int Release() = 0;

};

class CRefCounted
{
public:
	virtual ~CRefCounted() { }
	virtual bool OnFinalRelease() = 0;

	int AddReference()
	{
		return /*__sync_fetch_and_add(viReferenceCount, 1) + 1*/ static_cast<int>(_InterlockedIncrement(reinterpret_cast<volatile long*>(&viReferenceCount)));
	}

	int Release()
	{
		if (const int iResult = /*__sync_fetch_and_add(viReferenceCount, -1) - 1*/ static_cast<int>(_InterlockedDecrement(reinterpret_cast<volatile long*>(&viReferenceCount))); iResult > 0)
			return iResult;

		if (/*bSelfDelete &&*/ this->OnFinalRelease())
			/*delete this;*/this->~CRefCounted();

		return 0;
	}

private:
	volatile int viReferenceCount;
};
static_assert(sizeof(CRefCounted) == 0x8);

template <class T>
class CBaseAutoPtr
{
public:
	CBaseAutoPtr() :
		pObject(nullptr) { }

	CBaseAutoPtr(T* pObject) :
		pObject(pObject) { }

	operator const void* () const
	{
		return pObject;
	}

	operator void* () const
	{
		return pObject;
	}

	operator const T* () const
	{
		return pObject;
	}

	operator const T* ()
	{
		return pObject;
	}

	operator T* ()
	{
		return pObject;
	}

	int	operator=(int i)
	{
		pObject = nullptr;
		return 0;
	}

	T* operator=(T* pSecondObject)
	{
		pObject = pSecondObject;
		return pSecondObject;
	}

	bool operator!() const
	{
		return (!pObject);
	}

	bool operator==(const void* pSecondObject) const
	{
		return (pObject == pSecondObject);
	}

	bool operator!=(const void* pSecondObject) const
	{
		return (pObject != pSecondObject);
	}

	bool operator==(T* pSecondObject) const
	{
		return operator==(static_cast<void*>(pSecondObject));
	}

	bool operator!=(T* pSecondObject) const
	{
		return operator!=(static_cast<void*>(pSecondObject));
	}

	bool operator==(const CBaseAutoPtr<T>& pSecondPtr) const
	{
		return operator==(static_cast<const void*>(pSecondPtr));
	}

	bool operator!=(const CBaseAutoPtr<T>& pSecondPtr) const
	{
		return operator!=(static_cast<const void*>(pSecondPtr));
	}

	T* operator->()
	{
		return pObject;
	}

	T& operator*()
	{
		return *pObject;
	}

	T** operator&()
	{
		return &pObject;
	}

	const T* operator->() const
	{
		return pObject;
	}

	const T& operator*() const
	{
		return *pObject;
	}

	T* const* operator&() const
	{
		return &pObject;
	}

protected:
	CBaseAutoPtr(const CBaseAutoPtr<T>& pSecondPtr) :
		pObject(pSecondPtr.pObject) { }

	void operator=(const CBaseAutoPtr<T>& pSecondPtr)
	{
		pObject = pSecondPtr.pObject;
	}

	T* pObject;
};

template <class T>
class CRefPtr : public CBaseAutoPtr<T>
{
	typedef CBaseAutoPtr<T> CBaseClass;
public:
	CRefPtr() { }

	CRefPtr(T* pInit)
		: CBaseClass(pInit) { }

	CRefPtr(const CRefPtr<T>& pRefPtr)
		: CBaseClass(pRefPtr) { }

	~CRefPtr()
	{
		if (CBaseClass::pObject != nullptr)
			CBaseClass::pObject->Release();
	}

	void operator=(const CRefPtr<T>& pSecondRefPtr)
	{
		CBaseClass::operator=(pSecondRefPtr);
	}

	int operator=(int i)
	{
		return CBaseClass::operator=(i);
	}

	T* operator=(T* pSecond)
	{
		return CBaseClass::operator=(pSecond);
	}

	operator bool() const
	{
		return !CBaseClass::operator!();
	}

	operator bool()
	{
		return !CBaseClass::operator!();
	}

	void SafeRelease()
	{
		if (CBaseClass::pObject != nullptr)
			CBaseClass::pObject->Release();

		CBaseClass::pObject = nullptr;
	}

	void AssignAddReference(T* pFrom)
	{
		if (pFrom != nullptr)
			pFrom->AddReference();

		SafeRelease();
		CBaseClass::pObject = pFrom;
	}

	void AddReferenceAssignTo(T*& pTo)
	{
		if (CBaseClass::pObject != nullptr)
			CBaseClass::pObject->AddRef();

		SafeRelease(pTo);
		pTo = CBaseClass::pObject;
	}
};
