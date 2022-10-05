#include <Windows.h>

#include "minhook/MinHook.h"

#pragma comment(lib, "minhook/minhook.lib")

#include "Util.h"

#include "Enums.h"
#include "Structs.h"

#include "Offsets.h"
#include "Helpers.h"

#include "Net.h"

#include "Hooks.h"

DWORD WINAPI MainThread(LPVOID)
{
    Util::InitConsole();

    LOG(Log, "Setting up!");

    UE4::Init();
    Net::Init();
    MH_Initialize();
    
    LOG(Log, "Setup!");

    UE4::Functions::Init();
    UE4::Offsets::Init();

    auto GameInstance = (*(UE4::UObject**)(__int64(Helpers::GetEngine()) + UE4::Offsets::GameInstanceOffset));
    auto LocalPlayers = ((UE4::TArray<UE4::UObject*>*)(__int64(GameInstance) + UE4::Offsets::LocalPlayersOffset));
    auto LocalPlayer = LocalPlayers->operator[](0);
    auto PlayerController = *(UE4::UObject**)(__int64(LocalPlayer) + UE4::Offsets::PlayerControllerOffset);

    Helpers::SwitchLevel(PlayerController, Helpers::GetMapName());

    LocalPlayers->Remove(0);

    Hooks::Init();

    return 0;
}

BOOL APIENTRY DllMain(HMODULE mod, DWORD reason, LPVOID res)
{
    if (reason == 1)
        CreateThread(0, 0, MainThread, mod, 0, 0);

    return TRUE;
}