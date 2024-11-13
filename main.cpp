#include <Windows.h>
#include "util.h"
#include "draw.h"

BOOL DllMain(HMODULE ModuleHandle, DWORD Reason, LPVOID Reserved) {

	switch (Reason) {

		case DLL_PROCESS_ATTACH: {

			DrawImGui(ModuleHandle);
		}
	}

	return TRUE;
}
