#pragma once

namespace Helpers
{
	static UE4::UObject* GetEngine()
	{
		static UE4::UObject* Engine = UE4::FindObject("FortEngine_");
		return Engine;
	}

	static UE4::UObject* GetWorld()
	{
		auto Engine = GetEngine();
		auto GameViewport = *(UE4::UObject**)(__int64(Engine) + UE4::Offsets::GameViewportOffset);
		auto World = *(UE4::UObject**)(__int64(GameViewport) + UE4::Offsets::WorldOffset);

		return World;
	}

	static UE4::UObject* GetGameState()
	{
		return *(UE4::UObject**)(__int64(GetWorld()) + UE4::Offsets::GameStateOffset);
	}

	static UE4::UObject* GetGameMode()
	{
		return *(UE4::UObject**)(__int64(GetWorld()) + UE4::Offsets::GameModeOffset);
	}

	static UE4::UObject* GetGameSession()
	{
		return *(UE4::UObject**)(__int64(GetGameMode()) + UE4::Offsets::GameSessionOffset);
	}

	static void SwitchLevel(UE4::UObject* Controller, UE4::FString URL)
	{
		UE4::ProcessEvent(Controller, UE4::Functions::SwitchLevelFunc, &URL);
	}

	static void OnRep_bHasServerFinishedLoading(UE4::UObject* FortController)
	{
		UE4::ProcessEvent(FortController, UE4::Functions::OnRep_bHasServerFinishedLoadingFunc, nullptr);
	}

	static void OnRep_SquadId(UE4::UObject* FortPlayerState)
	{
		UE4::ProcessEvent(FortPlayerState, UE4::Functions::OnRep_SquadIdFunc, nullptr);
	}

	static void OnRep_bHasStartedPlaying(UE4::UObject* FortPlayerState)
	{
		UE4::ProcessEvent(FortPlayerState, UE4::Functions::OnRep_bHasStartedPlayingFunc, nullptr);
	}

	static void OnRep_CharacterParts(UE4::UObject* FortPlayerState)
	{
		UE4::ProcessEvent(FortPlayerState, UE4::Functions::OnRep_CharacterPartsFunc, nullptr);
	}

	static void Possess(UE4::UObject* Controller, UE4::UObject* Pawn)
	{
		UE4::ProcessEvent(Controller, UE4::Functions::PossessFunc, &Pawn);
	}

	static void ServerChoosePart(UE4::UObject* Pawn, UE4::EFortCustomPartType Part, UE4::UObject* ChosenCharacterPart)
	{
		struct {
			UE4::EFortCustomPartType Part;
			UE4::UObject* ChosenCharacterPart;
		}params;
		params.Part = Part;
		params.ChosenCharacterPart = ChosenCharacterPart;

		UE4::ProcessEvent(Pawn, UE4::Functions::ServerChoosePartFunc, &params);
	}

	static UE4::FQuat RotatorToQuat(UE4::FRotator Rotator)
	{
		UE4::FQuat Quat;

		auto DEG_TO_RAD = 3.14159 / 180;
		auto DIVIDE_BY_2 = DEG_TO_RAD / 2;

		auto SP = sin(Rotator.Pitch * DIVIDE_BY_2);
		auto CP = cos(Rotator.Pitch * DIVIDE_BY_2);

		auto SY = sin(Rotator.Yaw * DIVIDE_BY_2);
		auto CY = cos(Rotator.Yaw * DIVIDE_BY_2);

		auto SR = sin(Rotator.Roll * DIVIDE_BY_2);
		auto CR = cos(Rotator.Roll * DIVIDE_BY_2);

		Quat.X = CR * SP * SY - SR * CP * CY;
		Quat.Y = -CR * SP * CY - SR * CP * SY;
		Quat.Z = CR * CP * SY - SR * SP * CY;
		Quat.W = CR * CP * CY + SR * SP * SY;

		return Quat;
	}

	static UE4::UObject* BeginDeferredActorSpawnFromClass(UE4::UObject* WorldContextObject, UE4::UObject* ActorClass, UE4::FTransform Transform, UE4::ESpawnActorCollisionHandlingMethod SpawnActorCollisionHandlingMethod, UE4::UObject* Owner)
	{
		struct {
			UE4::UObject* WorldContextObject;
			UE4::UObject* ActorClass;
			UE4::FTransform Transform;
			UE4::ESpawnActorCollisionHandlingMethod SpawnActorCollisionHandlingMethod;
			UE4::UObject* Owner;
			UE4::UObject* ReturnValue;
		}params;
		params.WorldContextObject = WorldContextObject;
		params.ActorClass = ActorClass;
		params.Transform = Transform;
		params.SpawnActorCollisionHandlingMethod = SpawnActorCollisionHandlingMethod;
		params.Owner = Owner;

		UE4::ProcessEvent(UE4::Offsets::Default__GameplayStatics, UE4::Functions::BeginDeferredActorSpawnFromClassFunc, &params);

		return params.ReturnValue;
	}

	static UE4::UObject* FinishSpawningActor(UE4::UObject* Actor, UE4::FTransform Transform)
	{
		struct {
			UE4::UObject* Actor;
			UE4::FTransform Transform;
			UE4::UObject* ReturnValue;
		}params;
		params.Actor = Actor;
		params.Transform = Transform;

		UE4::ProcessEvent(UE4::Offsets::Default__GameplayStatics, UE4::Functions::FinishSpawningActorFunc, &params);

		return params.ReturnValue;
	}

