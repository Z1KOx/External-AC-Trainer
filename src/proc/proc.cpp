#include "proc.h"

DWORD getProcId(const wchar_t* procName)
{
	DWORD procId{ NULL };
	const HANDLE hSnap{ CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL) };

	if (hSnap != INVALID_HANDLE_VALUE)
	{
		PROCESSENTRY32W procEntry;
		procEntry.dwSize = sizeof(procEntry);

		if (Process32FirstW(hSnap, &procEntry))
		{
			do
			{
				if (!_wcsicmp(procEntry.szExeFile, procName))
				{
					procId = procEntry.th32ProcessID;
					break;
				}

			} while (Process32NextW(hSnap, &procEntry));
		}
	}

	CloseHandle(hSnap);
	return procId;
}

std::uintptr_t getModuleBaseAddr(const wchar_t* modName, const DWORD procId)
{
	std::uintptr_t modBaseAddr{ NULL };
	const HANDLE hSnap{ CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, procId) };

	if (hSnap != INVALID_HANDLE_VALUE)
	{
		MODULEENTRY32W modEntry;
		modEntry.dwSize = sizeof(modEntry);

		if (Module32FirstW(hSnap, &modEntry))
		{
			do
			{
				if (!_wcsicmp(modEntry.szModule, modName))
				{
					modBaseAddr = (std::uintptr_t)modEntry.modBaseAddr;
					break;
				}

			} while (Module32NextW(hSnap, &modEntry));
		}
	}

	CloseHandle(hSnap);
	return modBaseAddr;
}

std::uintptr_t findDMAAddy(HANDLE hProc, uintptr_t ptr, const std::vector<unsigned int>& offsets)
{
	std::uintptr_t addr = ptr;
	for (unsigned int i = 0; i < offsets.size(); i++)
	{
		ReadProcessMemory(hProc, (BYTE*)addr, &addr, sizeof(addr), 0);
		addr += offsets[i];
	}
	return addr;
}