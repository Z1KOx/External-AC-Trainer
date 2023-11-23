#include "proc/proc.h"
#include <iostream>
#include <thread>

void loadTrainer(DWORD& procId, std::uintptr_t& modBaseAddr, HANDLE& hProcess, std::uintptr_t& localPlayer)
{
	procId = getProcId(L"ac_client.exe");

	if (!procId)
		std::cout << "\tSearching for game window AssaultCube\n";
	while (true)
	{
		procId = getProcId(L"ac_client.exe");
		if (procId)
			break;
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
	std::cout << "\tWindow  found [AssaultCube]\n";
	std::cout << "\tProcess found [" << procId << "]\n";

	modBaseAddr = getModuleBaseAddr(L"ac_client.exe", procId);
	if (!modBaseAddr)
		std::cout << "\tSearching module\n";
	while (true)
	{
		modBaseAddr = getModuleBaseAddr(L"ac_client.exe", procId);
		if (modBaseAddr)
			break;
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
	std::cout << "\tModule  found [" << modBaseAddr << "]\n";

	hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, procId);

	localPlayer = modBaseAddr + 0x0018AC00;
}

void loadOffsets(const HANDLE& hProcess, const uintptr_t& localPlayer, std::vector<std::uintptr_t>& addr)
{
	std::uintptr_t rifleAmmoAddr{ NULL }, healthAddr{ NULL }, grenadeAddr{ NULL }, armorAddr{ NULL }, pistolAmmoAddr{ NULL }, pistolClipAddr{ NULL }, rifleClipAddr{ NULL };
	addr = { rifleAmmoAddr, healthAddr, grenadeAddr, armorAddr, pistolAmmoAddr, pistolClipAddr, rifleClipAddr };

	addr[0] = findDMAAddy(hProcess, localPlayer, { 0x140 });
	addr[1] = findDMAAddy(hProcess, localPlayer, { 0xEC });
	addr[2] = findDMAAddy(hProcess, localPlayer, { 0x144 });
	addr[3] = findDMAAddy(hProcess, localPlayer, { 0xF0 });
	addr[4] = findDMAAddy(hProcess, localPlayer, { 0x12C });
	addr[5] = findDMAAddy(hProcess, localPlayer, { 0x108 });
	addr[6] = findDMAAddy(hProcess, localPlayer, { 0x11C });
}

int main()
{
	DWORD procId{ NULL };
	std::uintptr_t modBaseAddr{ NULL };
	HANDLE hProcess;
	std::uintptr_t localPlayer{ NULL };
	loadTrainer(procId, modBaseAddr, hProcess, localPlayer);

	std::cout << "\n\n\tNumpad1 = Rifle Ammo\n";
	std::cout << "\tNumpad2 = Health\n";
	std::cout << "\tNumpad3 = Grenade\n";
	std::cout << "\tNumpad4 = Armor\n";
	std::cout << "\tNumpad5 = Pistol Ammo\n";
	std::cout << "\tNumpad6 = Pistol Clip\n";
	std::cout << "\tNumpad7 = Rifle Clip\n";

	std::vector<std::uintptr_t> addr;
	constexpr int writeValue{ 999 };
	while (true)
	{
		loadOffsets(hProcess, localPlayer, addr);

		if (GetAsyncKeyState(VK_NUMPAD1) & 1)
			WriteProcessMemory(hProcess, (BYTE*)addr[0], &writeValue, sizeof(writeValue), nullptr);
		if (GetAsyncKeyState(VK_NUMPAD2) & 1)
			WriteProcessMemory(hProcess, (BYTE*)addr[1], &writeValue, sizeof(writeValue), nullptr);
		if (GetAsyncKeyState(VK_NUMPAD3) & 1)
			WriteProcessMemory(hProcess, (BYTE*)addr[2], &writeValue, sizeof(writeValue), nullptr);
		if (GetAsyncKeyState(VK_NUMPAD4) & 1)
			WriteProcessMemory(hProcess, (BYTE*)addr[3], &writeValue, sizeof(writeValue), nullptr);
		if (GetAsyncKeyState(VK_NUMPAD5) & 1)
			WriteProcessMemory(hProcess, (BYTE*)addr[4], &writeValue, sizeof(writeValue), nullptr);
		if (GetAsyncKeyState(VK_NUMPAD6) & 1)
			WriteProcessMemory(hProcess, (BYTE*)addr[5], &writeValue, sizeof(writeValue), nullptr);
		if (GetAsyncKeyState(VK_NUMPAD7) & 1)
			WriteProcessMemory(hProcess, (BYTE*)addr[6], &writeValue, sizeof(writeValue), nullptr);

		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}

	return 0;
}