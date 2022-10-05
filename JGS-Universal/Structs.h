#pragma once

#include <string>
#include <locale>
#include <format>
#include <regex>

namespace UE4
{
	uintptr_t GObjectsAddr;
	uintptr_t FNameToStringAddr;
	uintptr_t FreeMemoryAddr;
	uintptr_t ProcessEventAddr;
	uintptr_t CreateDefaultObjectAddr;
	uintptr_t InitHostAddr;
	uintptr_t InitListenAddr;
	uintptr_t SetWorldAddr;
	uintptr_t SetReplicationDriverAddr; //Not required for all versions
	uintptr_t ServerReplicateActorsAddr;
	uintptr_t TickFlushAddr;
	uintptr_t GetNetModeAddr; //Todo get this for every version
	uintptr_t GiveAbilityAddr;
	uintptr_t InternalTryActivateAbilityAddr;
	uintptr_t SetClientLoginStateAddr;
	uintptr_t SpawnActorAddr;

	uint32_t NextOffset = 0x28;
	uint32_t SuperOffset = 0x30;
	uint32_t ChildrenOffset = 0x38;
	uint32_t PropertyOffsetOffset = 0x44;

	double FortniteVersion;
	double EngineVersion;

	class FFixedUObjectArray* OldObjects;
	class FChunkedFixedUObjectArray* NewObjects;

	void(*FNameToString)(void*, struct FString&);
	void(*FreeMemory)(void*);
	void(*ProcessEvent)(void*, void*, void*);

	template<class T>
	struct TArray
	{
		friend struct FString;

	public:
		inline TArray()
		{
			Data = nullptr;
			Count = Max = 0;
		};

		inline int Num() const
		{
			return Count;
		};

		inline T& operator[](int i)
		{
			return Data[i];
		};

		inline const T& operator[](int i) const
		{
			return Data[i];
		};

		inline bool IsValidIndex(int i) const
		{
			return i < Num();
		}

		inline int Add(T InputData, size_t Size = sizeof(T))
		{
			Data = (T*)std::realloc(Data, sizeof(T) * (Count + 1), 0);
			Data[Count++] = InputData;
			Max = Count;

			memcpy_s((T*)(__int64(Data) + (Count * Size)), Size, (void*)&InputData, Size);

			++Count;
			++Max;

			return Count;
		};

		inline bool Remove(int Index)
		{
			if (Index < Count)
			{
				if (Index != Count - 1)
					Data[Index] = Data[Count - 1];

				--Count;

				return true;
			}
			return false;
		}

		inline void RemoveAt(int Index, int Lenght = 1)
		{
			for (; Lenght != 0; --Lenght)
			{
				if (!Remove(Index++))
					break;
			}
		}

		T* Data;
		int32_t Count;
		int32_t Max;
	};

	struct FString : private TArray<wchar_t>
	{
		inline FString()
		{
		};

		FString(const wchar_t* other)
		{
			Max = Count = *other ? std::wcslen(other) + 1 : 0;

			if (Count)
			{
				Data = const_cast<wchar_t*>(other);
			}
		};

		inline bool IsValid() const
		{
			return Data != nullptr;
		}

		inline const wchar_t* c_str() const
		{
			return Data;
		}

		std::string ToString() const
		{
			auto length = std::wcslen(Data);

			std::string str(length, '\0');

			std::use_facet<std::ctype<wchar_t>>(std::locale()).narrow(Data, Data + length, '?', &str[0]);

			return str;
		}
	};

	template<class TEnum>
	class TEnumAsByte
	{
	public:
		inline TEnumAsByte()
		{
		}

		inline TEnumAsByte(TEnum _value)
			: value(static_cast<uint8_t>(_value))
		{
		}

		explicit inline TEnumAsByte(int32_t _value)
			: value(static_cast<uint8_t>(_value))
		{
		}

		explicit inline TEnumAsByte(uint8_t _value)
			: value(_value)
		{
		}

		inline operator TEnum() const
		{
			return (TEnum)value;
		}

		inline TEnum GetValue() const
		{
			return (TEnum)value;
		}

	private:
		uint8_t value;
	};

	struct FName
	{
		int32_t ComparisonIndex;
		int32_t Number;

