#pragma once

#include <Windows.h>
#include <iostream>
#include <cstdio>
#include <consoleapi.h>
#include <vector>
#include <cstdint>
#include <numbers>
#include <fstream>
#include <format>

#include "minhook/MinHook.h"

#pragma comment(lib, "minhook/minhook.lib")

#define CREATEHOOK(Address, Hook, Og) MH_CreateHook((void*)(Address), Hook, (void**)(Og)); MH_EnableHook((void*)(Address));

#define LOG(LogLevel, Msg, ...) \
Util::Log(LogLevel, std::format(Msg, __VA_ARGS__));

enum LogLevel
{
	Log,
	Warning,
	Error
};

namespace Util
{
	static void Log(LogLevel level, std::string Msg)
	{
		HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

		switch (level)
		{
		case LogLevel::Warning:
			SetConsoleTextAttribute(hConsole, 14);
			break;
		case LogLevel::Error:
			SetConsoleTextAttribute(hConsole, 12);
			break;
		default:
			break;
		}

		std::cout << "LogJGS: " << Msg << "\n";

		SetConsoleTextAttribute(hConsole, 15);
	}

	static void InitConsole()
	{
		FILE* fDummy;
		AllocConsole();
		freopen_s(&fDummy, "CONIN$", "r", stdin);
		freopen_s(&fDummy, "CONOUT$", "w", stdout);
	}

	static uintptr_t FindPattern(const char* signature, bool bRelative = false, uint32_t offset = 0)
	{
		uintptr_t base_address = reinterpret_cast<uintptr_t>(GetModuleHandle(NULL));
		static auto patternToByte = [](const char* pattern)
		{
			auto bytes = std::vector<int>{};
			const auto start = const_cast<char*>(pattern);
			const auto end = const_cast<char*>(pattern) + strlen(pattern);

			for (auto current = start; current < end; ++current)
			{
				if (*current == '?')
				{
					++current;
					if (*current == '?') ++current;
					bytes.push_back(-1);
				}
				else { bytes.push_back(strtoul(current, &current, 16)); }
			}
			return bytes;
		};

		const auto dosHeader = (PIMAGE_DOS_HEADER)base_address;
		const auto ntHeaders = (PIMAGE_NT_HEADERS)((std::uint8_t*)base_address + dosHeader->e_lfanew);

		const auto sizeOfImage = ntHeaders->OptionalHeader.SizeOfImage;
		auto patternBytes = patternToByte(signature);
		const auto scanBytes = reinterpret_cast<std::uint8_t*>(base_address);

		const auto s = patternBytes.size();
		const auto d = patternBytes.data();

		for (auto i = 0ul; i < sizeOfImage - s; ++i)
		{
			bool found = true;
			for (auto j = 0ul; j < s; ++j)
			{
				if (scanBytes[i + j] != d[j] && d[j] != -1)
				{
					found = false;
					break;
				}
			}
			if (found)
			{
				uintptr_t address = reinterpret_cast<uintptr_t>(&scanBytes[i]);
				if (bRelative)
				{
					address = ((address + offset + 4) + *(int32_t*)(address + offset));
					return address;
				}
				return address;
			}
		}
		return NULL;
	}
}