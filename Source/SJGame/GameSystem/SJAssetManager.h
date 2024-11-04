// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/AssetManager.h"
#include "SJAssetManager.generated.h"


class USJPawnData;

struct FSJBundles
{
	static const FName Equipped;
};

/**
 * 
 */
UCLASS(Config = Game)
class SJGAME_API USJAssetManager : public UAssetManager
{
	GENERATED_BODY()

public:
	// Returns the AssetManager singleton object.
	static USJAssetManager& Get();
	const USJPawnData* GetDefaultPawnData() const;
	
	// Returns the asset referenced by a TSoftObjectPtr.  This will synchronously load the asset if it's not already loaded.
	template<typename AssetType>
	static AssetType* GetAsset(const TSoftObjectPtr<AssetType>& AssetPointer, bool bKeepInMemory = true);

protected:
	static UObject* SynchronousLoadAsset(const FSoftObjectPath& AssetPath);
	static bool ShouldLogAssetLoads();

	// Thread safe way of adding a loaded asset to keep in memory.
	void AddLoadedAsset(const UObject* Asset);
	
protected:
	
	// Pawn data used when spawning player pawns if there isn't one set on the player state.
	UPROPERTY(Config)
	TSoftObjectPtr<USJPawnData> DefaultPawnData;

private:
	// Used for a scope lock when modifying the list of load assets.
	FCriticalSection LoadedAssetsCritical;

	// Assets loaded and tracked by the asset manager.
	UPROPERTY()
	TSet<TObjectPtr<const UObject>> LoadedAssets;
};

template<typename AssetType>
AssetType* USJAssetManager::GetAsset(const TSoftObjectPtr<AssetType>& AssetPointer, bool bKeepInMemory)
{
	AssetType* LoadedAsset = nullptr;

	const FSoftObjectPath& AssetPath = AssetPointer.ToSoftObjectPath();

	if (AssetPath.IsValid())
	{
		LoadedAsset = AssetPointer.Get();
		if (!LoadedAsset)
		{
			LoadedAsset = Cast<AssetType>(SynchronousLoadAsset(AssetPath));
			ensureAlwaysMsgf(LoadedAsset, TEXT("Failed to load asset [%s]"), *AssetPointer.ToString());
		}
		
		if (LoadedAsset && bKeepInMemory)
		{
			// Added to loaded asset list.
			Get().AddLoadedAsset(Cast<UObject>(LoadedAsset));
		}
	}

	return LoadedAsset;
}