		inline std::string ToString()
		{
			FString Buf;

			FNameToString(this, Buf);

			std::string String(Buf.ToString());

			FreeMemory((void*)Buf.c_str());

			return String;
		}
	};

	static struct UObject* GetStructSuper(UObject* Struct);

	struct UObject
	{
		void** VFT;
		int32_t ObjectFlags;
		int32_t InternalIndex;
		UObject* ClassPrivate;
		FName NamePrivate;
		UObject* OuterPrivate;

		inline std::string GetName()
		{
			return NamePrivate.ToString();
		}

		inline std::string GetFullName()
		{
			std::string Temp;

			for (auto Outer = OuterPrivate; Outer; Outer = Outer->OuterPrivate)
				Temp = std::format("{}.{}", Outer->GetName(), Temp);

			return std::format("{} {}{}", ClassPrivate->GetName(), Temp, GetName());
		}

		inline bool IsA(UObject* cmp)
		{
			for (auto Super = ClassPrivate; Super; Super = GetStructSuper(Super))
				if (Super == cmp)
					return true;

			return false;
		}
	};

	static UObject* GetFieldNext(UObject* Field)
	{
		return *(UObject**)(__int64(Field) + NextOffset);
	}

	static UObject* GetStructSuper(UObject* Struct)
	{
		return *(UObject**)(__int64(Struct) + SuperOffset);
	}

	static UObject* GetStructChildren(UObject* Struct)
	{
		return *(UObject**)(__int64(Struct) + ChildrenOffset);
	}

	static int32_t GetPropertyOffset(UObject* Property)
	{
		return *(int32_t*)(__int64(Property) + PropertyOffsetOffset);
	}

	struct FUObjectItem
	{
		UObject* Object;
		int32_t Flags;
		int32_t ClusterRootIndex;
		int32_t SerialNumber;
	};

	class FFixedUObjectArray
	{
		FUObjectItem* Objects;
		int32_t MaxElements;
		int32_t NumElements;

	public:

		FORCEINLINE int32_t Num() const
		{
			return NumElements;
		}

		FORCEINLINE int32_t Capacity() const
		{
			return MaxElements;
		}

		FORCEINLINE bool IsValidIndex(int32_t Index) const
		{
			return Index < Num() && Index >= 0;
		}

		FORCEINLINE UObject* GetByIndex(int32_t Index)
		{
			if (IsValidIndex(Index))
				return Objects[Index].Object;

			return nullptr;
		}
	};

	class FChunkedFixedUObjectArray
	{
		enum
		{
			NumElementsPerChunk = 64 * 1024,
		};

		FUObjectItem** Objects;
		FUObjectItem* PreAllocatedObjects;
		int32_t MaxElements;
		int32_t NumElements;
		int32_t MaxChunks;
		int32_t NumChunks;

	public:

		FORCEINLINE int32_t Num() const
		{
			return NumElements;
		}

		FORCEINLINE int32_t Capacity() const
		{
			return MaxElements;
		}

		FORCEINLINE bool IsValidIndex(int32_t Index) const
		{
			return Index < Num() && Index >= 0;
		}

		FORCEINLINE UObject* GetByIndex(int32_t Index)
		{
			const int32_t ChunkIndex = Index / NumElementsPerChunk;
			const int32_t WithinChunkIndex = Index % NumElementsPerChunk;

			if (!IsValidIndex(Index)) return nullptr;
			if (ChunkIndex > NumChunks) return nullptr;
			if (Index > MaxElements) return nullptr;

			FUObjectItem* Chunk = Objects[ChunkIndex];
			if (!Chunk) return nullptr;

			return (Chunk + WithinChunkIndex)->Object;
		}
	};

