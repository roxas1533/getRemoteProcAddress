#include "RemoteHandle.h"
#include <iostream>
using namespace rth;
int main() {
	RemoteHandle t = RemoteHandle((DWORD)8112);
	std::unordered_map<std::string, HMODULE> dllMap = t.getNameToModuleList();
	std::cout << t.getRemoteModuleHandle("USER32.dll")<<"\n";
	std::cout << (HMODULE)t.getRemoteProcAdress("USER32.dll", "CreateWindowExW");
}