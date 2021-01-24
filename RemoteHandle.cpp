#include "RemoteHandle.h"
using namespace rth;
HMODULE RemoteHandle::getFunctionList(HMODULE m, std::string name, std::string fName) {
	UINT_PTR baseModule = (UINT64)m;
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
	ExportTable = (ntHeader.OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT]).VirtualAddress;
	ReadProcessMemory(hProcess, (LPCVOID)(baseModule + ExportTable), &exportDirectory, sizeof(exportDirectory), 0);

	ExportFunctionTableVA = (UINT_PTR)baseModule + exportDirectory.AddressOfFunctions;
	ExportNameTableVA = (UINT_PTR)baseModule + exportDirectory.AddressOfNames;
	ExportOrdinalTableVA = (UINT_PTR)baseModule + exportDirectory.AddressOfNameOrdinals;
	ExportNameTable = new DWORD[exportDirectory.NumberOfNames];
	ExportFunctionTable = new DWORD[exportDirectory.NumberOfFunctions];
	ExportOrdinalsTable = new WORD[exportDirectory.NumberOfFunctions];
	char TempChar;
	BOOL DONE = FALSE;
	std::string TempFunctionName;
	HMODULE find = 0;
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
	std::map<std::string, HMODULE> funcList;
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
		funcList.insert(std::make_pair(TempFunctionName, (HMODULE)(baseModule + ExportFunctionTable[ExportOrdinalsTable[i]])));
		if (TempFunctionName == fName)
			find = HMODULE(baseModule + ExportFunctionTable[ExportOrdinalsTable[i]]);
	}
	nameToFunctionList.insert(std::make_pair(name, funcList));
	moduleToFunctionList.insert(std::make_pair(m, funcList));
	return find;
}
RemoteHandle::RemoteHandle(DWORD pid) :RemoteHandle(OpenProcess(PROCESS_ALL_ACCESS, TRUE, pid)) {
	isOpenProcess = true;
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
		moduleToNameList.insert(std::make_pair(hmod[i], std::string(name)));
	}
}
RemoteHandle::~RemoteHandle() {
	if (isOpenProcess)
		::CloseHandle(hProcess);
}

std::unordered_map<std::string, HMODULE> const RemoteHandle::getNameToModuleList() {
	return nameToModuleList;
}

HMODULE rth::RemoteHandle::getRemoteModuleHandle(std::string mName)
{
	try {
		return nameToModuleList.at(mName);
	}
	catch (std::out_of_range) {
		return 0;
	}
}

HMODULE RemoteHandle::getRemoteProcAdress(std::string mName, std::string fName)
{
	if (nameToFunctionList.count(mName)) {
		try {
			HMODULE find = nameToFunctionList.at(mName).at(fName);
			return find;
		}
		catch (std::out_of_range e) {
			return 0;
		}
	}
	else
		return getFunctionList(nameToModuleList.at(mName), mName, fName);
}

HMODULE rth::RemoteHandle::getRemoteProcAdress(HMODULE module, std::string fName)
{
	return getRemoteProcAdress(moduleToNameList.at(module), fName);
}

std::string rth::RemoteHandle::getRemoteProcName(HMODULE module, HMODULE func)
{
	std::map<std::string, HMODULE>& temp = moduleToFunctionList.at(module);
	for (auto m : temp) {
		if (m.second == func)
			return m.first;
	}
	return "";
}