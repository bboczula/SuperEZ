#pragma once

#include <Windows.h>
#include <comdef.h>

#define FRAME_COUNT 2

inline UINT Align(UINT location, UINT align)
{
	if ((0 == align) || (align & (align - 1)))
	{
		exit(1);
	}

	return ((location + (align - 1)) & ~(align - 1));
}

inline void ExitIfFailed(HRESULT hr)
{
	if (FAILED(hr))
	{
		_com_error error(hr);
		MessageBox(nullptr, error.ErrorMessage(), L"Error", MB_OK | MB_ICONERROR);
		exit(1);
	}
}

template <class T>
void SafeRelease(T** comObjectAddresssOf)
{
	if (*comObjectAddresssOf)
	{
		(*comObjectAddresssOf)->Release();
		*comObjectAddresssOf = nullptr;
	}
}

// Assume `pUnknown` is a pointer to your COM object
inline void PrintRefCount(IUnknown* pUnknown, const char* msg) {
	if (pUnknown) {
		// Call AddRef to get the incremented count
		pUnknown->AddRef();

		// Immediately release it so the count remains unchanged
		ULONG refCount = pUnknown->Release();

		// The returned count is the number of references before calling AddRef
		//std::cout << "Current reference count: " << refCount << std::endl;
		//return refCount;
		char buf[256];
		snprintf(buf, sizeof(buf), "Num of %s refs: %d\n", msg, refCount);
		OutputDebugStringA(buf);
	}
	else {
		//std::cout << "Invalid COM object pointer!" << std::endl;
		exit(1);
	}
}