	static UObject* FindObject(std::string Name, bool bEqual = false, bool bFullName = true)
	{
		if (NewObjects)
		{
			for (int i = 0; i < NewObjects->Num(); i++)
			{
				auto Object = NewObjects->GetByIndex(i);

				if (!Object)
					continue;

				if (bEqual)
				{
					if (bFullName)
					{
						if (Object->GetFullName() == Name)
							return Object;
					}
					else {
						if (Object->GetName() == Name)
							return Object;
					}
				}
				else {
					if (bFullName)
					{
						if (Object->GetFullName().contains(Name))
							return Object;
					}
					else {
						if (Object->GetName().contains(Name))
							return Object;
					}
				}
			}
		}

		if (OldObjects)
		{
			for (int i = 0; i < OldObjects->Num(); i++)
			{
				auto Object = OldObjects->GetByIndex(i);

				if (!Object)
					continue;

				if (bEqual)
				{
					if (bFullName)
					{
						if (Object->GetFullName() == Name)
							return Object;
					}
					else {
						if (Object->GetName() == Name)
							return Object;
					}
				}
				else {
					if (bFullName)
					{
						if (Object->GetFullName().contains(Name))
							return Object;
					}
					else {
						if (Object->GetName().contains(Name))
							return Object;
					}
				}
			}
		}

		return nullptr;
	}

	static int32_t FindOffset(std::string ClassName, std::string PropertyName)
	{
		static auto PropertyClass = FindObject("Property", true, false);

		UObject* Class = FindObject(ClassName, true, false);

		int32_t RetValue = 0;

		if (Class)
		{
			auto Children = GetStructChildren(Class);
			
			while (Children)
			{
				if (Children->IsA(PropertyClass))
				{
					if (Children->GetName() == PropertyName)
					{
						RetValue = GetPropertyOffset(Children);
					}
				}

				Children = GetFieldNext(Children);
			}
		}

		if (RetValue == 0)
			LOG(Warning, "Failed to find {}::{} Offset!", ClassName, PropertyName);

		return RetValue;
	}

	static UObject* FindFunction(std::string ClassName, std::string FunctionName)
	{
		static auto FunctionClass = FindObject("Function", true, false);

		UObject* Class = FindObject(ClassName, true, false);

		UObject* RetValue = nullptr;

		if (Class)
		{
			auto Children = GetStructChildren(Class);

			while (Children)
			{
				if (Children->IsA(FunctionClass))
				{
					if (Children->GetName() == FunctionName)
					{
						RetValue = Children;
					}
				}

				Children = GetFieldNext(Children);
			}
		}

		if (RetValue == nullptr)
			LOG(Warning, "Failed to find function {}::{}", ClassName, FunctionName);

		return RetValue;
	}

	FString(*GetEngineVersion)();

	static void InitEngineVersion()
	{
		//4.20.0-4008490+++Fortnite+Release-3.5

		GetEngineVersion = decltype(GetEngineVersion)(Util::FindPattern("40 53 48 83 EC 20 48 8B D9 E8 ? ? ? ? 48 8B C8 41 B8 04 ? ? ? 48 8B D3"));
		
		std::string Temp = GetEngineVersion().ToString();

		auto FNVer = Temp.substr(Temp.find_last_of("-") + 1);
		auto EngineVer = Temp.erase(Temp.find_first_of("-"), Temp.length() - 1);

		if (EngineVer.find_first_of(".") != EngineVer.find_last_of("."))
			EngineVer = EngineVer.erase(EngineVer.find_last_of("."), 2);

		EngineVersion = std::stod(EngineVer);
		FortniteVersion = std::stod(FNVer);

		LOG(Log, "FortniteVersion: {}", FortniteVersion);
		LOG(Log, "EngineVersion: {}", EngineVer);
	}