	static UE4::UObject* SpawnActor(UE4::UObject* Class, UE4::FVector Location, UE4::FRotator Rotation)
	{
		UE4::FTransform Transform;
		Transform.Rotation = RotatorToQuat(Rotation);
		Transform.Translation = Location;
		Transform.Scale3D = { 1,1,1 };

		auto Actor = BeginDeferredActorSpawnFromClass(GetWorld(), Class, Transform, UE4::ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn, nullptr);
		FinishSpawningActor(Actor, Transform);
		return Actor;
	}

	static UE4::UObject* SpawnObject(UE4::UObject* Class, UE4::UObject* Outer)
	{
		struct {
			UE4::UObject* Class;
			UE4::UObject* Outer;
			UE4::UObject* ReturnValue;
		}params;
		params.Class = Class;
		params.Outer = Outer;

		UE4::ProcessEvent(UE4::Offsets::Default__GameplayStatics, UE4::Functions::SpawnObjectFunc, &params);

		return params.ReturnValue;
	}

	static void OnRep_CurrentPlaylistData(UE4::UObject* GameState)
	{
		UE4::ProcessEvent(GameState, UE4::Functions::OnRep_CurrentPlaylistDataFunc, nullptr);
	}

	static void OnRep_CurrentPlaylistInfo(UE4::UObject* GameState)
	{
		UE4::ProcessEvent(GameState, UE4::Functions::OnRep_CurrentPlaylistInfoFunc, nullptr);
	}

	static void OnRep_GamePhase(UE4::UObject* GameState, int OldPhase)
	{
		UE4::ProcessEvent(GameState, UE4::Functions::OnRep_GamePhaseFunc, &OldPhase);
	}

	static UE4::FString GetMapName()
	{
		if (UE4::EngineVersion >= 4.24)
			return L"Apollo_Terrain";
		else
			return L"Athena_Terrain";
	}

	static void MarkArrayDirty(void* FastArray)
	{
		auto ArrayReplicationKey = (int*)(__int64(FastArray) + 0x54);

		auto CachedNumItems = UE4::EngineVersion >= 4.23 ? (int*)(__int64(FastArray) + 0xF8) : (int*)(__int64(FastArray) + 0xA8);
		auto CachedNumItemsToConsiderForWriting = UE4::EngineVersion >= 4.23 ? (int*)(__int64(FastArray) + 0xFC) : (int*)(__int64(FastArray) + 0xAC);

		*CachedNumItems = 0;
		*CachedNumItemsToConsiderForWriting = 0;

		++*ArrayReplicationKey;
		if (*ArrayReplicationKey == 0)
			++*ArrayReplicationKey;
	}

	static void MarkItemDirty(void* FastArray, void* Item)
	{
		auto ReplicationID = (int*)(__int64(Item));
		auto ReplicationKey = (int*)(__int64(Item) + UE4::Offsets::ReplicationKeyOffset);
		auto IDCounter = (int*)(__int64(FastArray) + 0x50);

		if (*ReplicationID == 0)
		{
			*ReplicationID = ++*IDCounter;
			if (*IDCounter == 0)
				++*IDCounter;
		}

		++*ReplicationKey;

		MarkArrayDirty(FastArray);
	}

	static void SetPlaylist(UE4::UObject* GameState, UE4::UObject* Playlist)
	{
		if (UE4::FortniteVersion > 5)
		{
			auto CurrentPlaylistInfo = (void*)(__int64(GameState) + UE4::Offsets::CurrentPlaylistInfoOffset);
			*(UE4::UObject**)(__int64(CurrentPlaylistInfo) + UE4::Offsets::BasePlaylistOffset) = Playlist;
			*(UE4::UObject**)(__int64(CurrentPlaylistInfo) + UE4::Offsets::OverridePlaylistOffset) = Playlist;
			++*(int*)(__int64(CurrentPlaylistInfo) + UE4::Offsets::PlaylistReplicationKeyOffset);
			MarkArrayDirty(CurrentPlaylistInfo);
			OnRep_CurrentPlaylistInfo(GameState);
		}
		else {
			auto CurrentPlaylistData = (UE4::UObject**)(__int64(GameState) + UE4::Offsets::CurrentPlaylistDataOffset);
			*CurrentPlaylistData = Playlist;
			OnRep_CurrentPlaylistData(GameState);
		}
	}

	static void InitMatch()
	{
		auto SoloPlaylist = UE4::FindObject("Playlist_DefaultSolo.Playlist_DefaultSolo");
		//SetPlaylist(GetGameState(), SoloPlaylist);

		auto GamePhase = (int*)(__int64(GetGameState()) + UE4::Offsets::GamePhaseOffset);
		*GamePhase = 0;
		OnRep_GamePhase(GetGameState(), 0);

		auto MaxPlayers = (int*)(__int64(GetGameSession()) + UE4::Offsets::MaxPlayersOffset);
		*MaxPlayers = 100;
	}

	static UE4::FVector GetPlayerStartLocation()
	{
		return { 0,0,5000 };
	}
}