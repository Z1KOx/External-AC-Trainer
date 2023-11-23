#ifndef PROC_H
#define PROC_H

#include <windows.h>
#include <TlHelp32.h>
#include <vector>

DWORD getProcId(const wchar_t* procName);
std::uintptr_t getModuleBaseAddr(const wchar_t* modName, const DWORD procId);
std::uintptr_t findDMAAddy(HANDLE hProc, uintptr_t ptr, const std::vector<unsigned int>& offsets);

#endif