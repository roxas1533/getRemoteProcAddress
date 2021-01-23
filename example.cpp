#include "RemoteHandle.h"
#include <iostream>
using namespace rth;
int main() {
	RemoteHandle t = RemoteHandle((DWORD)8112);
	std::unordered_map<std::string, HMODULE> dllMap = t.getModuleList();
	std::cout << t.getRemoteModule("USER32.dll")<<"\n";
	std::cout << (HMODULE)t.getRemoteProcAdress("USER32.dll", "CreateWindowExW");
}