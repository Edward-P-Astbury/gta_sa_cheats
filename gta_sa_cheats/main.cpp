#include <iostream>
#include <Windows.h>
#include <TlHelp32.h>
#include "Offsets.h"
#include "GameMemoryAddresses.h"

void ConsoleErrorMessage(std::string);
uintptr_t GetModuleBaseAddress(DWORD, const wchar_t*);
template <class T>
void wpm(T, DWORD);
template <class T>
T rpm(T);
uintptr_t GetPlayerBaseAddress();

HANDLE handleProcess;
DWORD processID;

int main()
{
	HWND hwnd = FindWindowA(NULL, "GTA: San Andreas");

	if (hwnd == NULL)
	{
		ConsoleErrorMessage("Cannot find the game window.");
	}
	else
	{
		GetWindowThreadProcessId(hwnd, &processID);
		handleProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processID);

		if (processID == NULL)
		{
			ConsoleErrorMessage("Cannot retrieve process.");
		}

		while (!GetAsyncKeyState(VK_END))
		{
			if (GetAsyncKeyState(74)) //'j'
			{
				float health;
				ReadProcessMemory(handleProcess, (LPCVOID)(GetPlayerBaseAddress() + offset_HEALTH), &health, sizeof(health), NULL);
				std::cout << "Health value: " << health << std::endl;

				wpm(100.0f, GetPlayerBaseAddress() + offset_HEALTH);
			}
			if (GetAsyncKeyState(75)) //'k'
			{
				wpm(99999999, address_Money);
			}
			if (GetAsyncKeyState(76)) //'l'
			{
				wpm(100.0f, GetPlayerBaseAddress() + offset_ARMOUR);
			}
			if (GetAsyncKeyState(77)) //'m'
			{
				wpm(1, GetPlayerBaseAddress() + offset_WEAPONSLOT_0);
				wpm(22, GetPlayerBaseAddress() + offset_WEAPONSLOT_2);
				wpm(15, GetPlayerBaseAddress() + offset_TOTAL_PISTOL_AMMO);
			}

			Sleep(1000);
		}
	}

	CloseHandle(handleProcess);

	return 0;
}

void ConsoleErrorMessage(std::string message)
{
	std::cout << message << std::endl;
	Sleep(3000);
	exit(0);
}

uintptr_t GetModuleBaseAddress(DWORD processId, const wchar_t* moduleName)
{
	uintptr_t moduleBaseAddress = NULL;
	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, processId);

	if (hSnap != INVALID_HANDLE_VALUE)
	{
		MODULEENTRY32 moduleEntry;
		moduleEntry.dwSize = sizeof(moduleEntry);

		if (Module32First(hSnap, &moduleEntry))
		{
			do
			{
				if (!_wcsicmp(moduleEntry.szModule, moduleName))
				{
					moduleBaseAddress = (uintptr_t)moduleEntry.modBaseAddr;
					break;
				}
			} while (Module32First(hSnap, &moduleEntry));
		}
	}
	CloseHandle(hSnap);

	return moduleBaseAddress;
}

template <class T>
void wpm(T valueToWrite, DWORD addressToWrite)
{
	WriteProcessMemory(handleProcess, (LPVOID)addressToWrite, &valueToWrite, sizeof(T), NULL);
}

template <class T>
T rpm(T addressToRead)
{
	T rpmBuffer;

	ReadProcessMemory(handleProcess, (LPCVOID)addressToRead, &rpmBuffer, sizeof(T), NULL);

	return rpmBuffer;
}

uintptr_t GetPlayerBaseAddress()
{
	return rpm<uintptr_t>(GetModuleBaseAddress(processID, L"gta_sa.exe") + address_CPed);
}