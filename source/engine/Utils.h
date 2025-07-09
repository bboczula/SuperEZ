#pragma once

#include <Windows.h>
#include <comdef.h>

#define FRAME_COUNT 2

#define DX_TRY(expr, context, tip) \
    CheckHr((expr), (context), __FILE__, __LINE__, (tip))

#define DX_TRY_SIMPLE(expr, context) \
    CheckHr((expr), (context), __FILE__, __LINE__, nullptr)

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

inline void CheckHr(HRESULT hr, const char* context, const char* file, int line, const char* tip = nullptr)
{
	if (SUCCEEDED(hr)) return;

	char sysMsg[512];
	FormatMessageA(
		FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		nullptr,
		hr,
		0,
		sysMsg,
		sizeof(sysMsg),
		nullptr);

	char fullMsg[1024];
	snprintf(fullMsg, sizeof(fullMsg),
		"%s failed.\n\n%s\n\nTip: %s\n\nFile: %s\nLine: %d",
		context,
		sysMsg[0] ? sysMsg : "Unknown error",
		tip ? tip : "No tip available",
		file,
		line);

	char title[64];
	snprintf(title, sizeof(title), "HRESULT: 0x%08X", hr);

	int result = MessageBoxA(nullptr, fullMsg, title, MB_ABORTRETRYIGNORE | MB_ICONERROR);

	if (result == IDABORT)
		exit(EXIT_FAILURE);
	else if (result == IDRETRY)
		__debugbreak();
}