	static void Init()
	{
		InitEngineVersion();

		if (EngineVersion == 4.20)
		{
			GObjectsAddr = Util::FindPattern("48 8B 05 ? ? ? ? 48 8D 14 C8 EB 02", true, 3);
			OldObjects = decltype(OldObjects)(GObjectsAddr);

			FNameToStringAddr = Util::FindPattern("48 89 5C 24 ? 57 48 83 EC 40 83 79 04 00 48 8B DA 48 8B F9");
			FNameToString = decltype(FNameToString)(FNameToStringAddr);

			FreeMemoryAddr = Util::FindPattern("48 85 C9 74 1D 4C 8B 05 ? ? ? ?");
			FreeMemory = decltype(FreeMemory)(FreeMemoryAddr);

			CreateDefaultObjectAddr = Util::FindPattern("4C 8B DC 57 48 81 EC ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 84 24 ? ? ? ? 48 83 B9 ? ? ? ? ? 48 8B F9 0F 85 ? ? ? ? 49 89 5B 10 48 8B 59 30");
			InitHostAddr = Util::FindPattern("48 8B C4 48 81 EC ? ? ? ? 48 89 58 18 4C 8D 05 ? ? ? ? 48 8B D9 48 89 78 F8 48 8D 48 88 45 33 C9");
			InitListenAddr = Util::FindPattern("48 89 5C 24 ? 48 89 74 24 ? 57 48 83 EC 50 48 8B BC 24 ? ? ? ? 49 8B F0 48 8B 01 48 8B D9");

			SetWorldAddr = Util::FindPattern("48 89 5C 24 ? 48 89 74 24 ? 57 48 83 EC 20 48 8B B1 ? ? ? ? 48 8B FA 48 8B D9 48 85 F6 74 5C");
			if (!SetWorldAddr)
				SetWorldAddr = Util::FindPattern("48 89 5C 24 ? 57 48 83 EC 20 48 8B FA 48 8B D9 48 8B 91 ? ? ? ? 48 85 D2 74 28");

			ServerReplicateActorsAddr = Util::FindPattern("4C 8B DC 55 49 8D AB ? ? ? ? 48 81 EC ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 85 ? ? ? ? 48 8B 41 58");
			if (!ServerReplicateActorsAddr)
				ServerReplicateActorsAddr = Util::FindPattern("4C 8B DC 55 49 8D AB ? ? ? ? 48 81 EC ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 85 ? ? ? ? 48 8B 41 58 45 33 C9");

			TickFlushAddr = Util::FindPattern("4C 8B DC 55 49 8D AB ? ? ? ? 48 81 EC ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 85 ? ? ? ? 49 89 5B 18 48 8D 05 ? ? ? ? 49 89 73 F0 33 F6");
			GiveAbilityAddr = Util::FindPattern("48 89 5C 24 ? 56 57 41 56 48 83 EC 20 83 B9 ? ? ? ? ? 49 8B F0 4C 8B F2 48 8B D9 7E 61 48 63 BB ? ? ? ? 48 81 C3 ? ? ? ?");
			InternalTryActivateAbilityAddr = Util::FindPattern("4C 89 4C 24 ? 4C 89 44 24 ? 89 54 24 10 55 53 56 57 41 54 41 57 48 8D AC 24 ? ? ? ? 48 81 EC ? ? ? ?");
			SetClientLoginStateAddr = Util::FindPattern("48 89 74 24 ? 57 48 83 EC 40 8B FA 48 8B F1 39 91 ? ? ? ? 75 50 80 3D ? ? ? ? ? 0F 82 ? ? ? ? 48 8B 05 ? ? ? ?");

			ProcessEventAddr = (uintptr_t)OldObjects->GetByIndex(0)->VFT[0x40];
			ProcessEvent = decltype(ProcessEvent)(ProcessEventAddr);
		}

		if (EngineVersion == 4.21)
		{
			GObjectsAddr = Util::FindPattern("48 8B 05 ? ? ? ? 48 8B 0C C8 48 8B 04 D1", true, 3);
			NewObjects = decltype(NewObjects)(GObjectsAddr);

			FNameToStringAddr = Util::FindPattern("48 89 5C 24 ? 57 48 83 EC 40 83 79 04 00 48 8B DA 48 8B F9");
			if (!FNameToStringAddr)
				FNameToStringAddr = Util::FindPattern("48 89 5C 24 ? 48 89 74 24 ? 57 48 83 EC 20 48 8B DA 48 8B F1 E8 ? ? ? ? 4C 8B C8");

			if (!FNameToStringAddr)
				FNameToStringAddr = Util::FindPattern("48 89 5C 24 ? 48 89 74 24 ? 48 89 7C 24 ? 41 56 48 83 EC 20 48 8B DA 4C 8B F1 E8 ? ? ? ? 4C 8B C8");

			FNameToString = decltype(FNameToString)(FNameToStringAddr);

			FreeMemoryAddr = Util::FindPattern("48 85 C9 74 2E 53 48 83 EC 20 48 8B D9");
			FreeMemory = decltype(FreeMemory)(FreeMemoryAddr);

			CreateDefaultObjectAddr = Util::FindPattern("4C 8B DC 57 48 81 EC ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 84 24 ? ? ? ? 48 83 B9 ? ? ? ? ? 48 8B F9 0F 85 ? ? ? ? 49 89 5B 10 48 8B 59 30");
			InitHostAddr = Util::FindPattern("48 8B C4 48 81 EC ? ? ? ? 48 89 58 18 4C 8D 05 ? ? ? ? 48 8B D9 48 89 78 F8 48 8D 48 88 45 33 C9 33 D2");
			InitListenAddr = Util::FindPattern("48 89 5C 24 ? 48 89 74 24 ? 57 48 83 EC 50 48 8B BC 24 ? ? ? ? 49 8B F0 48 8B 01 48 8B D9");
			SetWorldAddr = Util::FindPattern("48 89 5C 24 ? 57 48 83 EC 20 48 8B FA 48 8B D9 48 8B 91 ? ? ? ? 48 85 D2 74 28");
			ServerReplicateActorsAddr = Util::FindPattern("4C 8B DC 55 49 8D AB ? ? ? ? 48 81 EC ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 85 ? ? ? ? 48 8B 41 30 45 33 C0 49 89 5B 10 49 89 73 18 49 89 7B 20 41 8B F8");
			TickFlushAddr = Util::FindPattern("4C 8B DC 55 49 8D AB ? ? ? ? 48 81 EC ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 85 ? ? ? ? 49 89 5B 18 48 8D 05 ? ? ? ? 49 89 73 F0 33 F6");
			GiveAbilityAddr = Util::FindPattern("48 89 5C 24 ? 48 89 6C 24 ? 48 89 7C 24 ? 41 56 48 83 EC 20 83 B9 ? ? ? ? ? 49 8B E8 4C 8B F2 48 8B F9");

			InternalTryActivateAbilityAddr = Util::FindPattern("4C 89 4C 24 ? 4C 89 44 24 ? 89 54 24 10 55 53 57 41 54 41 55 41 56 41 57 48 8D AC 24 ? ? ? ? 48 81 EC ? ? ? ? 45 33 FF 48 8D 05 ? ? ? ? 44 38 3D ? ? ? ? 8B FA");
			if (!InternalTryActivateAbilityAddr)
				InternalTryActivateAbilityAddr = Util::FindPattern("4C 89 4C 24 ? 4C 89 44 24 ? 89 54 24 10 55 53 56 57 41 54 41 55 41 57 48 8D AC 24 ? ? ? ? 48 81 EC ? ? ? ? 45 33 FF 48 8D 05 ? ? ? ? 44 38 3D ? ? ? ? 8B FA");

			if (!InternalTryActivateAbilityAddr)
				InternalTryActivateAbilityAddr = Util::FindPattern("4C 89 4C 24 ? 4C 89 44 24 ? 89 54 24 10 55 53 56 57 41 54 41 56 48 8D AC 24 ? ? ? ? 48 81 EC ? ? ? ? 45 33 F6 48 8D 05 ? ? ? ? 44 38 35 ? ? ? ? 8B FA");

			SetClientLoginStateAddr = Util::FindPattern("48 89 5C 24 ? 57 48 83 EC 40 8B DA 48 8B F9 8B 91 ? ? ? ? 3B D3 0F 85 ? ? ? ? 80 3D ? ? ? ? ? 0F 82 ? ? ? ? 85 DB 74 41");
			if (!SetClientLoginStateAddr)
				SetClientLoginStateAddr = Util::FindPattern("48 89 5C 24 ? 57 48 83 EC 40 8B DA 48 8B F9 39 91 ? ? ? ? 0F 85 ? ? ? ? 80 3D ? ? ? ? ? 0F 82 ? ? ? ? 85 D2");

			ProcessEventAddr = (uintptr_t)NewObjects->GetByIndex(0)->VFT[0x40];
			ProcessEvent = decltype(ProcessEvent)(ProcessEventAddr);
		}
		
		if (EngineVersion == 4.22)
		{
			ChildrenOffset = 0x48;
			SuperOffset = 0x40;

			GObjectsAddr = Util::FindPattern("48 8B 05 ? ? ? ? 48 8B 0C C8 48 8B 04 D1", true, 3);
			NewObjects = decltype(NewObjects)(GObjectsAddr);

			FNameToStringAddr = Util::FindPattern("48 89 5C 24 ? 48 89 74 24 ? 48 89 7C 24 ? 41 56 48 83 EC 20 48 8B DA 4C 8B F1 E8 ? ? ? ? 4C 8B C8");
			FNameToString = decltype(FNameToString)(FNameToStringAddr);

			FreeMemoryAddr = Util::FindPattern("48 85 C9 74 2E 53 48 83 EC 20 48 8B D9");
			FreeMemory = decltype(FreeMemory)(FreeMemoryAddr);

			CreateDefaultObjectAddr = Util::FindPattern("4C 8B DC 57 48 81 EC ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 84 24 ? ? ? ? 48 83 B9 ? ? ? ? ? 48 8B F9");
			InitHostAddr = Util::FindPattern("48 8B C4 48 81 EC ? ? ? ? 48 89 58 18 4C 8D 05 ? ? ? ? 48 8B D9 48 89 78 F8 48 8D 48 88 45 33 C9 33 D2");
			InitListenAddr = Util::FindPattern("48 89 5C 24 ? 48 89 74 24 ? 57 48 83 EC 50 48 8B BC 24 ? ? ? ? 49 8B F0 48 8B 01 48 8B D9");

			SetWorldAddr = Util::FindPattern("48 89 5C 24 ? 57 48 83 EC 20 48 8B FA 48 8B D9 48 8B 91 ? ? ? ? 48 85 D2 74 28");
			if (!SetWorldAddr)
				SetWorldAddr = Util::FindPattern("48 89 74 24 ? 57 48 83 EC 20 48 8B F2 48 8B F9 48 8B 91 ? ? ? ? 48 85 D2");

			ServerReplicateActorsAddr = Util::FindPattern("4C 8B DC 55 49 8D AB ? ? ? ? 48 81 EC ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 85 ? ? ? ? 48 8B 41 30 45 33 C0 49 89 5B 10 49 89 73 18 49 89 7B 20 41 8B F8");
			if (!ServerReplicateActorsAddr)
				ServerReplicateActorsAddr = Util::FindPattern("4C 8B DC 55 41 55 49 8D AB ? ? ? ? 48 81 EC ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 85 ? ? ? ? 48 8B 41 30 45 33 C0");

			TickFlushAddr = Util::FindPattern("4C 8B DC 55 49 8D AB ? ? ? ? 48 81 EC ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 85 ? ? ? ? 49 89 5B 18 48 8D 05 ? ? ? ? 49 89 73 F0 33 F6");
			if (TickFlushAddr)
				Util::FindPattern("4C 8B DC 55 49 8D AB ? ? ? ? 48 81 EC ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 85 ? ? ? ? 49 89 5B 18 49 89 73 F0");

			GiveAbilityAddr = Util::FindPattern("48 89 5C 24 ? 48 89 6C 24 ? 48 89 7C 24 ? 41 56 48 83 EC 20 83 B9 ? ? ? ? ? 49 8B E8 4C 8B F2 48 8B F9");
			InternalTryActivateAbilityAddr = Util::FindPattern("4C 89 4C 24 ? 4C 89 44 24 ? 89 54 24 10 55 53 56 57 41 54 41 56 48 8D AC 24 ? ? ? ? 48 81 EC ? ? ? ? 45 33 F6 48 8D 05 ? ? ? ? 44 38 35 ? ? ? ? 8B FA");
			SetClientLoginStateAddr = Util::FindPattern("48 89 5C 24 ? 57 48 83 EC 40 8B DA 48 8B F9 39 91 ? ? ? ? 0F 85 ? ? ? ? 80 3D ? ? ? ? ? 0F 82 ? ? ? ? 85 D2");

			ProcessEventAddr = Util::FindPattern("40 55 56 57 41 54 41 55 41 56 41 57 48 81 EC ? ? ? ? 48 8D 6C 24 ? 48 89 9D ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C5 48 89 85 ? ? ? ? 8B 41 0C 45 33 F6 3B 05 ? ? ? ? 4D 8B F8 48 8B F2 4C 8B E1 41 B8 ? ? ? ? 7D 2A"); //Sig cause the vtable changes on 7.40
			ProcessEvent = decltype(ProcessEvent)(ProcessEventAddr);
		}

		if (EngineVersion == 423)
		{
			ChildrenOffset = 0x48;
			SuperOffset = 0x40;

			GObjectsAddr = Util::FindPattern("48 8B 05 ? ? ? ? 48 8B 0C C8 48 8B 04 D1", true, 3);
			NewObjects = decltype(NewObjects)(GObjectsAddr);


		}

		if (!GObjectsAddr)
			LOG(Error, "Failed to find GObjects Address!");
		if (!FNameToStringAddr)
			LOG(Error, "Failed to find FNameToString Address!");
		if (!FreeMemoryAddr)
			LOG(Error, "Failed to find FreeMemory Address!");
		if (!CreateDefaultObjectAddr)
			LOG(Error, "Failed to find CreateDefaultObject Address!");
		if (!InitHostAddr)
			LOG(Error, "Failed to find InitHost Address!");
		if (!InitListenAddr)
			LOG(Error, "Failed to find InitListen Address!");
		if (!SetWorldAddr)
			LOG(Error, "Failed to find SetWorld Address!");
		if (!SetReplicationDriverAddr && EngineVersion >= 423)
			LOG(Error, "Failed to find SetReplicationDriver Address!");
		if (!ServerReplicateActorsAddr)
			LOG(Error, "Failed to find ServerReplicateActors Address!");
		if (!TickFlushAddr)
			LOG(Error, "Failed to find TickFlush Address!");
		if (!GiveAbilityAddr)
			LOG(Error, "Failed to find GiveAbility Address!");
		if (!InternalTryActivateAbilityAddr)
			LOG(Error, "Failed to find InternalTryActivateAbility Address!");
		if (!SetClientLoginStateAddr)
			LOG(Error, "Failed to find SetClientLoginState Address!");
	}

