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

				auto GamePhase = (int*)(__int64(Helpers::GetGameState()) + UE4::Offsets::GamePhaseOffset);
				*GamePhase = 2;
				Helpers::OnRep_GamePhase(Helpers::GetGameState(), 0);
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
				Helpers::InitMatch();

				CreateThread(0, 0, TickThread, 0, 0, 0);
			}
		}

		return ProcessEventOG(pObject, pFunction, pParams);
	}

	static void (*SetClientLoginState)(UE4::UObject* Connection, uint8_t State);
	static void SetClientLoginStateHook(UE4::UObject* Connection, uint8_t State)
	{
		if (State == 3)
		{
			auto PlayerController = *(UE4::UObject**)(__int64(Connection) + UE4::Offsets::PlayerControllerOffset);
			if (PlayerController)
			{
				auto NewPawn = Helpers::SpawnActor(UE4::Offsets::PlayerPawnClass, Helpers::GetPlayerStartLocation(), {});
				Helpers::Possess(PlayerController, NewPawn);

				*(bool*)(__int64(PlayerController) + UE4::Offsets::bHasServerFinishedLoadingOffset) = true;
				Helpers::OnRep_bHasServerFinishedLoading(PlayerController);


			}
		}

		return SetClientLoginState(Connection, State);
	}

	static void Init()
	{
		CREATEHOOK(UE4::ProcessEventAddr, ProcessEventHook, &ProcessEventOG);
	}
}