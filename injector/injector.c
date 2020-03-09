#include "injector.h"
#define MyDll "C:\\Users\\Balmas\\Desktop\\dll-play\\Debug\\dll_play.dll"
#define MyDll64 "C:\\Users\\Balmas\\Desktop\\dll-play\\x64\\Debug\\dll_play.dll"


int findProcessId(wchar_t* name);
int wmain(int argc, wchar_t* argv[]);
int injection(int process_id);


int wmain(int argc, wchar_t* argv[]) {
	wchar_t* processName = argv[1];
	int id = findProcessId(processName);
	printf_s("path id of %ls is: %d\n", processName ,id);
	if (injection(id)) {
		printf("the given DLL was injected to %ls\n", processName);
	}
	return 0;
}

int findProcessId(wchar_t* name) {

	int process_id = 0;
	int true = 1;
	//keep the rutine until the given procces is located
	while (true) {
		//get all the running processes into snap!
		HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
		if (snap == INVALID_HANDLE_VALUE) {
			printf_s("%lc", GetLastError());
		}

		// Declare a PROCESSENTRY32 variable
		PROCESSENTRY32 pe32;
		// Set the size of the structure before using it.
		pe32.dwSize = sizeof(PROCESSENTRY32);


		if (!Process32First(snap, &pe32)) {
			printf_s("%lc", GetLastError());
			CloseHandle(snap);          // clean the snapshot object
		}
		// Cycle through Process List
		do {

			// Uncomment line below if you want your program to spit out every single list
			// printf("%20s\t\t%d\n",pe32.szExeFile, pe32.th32ProcessID);
			// Comparing two strings containing process names for 'equality'
			if (_wcsicmp(pe32.szExeFile, name) == 0) {
				process_id = pe32.th32ProcessID;
				true = 0;
			}

		} while (Process32Next(snap, &pe32));

		// close the snapshot 
		if (!snap) {
			CloseHandle(snap);
		}
		Sleep(100);
	}

	return process_id;
}

int injection(int process_id) {
	int toReturn = 1;
	DWORD size = (strlen(MyDll) + 1) * sizeof(wchar_t);

	HANDLE hToPrc = OpenProcess(PROCESS_ALL_ACCESS, FALSE, process_id);
	if (!hToPrc) {
		printf("couldn't get a handle to the given procces");
		toReturn = 0;
	}
	LPVOID dllPathAdress = VirtualAllocEx(hToPrc, 0, size, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	if (!dllPathAdress) {
		printf("couldn't allocate memory in the target heap");
		toReturn = 0;
	}
	int res = WriteProcessMemory(hToPrc, dllPathAdress, MyDll, size, NULL);
	if (!res) {
		printf("couldn't write to the target heap");
		toReturn = 0;
	}
	FARPROC loadLibAadress = GetProcAddress(GetModuleHandleA("kernel32.dll"), "LoadLibraryA");
	if (!loadLibAadress) {
		printf_s("%lc", GetLastError());
		toReturn = 0;
	}

	HANDLE nThread = CreateRemoteThread(hToPrc, NULL, 0, (LPTHREAD_START_ROUTINE)loadLibAadress, dllPathAdress, 0, NULL);
	if (!nThread) {
		printf("couldn't get a handle to the new remote thread");
		toReturn = 0;
	}

	res = WaitForSingleObject(nThread, INFINITE);

	CloseHandle(hToPrc);

	return toReturn;
}

