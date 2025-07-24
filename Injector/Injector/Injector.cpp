// Injector.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include <Windows.h>
#include <winerror.h>

#include <stdio.h>

int main(int argc, char* argv[])
{
	char* szDLLPathToInject = argv[2];
	printf("dll to be inject is %s\n",(szDLLPathToInject));
	int nDLLPathLen = lstrlenA(szDLLPathToInject);
	int nTotBytesToAllocate = nDLLPathLen + 1; // including NULL character.

	// 0. Open The process
	HANDLE hProcess = OpenProcess(PROCESS_CREATE_THREAD |
		PROCESS_VM_WRITE | PROCESS_VM_OPERATION, FALSE, atoi(argv[1]));

	if (hProcess == NULL)
	{
		printf("Failed to open process handle. Exiting");
		return -1;
	}
	printf("opened handle succesfully\n");
	// 1. Alocate heap memory in remote process
	LPVOID lpHeapBaseAddress1 = VirtualAllocEx(hProcess, NULL,
		nTotBytesToAllocate, MEM_COMMIT, PAGE_READWRITE);

	if (lpHeapBaseAddress1 == NULL)
	{
		printf("Failed to create virtual memory in remote process\n");
		return -1;
	}
	printf("Memory succesfully created\n");
	// 2. Write the DLL path in the remote allocated heap memory.
	SIZE_T lNumberOfBytesWritten = 0;
	WriteProcessMemory(hProcess, lpHeapBaseAddress1,
		szDLLPathToInject, nTotBytesToAllocate, &lNumberOfBytesWritten);

	if (GetLastError() != ERROR_SUCCESS)
	{
		printf("Failed to Write virtual memory in remote process");
		return -1;
	}
	printf("Write memory success\n");
	// 3.0. Get the starting address of the function LoadLibrary 
	// which is available in kernal32.dll
	LPTHREAD_START_ROUTINE lpLoadLibraryStartAddress =
		(LPTHREAD_START_ROUTINE)GetProcAddress(GetModuleHandle(L"Kernel32.dll"),
			"LoadLibraryA");
	if (lpLoadLibraryStartAddress == NULL) {
		printf("Failed to get loadlibrary address");
		return -1;
	}
	printf("loadlibrary adress found\n");
	// 3.1. Call LoadLibraryin remote process and pass the remote heap memory 
	// which contains the dll path to load.
	CreateRemoteThread(hProcess, NULL, 0, lpLoadLibraryStartAddress,
		lpHeapBaseAddress1, 0, NULL);
	
	DWORD last_error = GetLastError();
	if (last_error != ERROR_SUCCESS)
	{
		printf("Failed to create remote thread in remote process %x", last_error);
		return -1;
	}
	printf("create remote thread success\n");
	CloseHandle(hProcess);
	return 0;
}