	//Other structs

	struct FURL
	{
		FString                                     Protocol;                                                 // 0x0000(0x0010) (ZeroConstructor)
		FString                                     Host;                                                     // 0x0010(0x0010) (ZeroConstructor)
		int                                         Port;                                                     // 0x0020(0x0004) (ZeroConstructor, IsPlainOldData)
		int                                         Valid;                                                    // 0x0024(0x0004) (ZeroConstructor, IsPlainOldData)
		FString                                     Map;                                                      // 0x0028(0x0010) (ZeroConstructor)
		FString                                     RedirectUrl;                                              // 0x0038(0x0010) (ZeroConstructor)
		TArray<FString>                             Op;                                                       // 0x0048(0x0010) (ZeroConstructor)
		FString                                     Portal;                                                   // 0x0058(0x0010) (ZeroConstructor)
	};

	struct FVector
	{
		float X;
		float Y;
		float Z;
	};

	struct FRotator
	{
		float Pitch;
		float Yaw;
		float Roll;
	};

	struct FQuat
	{
		float X;
		float Y;
		float Z;
		float W;
	};

	struct FTransform
	{
		FQuat Rotation;
		FVector Translation;
		char pad01[0x4];
		FVector Scale3D;
		char pad02[0x4];
	};

	struct FLevelCollection
	{
		//char pad[0x8];
		UObject* GameState;
		UObject* NetDriver;
		UObject* DemoNetDriver;
		UObject* PersistentLevel;
		char pad1[0x50];
	};

	struct FLevelCollectionOld
	{
		//char pad[0x8];
		UObject* GameState;
		UObject* NetDriver;
		UObject* DemoNetDriver;
		UObject* PersistentLevel;
		char pad1[0x50];
	};

	struct FActorSpawnParameters
	{
		FActorSpawnParameters() : Name(), Template(nullptr), Owner(nullptr), Instigator(nullptr), OverrideLevel(nullptr),
			SpawnCollisionHandlingOverride(), bRemoteOwned(0), bNoFail(0),
			bDeferConstruction(0),
			bAllowDuringConstructionScript(0),
			NameMode(),
			ObjectFlags()
		{
		}
		;

		FName Name;
		UObject* Template;
		UObject* Owner;
		UObject* Instigator;
		UObject* OverrideLevel;
		ESpawnActorCollisionHandlingMethod SpawnCollisionHandlingOverride;

	private:
		uint8_t bRemoteOwned : 1;

	public:
		bool IsRemoteOwned() const { return bRemoteOwned; }

		uint8_t bNoFail : 1;
		uint8_t bDeferConstruction : 1;
		uint8_t bAllowDuringConstructionScript : 1;
		uint8_t NameMode;
		int ObjectFlags;
	};
}