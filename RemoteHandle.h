#pragma once
#include <Windows.h>
#include <stdexcept>
#include <unordered_map>
#include <map>
#include <string>
#include <psapi.h>
#pragma comment (lib, "psapi.lib")
namespace rth {
	class RemoteHandle {
	private:
		bool isOpenProcess = false;
		HANDLE hProcess;
		std::unordered_map<std::string, HMODULE> nameToModuleList;
		std::unordered_map<HMODULE,std::string> moduleToNameList;
		std::unordered_map<std::string, std::map<std::string, HMODULE>> nameToFunctionList;
		std::unordered_map<HMODULE, std::map<std::string,HMODULE>> moduleToFunctionList;
		HMODULE getFunctionList(HMODULE m,std::string name,std::string fName);
	public:
		RemoteHandle(DWORD pid);
		RemoteHandle(HANDLE hProcess);
		~RemoteHandle();
		std::unordered_map<std::string, HMODULE> const getNameToModuleList();
		HMODULE getRemoteModuleHandle(std::string mName);
		HMODULE getRemoteProcAdress(std::string mName, std::string fName);
		HMODULE getRemoteProcAdress(HMODULE module, std::string fName);
		std::string getRemoteProcName(HMODULE module, HMODULE func);
	};

	class FAILED_FUNCTION :public std::runtime_error {
	public:
		FAILED_FUNCTION(const char* _Message):runtime_error(_Message){}
		DWORD returncode()
		{
			return GetLastError();
		}
	};
}