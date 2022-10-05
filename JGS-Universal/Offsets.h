#pragma once

//Cached functions/offsets

namespace UE4
{
	namespace Offsets
	{
		static UObject* Default__GameplayStatics;

		static UObject* OnlineBeaconHostClass;

		static int32_t GameInstanceOffset;
		static int32_t LocalPlayersOffset;
		static int32_t PlayerControllerOffset;
		static int32_t GameViewportOffset;
		static int32_t WorldOffset;
		static int32_t ReplicationDriverOffset;
		static int32_t ListenPortOffset;
		static int32_t NetDriverNameOffset;
		static int32_t WorldOffset2;
		static int32_t NetDriverOffset;
		static int32_t NetDriverOffset2;
		static int32_t LevelCollectionsOffset;
		static int32_t ReplicationIDOffset;
		static int32_t ReplicationKeyOffset;
		static int32_t CurrentPlaylistDataOffset;
		static int32_t PlaylistReplicationKeyOffset;
		static int32_t BasePlaylistOffset;
		static int32_t OverridePlaylistOffset;
		static int32_t CurrentPlaylistInfoOffset;
		static int32_t GameStateOffset;
		static int32_t GameModeOffset;
		static int32_t GamePhaseOffset;
		static int32_t GameSessionOffset;
		static int32_t MaxPlayersOffset;

		static void Init()
		{
			Default__GameplayStatics = FindObject("Default__GameplayStatics");

			OnlineBeaconHostClass = FindObject("OnlineBeaconHost", true, false);

			GameInstanceOffset = FindOffset("GameEngine", "GameInstance");
			LocalPlayersOffset = FindOffset("GameInstance", "LocalPlayers");
			GameViewportOffset = FindOffset("Engine", "GameViewport");
			WorldOffset = FindOffset("GameViewportClient", "World");
			PlayerControllerOffset = FindOffset("Player", "PlayerController");
			ReplicationDriverOffset = FindOffset("NetDriver", "ReplicationDriver");
			ListenPortOffset = FindOffset("OnlineBeaconHost", "ListenPort");
			NetDriverNameOffset = FindOffset("NetDriver", "NetDriverName");
			WorldOffset2 = FindOffset("NetDriver", "World");
			NetDriverOffset = FindOffset("OnlineBeacon", "NetDriver");
			NetDriverOffset2 = FindOffset("World", "NetDriver");
			LevelCollectionsOffset = FindOffset("World", "LevelCollections");
			ReplicationIDOffset = FindOffset("FastArraySerializerItem", "ReplicationID");
			ReplicationKeyOffset = FindOffset("FastArraySerializerItem", "ReplicationKey");
			CurrentPlaylistDataOffset = FindOffset("FortGameStateAthena", "CurrentPlaylistData");
			PlaylistReplicationKeyOffset = FindOffset("PlaylistPropertyArray", "PlaylistReplicationKey");
			BasePlaylistOffset = FindOffset("PlaylistPropertyArray", "BasePlaylist");
			OverridePlaylistOffset = FindOffset("PlaylistPropertyArray", "OverridePlaylist");
			CurrentPlaylistInfoOffset = FindOffset("FortGameStateAthena", "CurrentPlaylistInfo");
			GameStateOffset = FindOffset("World", "GameState");
			GameModeOffset = FindOffset("World", "AuthorityGameMode");
			GamePhaseOffset = FindOffset("FortGameStateAthena", "GamePhase");
			GameSessionOffset = FindOffset("GameModeBase", "GameSession");
			MaxPlayersOffset = FindOffset("GameSession", "MaxPlayers");
		}
	}

	namespace Functions
	{
		static UObject* SwitchLevelFunc;
		static UObject* OnRep_bHasServerFinishedLoadingFunc;
		static UObject* OnRep_SquadIdFunc;
		static UObject* OnRep_bHasStartedPlayingFunc;
		static UObject* OnRep_CharacterPartsFunc;
		static UObject* PossessFunc;
		static UObject* ServerChoosePartFunc;
		static UObject* BeginDeferredActorSpawnFromClassFunc;
		static UObject* FinishSpawningActorFunc;
		static UObject* SpawnObjectFunc;
		static UObject* OnRep_CurrentPlaylistDataFunc;
		static UObject* OnRep_CurrentPlaylistInfoFunc;
		static UObject* OnRep_GamePhaseFunc;

		static void Init()
		{
			SwitchLevelFunc = FindFunction("PlayerController", "SwitchLevel");
			OnRep_bHasServerFinishedLoadingFunc = FindFunction("FortPlayerController", "OnRep_bHasServerFinishedLoading");
			OnRep_SquadIdFunc = FindFunction("FortPlayerStateAthena", "OnRep_SquadId");
			OnRep_bHasStartedPlayingFunc = FindFunction("FortPlayerState", "OnRep_bHasStartedPlaying");
			OnRep_CharacterPartsFunc = FindFunction("FortPlayerState", "OnRep_CharacterParts");
			PossessFunc = FindFunction("Controller", "Possess");
			ServerChoosePartFunc = FindFunction("FortPlayerPawn", "ServerChoosePart");
			BeginDeferredActorSpawnFromClassFunc = FindFunction("GameplayStatics", "BeginDeferredActorSpawnFromClass");
			FinishSpawningActorFunc = FindFunction("GameplayStatics", "FinishSpawningActor");
			SpawnObjectFunc = FindFunction("GameplayStatics", "SpawnObject");
			OnRep_CurrentPlaylistDataFunc = FindFunction("FortGameStateAthena", "OnRep_CurrentPlaylistData");
			OnRep_CurrentPlaylistInfoFunc = FindFunction("FortGameStateAthena", "OnRep_CurrentPlaylistInfo");
			OnRep_GamePhaseFunc = FindFunction("FortGameStateAthena", "OnRep_GamePhase");
		}
	}
}