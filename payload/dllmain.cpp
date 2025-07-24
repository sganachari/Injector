#include <windows.h>
#include <winerror.h>
#include <psapi.h>
#include <stdio.h>

void Attack()
{
	char szProcessName[128];
	GetModuleBaseNameA(GetCurrentProcess(), NULL, szProcessName, sizeof(szProcessName));
	printf("calling attack");
	MessageBoxA(NULL, "BOOM!", szProcessName, MB_OK);
}

BOOL WINAPI DllMain(
	HINSTANCE hinstDLL,  // handle to DLL module
	DWORD fdwReason,     // reason for calling function
	LPVOID lpvReserved)  // reserved
{
	printf("DLL Main");
	switch (fdwReason)
	{
	case DLL_PROCESS_ATTACH:
		// Initialize once for each new process.
		printf("DLL process attach");
		Attack();
		break;

	case DLL_THREAD_ATTACH:
		printf("DLL thread attach");
		break;
	case DLL_THREAD_DETACH:
		printf("DLL thread detach");
		break;
	case DLL_PROCESS_DETACH:
		printf("DLL process detach");
		if (lpvReserved != nullptr)
		{
			break; // do not do cleanup if process termination scenario
		}
		break;
	}
	return TRUE;  // Successful DLL_PROCESS_ATTACH.
}