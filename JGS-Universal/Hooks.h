#pragma once

namespace Hooks
{
	DWORD WINAPI TickThread(LPVOID)
	{
		static bool bStartedAircraft = false;

		while (true)
		{
			if (GetAsyncKeyState(VK_F1) & 1 && !bStartedAircraft)
			{
				bStartedAircraft = true;


			}
		}

		return 0;
	}

	void (*ProcessEventOG)(void*, void*, void*);
	void ProcessEventHook(UE4::UObject* pObject, UE4::UObject* pFunction, void* pParams)
	{
		auto FuncName = pFunction->GetName();

		if (FuncName == "ReadyToStartMatch")
		{
			static bool bSetup = false;
			if (!bSetup)
			{
				bSetup = true;

				Net::Listen();

				CreateThread(0, 0, TickThread, 0, 0, 0);
			}
		}

		return ProcessEventOG(pObject, pFunction, pParams);
	}

	static void Init()
	{
		CREATEHOOK(UE4::ProcessEventAddr, ProcessEventHook, &ProcessEventOG);
	}
}