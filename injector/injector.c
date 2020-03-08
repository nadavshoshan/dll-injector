#include <Windows.h>
#include <stdio.h>
#include<stdlib.h>
#include <tlhelp32.h>
#include <string.h>
int findProcessId(wchar_t* name);
//char* ascii_to_utf8(unsigned char c);
int main(int argc, char* argv[]);
//char* concat(const char* s1, const char* s2);

int wmain(int argc, wchar_t* argv[]) {
	wchar_t* processName = argv[1];
	char* dllPath = argv[2];
	printf("%s\n", processName);
	char* utf_process_name="";
	int i;
	for (i = 0; i < strlen(processName); i++) {
		utf_process_name = concat(ascii_to_utf8(processName[i]), utf_process_name);
	}
	int id = findProcessId(utf_process_name);
	printf("path id of %s is: %d\n", processName ,id);
	return 0;
}
//
//char* concat(const char* s1, const char* s2)
//{
//	char* result = malloc(strlen(s1) + strlen(s2) + 1); // +1 for the null-terminator
//	// in real code you would check for errors in malloc here
//	strcpy(result, s1);
//	strcat(result, s2);
//	return result;
//}
//
//char* ascii_to_utf8(unsigned char c)
//{
//	unsigned char* out;
//
//	if (c < 128)
//	{
//		out = (char*)calloc(2, sizeof(char));
//		out[0] = c;
//		out[1] = '\0';
//	}
//	else
//	{
//		out = (char*)calloc(3, sizeof(char));
//		out[1] = (c >> 6) | 0xC0;
//		out[0] = (c & 0x3F) | 0x80;
//		out[2] = '\0';
//	}
//
//	return out;
//}

int findProcessId(wchar_t* name) {

	int process_id = 0;
	int true = 1;
	//keep the rutine until the given procces is located
	while (true) {
		//get all the running processes into snap!
		HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
		if (snap == INVALID_HANDLE_VALUE) {
			printf("%s", GetLastError());
		}

		// Declare a PROCESSENTRY32 variable
		PROCESSENTRY32 pe32;
		// Set the size of the structure before using it.
		pe32.dwSize = sizeof(PROCESSENTRY32);


		if (!Process32First(snap, &pe32)) {
			printf("%s", GetLastError());
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
		CloseHandle(snap);
		Sleep(100);
	}

	return process_id;
}