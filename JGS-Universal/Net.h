#pragma once

namespace Net
{
	bool (*InitHost)(UE4::UObject* BeaconHost);
	bool (*InitListen)(UE4::UObject* NetDriver, void* Notify, UE4::FURL Url, bool, UE4::FString&);
	void (*SetWorld)(UE4::UObject* NetDriver, UE4::UObject* World);
	void (*SetReplicationDriver)(UE4::UObject* NetDriver, UE4::UObject* ReplicationDriver); //If the repgraph doesnt auto spawn
	void (*ServerReplicateActors)(UE4::UObject* ReplicationGraph);

	static void Listen()
	{
		auto Beacon = Helpers::SpawnActor(UE4::Offsets::OnlineBeaconHostClass, {}, {});

		if (!Beacon)
		{
			LOG(Error, "Failed to Spawn BeaconHost!");
			return;
		}

		*(int*)(__int64(Beacon) + UE4::Offsets::ListenPortOffset) = 6969;
		
		if (!InitHost(Beacon))
		{
			LOG(Error, "Failed to InitHost!");
			return;
		}

		auto NetDriver = *(UE4::UObject**)(__int64(Beacon) + UE4::Offsets::NetDriverOffset);
		*(UE4::FName*)(__int64(NetDriver) + UE4::Offsets::NetDriverNameOffset) = UE4::FName{ 282, 0 };
		*(UE4::UObject**)(__int64(NetDriver) + UE4::Offsets::WorldOffset2) = Helpers::GetWorld();

		UE4::FURL Url;
		Url.Port = 7777;

		UE4::FString Error;

		if (!InitListen(NetDriver, Helpers::GetWorld(), Url, false, Error))
		{
			LOG(LogLevel::Error, "Failed to InitListen: {}", Error.ToString());
			return;
		}

		*(UE4::UObject**)(__int64(Helpers::GetWorld()) + UE4::Offsets::NetDriverOffset2) = NetDriver;
		SetWorld(NetDriver, Helpers::GetWorld());

		if (UE4::EngineVersion > 4.20)
		{
			auto LevelCollections = (UE4::TArray<UE4::FLevelCollection>*)(__int64(Helpers::GetWorld()) + UE4::Offsets::LevelCollectionsOffset);
			LevelCollections->operator[](0).NetDriver = NetDriver;
			LevelCollections->operator[](1).NetDriver = NetDriver;
		}
		else {
			auto LevelCollections = (UE4::TArray<UE4::FLevelCollectionOld>*)(__int64(Helpers::GetWorld()) + UE4::Offsets::LevelCollectionsOffset);
			LevelCollections->operator[](0).NetDriver = NetDriver;
			LevelCollections->operator[](1).NetDriver = NetDriver;
		}

		auto ReplicationDriver = (UE4::UObject**)(__int64(NetDriver) + UE4::Offsets::ReplicationDriverOffset);
		if (!*ReplicationDriver && SetReplicationDriver)
		{
			auto NewRepGraph = Helpers::SpawnObject(UE4::FindObject("FortReplicationGraph", true, false), NetDriver);
			SetReplicationDriver(NetDriver, NewRepGraph);
			*ReplicationDriver = NewRepGraph;
		}

		if (!*ReplicationDriver)
		{
			LOG(Warning, "No replication driver, will not replicate!");
		}
	}

	static void (*TickFlush)(UE4::UObject* NetDriver);
	static void TickFlushHook(UE4::UObject* NetDriver)
	{
		auto RepGraph = *(UE4::UObject**)(__int64(NetDriver) + UE4::Offsets::ReplicationDriverOffset);
		if (RepGraph && ServerReplicateActors)
		{
			ServerReplicateActors(RepGraph);
		}

		return TickFlush(NetDriver);
	}

	static void Init()
	{
		InitHost = decltype(InitHost)(UE4::InitHostAddr);
		InitListen = decltype(InitListen)(UE4::InitListenAddr);
		SetWorld = decltype(SetWorld)(UE4::SetWorldAddr);
		SetReplicationDriver = decltype(SetReplicationDriver)(UE4::SetReplicationDriverAddr);
		ServerReplicateActors = decltype(ServerReplicateActors)(UE4::ServerReplicateActorsAddr);

		CREATEHOOK(UE4::TickFlushAddr, TickFlushHook, &TickFlush);
	}
}