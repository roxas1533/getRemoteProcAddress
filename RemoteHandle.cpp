#include "RemoteHandle.h"
#include <assert.h>
#include <psapi.h>
#include <iostream> 
#pragma comment (lib, "psapi.lib")
using namespace rth;
UINT_PTR RemoteHandle::getFunctionList(HMODULE m , std::string name, std::string fName){
	UINT_PTR baseModule=(UINT64)m;
	std::cout << m<<"|"<<name<<"\n";
	_IMAGE_DOS_HEADER dosHeader;
	IMAGE_NT_HEADERS ntHeader;
	IMAGE_EXPORT_DIRECTORY exportDirectory;

	DWORD ExportTable = 0;
	UINT_PTR ExportFunctionTableVA = 0;
	UINT_PTR ExportNameTableVA = 0;
	UINT_PTR ExportOrdinalTableVA = 0;
	DWORD* ExportNameTable = NULL;
	DWORD* ExportFunctionTable = NULL;
	WORD* ExportOrdinalsTable = NULL;
	ReadProcessMemory(hProcess, (LPCVOID)baseModule, &dosHeader, sizeof(dosHeader), 0);
	ReadProcessMemory(hProcess, (LPCVOID)(baseModule + dosHeader.e_lfanew), &ntHeader, sizeof(ntHeader), 0);
	if (ntHeader.Signature != IMAGE_NT_SIGNATURE) {
		MessageBoxA(NULL, "Error", "E", MB_OK);
	}
	ExportTable = (ntHeader.OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT]).VirtualAddress;
	ReadProcessMemory(hProcess, (LPCVOID)(baseModule +ExportTable), &exportDirectory, sizeof(exportDirectory), 0);

	ExportFunctionTableVA = (UINT_PTR)baseModule + exportDirectory.AddressOfFunctions;
	ExportNameTableVA = (UINT_PTR)baseModule + exportDirectory.AddressOfNames;
	ExportOrdinalTableVA = (UINT_PTR)baseModule + exportDirectory.AddressOfNameOrdinals;
	ExportNameTable = new DWORD[exportDirectory.NumberOfNames];
	ExportFunctionTable = new DWORD[exportDirectory.NumberOfFunctions];
	ExportOrdinalsTable = new WORD[exportDirectory.NumberOfFunctions];
	char TempChar;
	BOOL DONE = FALSE;
	std::string TempFunctionName;

	for (UINT_PTR i = 0; i < exportDirectory.NumberOfNames; i++) {
		DWORD arrays = 0;
		WORD ord = 0;
		ReadProcessMemory(hProcess, (LPCVOID)(ExportNameTableVA + i * sizeof(DWORD)), &arrays, sizeof(arrays), 0);
		ReadProcessMemory(hProcess, (LPCVOID)(ExportOrdinalTableVA + i * sizeof(WORD)), &ord, sizeof(ord), 0);
		ExportNameTable[i] = arrays;
		ExportOrdinalsTable[i] = ord;
	}
	for (UINT_PTR i = 0; i < exportDirectory.NumberOfFunctions; i++) {
		DWORD arrays = 0;
		ReadProcessMemory(hProcess, (LPCVOID)(ExportFunctionTableVA + i * sizeof(DWORD)), &arrays, sizeof(arrays), 0);
		ExportFunctionTable[i] = arrays;
	}
	std::map<std::string, UINT_PTR> funcList;
	for (DWORD i = 0; i < exportDirectory.NumberOfNames; i++) {
		std::string TempFunctionName;
		TempFunctionName.clear();
		BOOL DONE = FALSE;
		for (UINT_PTR j = 0; !DONE; j++) {
			ReadProcessMemory(hProcess, (LPCVOID)(baseModule + ExportNameTable[i] + j), &TempChar, sizeof(TempChar), NULL);
			if (TempChar == (CHAR)'\0')
				DONE = TRUE;
			else
			TempFunctionName.push_back(TempChar);

		}
		funcList.insert(std::make_pair(TempFunctionName, (baseModule+ExportFunctionTable[ExportOrdinalsTable[i]])));
	}
	nameToFunctionList.insert(std::make_pair(name, funcList));

}
RemoteHandle::RemoteHandle(DWORD pid):RemoteHandle(OpenProcess(PROCESS_ALL_ACCESS, TRUE, pid)) {
}
RemoteHandle::RemoteHandle(HANDLE hProcess) {
	this->hProcess = hProcess;
	if (!hProcess)
		throw FAILED_FUNCTION("FAILD OpenProcess");
	HMODULE hmod[1000];
	DWORD   dwSize;
	EnumProcessModulesEx(hProcess, hmod, sizeof(hmod), &dwSize, LIST_MODULES_ALL);
	char name[_MAX_PATH];
	for (DWORD i = 0; i < dwSize / sizeof(DWORD); i++) {
		GetModuleBaseNameA(hProcess, hmod[i], name, _MAX_PATH);
		nameToModuleList.insert(std::make_pair(std::string(name), hmod[i]));
	}
}
RemoteHandle::~RemoteHandle() {
	::CloseHandle(hProcess);
}

std::unordered_map<std::string, HMODULE> const RemoteHandle::getModuleList(){
	return nameToModuleList;
}
UINT_PTR RemoteHandle::getRemoteProcAdress(std::string mName, std::string fName)
{
	if (nameToFunctionList.count(mName))
		return (nameToFunctionList.at(mName).at(fName));
	else
		return getFunctionList(nameToModuleList.at(mName), mName,fName);
}
