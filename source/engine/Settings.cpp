#include "Settings.h"

#include <Windows.h>
#include <debugapi.h>

Settings::Settings()
{
	OutputDebugString(L"Settings Constructor\n");
}

Settings::~Settings()
{
	OutputDebugString(L"Settings Destructor\n");
}
