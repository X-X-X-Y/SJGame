#include "SJAssetManager.h"
#include "SJLogChannels.h"
#include "Character/SJPawnData.h"

const FName FSJBundles::Equipped("Equipped");

USJAssetManager& USJAssetManager::Get()
{
	check(GEngine);

	if (USJAssetManager* Singleton = Cast<USJAssetManager>(GEngine->AssetManager))
	{
		return *Singleton;
	}
	UE_LOG(LogSJ, Fatal, TEXT("Invalid AssetManagerClassName in DefaultEngine.ini.  It must be set to LyraAssetManager!"));

	// Fatal error above prevents this from being called.
	return *NewObject<USJAssetManager>();
}

const USJPawnData* USJAssetManager::GetDefaultPawnData() const
{
	return GetAsset(DefaultPawnData);
}

UObject* USJAssetManager::SynchronousLoadAsset(const FSoftObjectPath& AssetPath)
{
	if (AssetPath.IsValid())
	{
		TUniquePtr<FScopeLogTime> LogTimePtr;

		if (ShouldLogAssetLoads())
		{
			LogTimePtr = MakeUnique<FScopeLogTime>(*FString::Printf(TEXT("Synchronously loaded asset [%s]"), *AssetPath.ToString()), nullptr, FScopeLogTime::ScopeLog_Seconds);
		}

		if (UAssetManager::IsInitialized())
		{
			return UAssetManager::GetStreamableManager().LoadSynchronous(AssetPath, false);
		}

		// Use LoadObject if asset manager isn't ready yet.
		return AssetPath.TryLoad();
	}

	return nullptr;
}

bool USJAssetManager::ShouldLogAssetLoads()
{
	static bool bLogAssetLoads = FParse::Param(FCommandLine::Get(), TEXT("LogAssetLoads"));
	return bLogAssetLoads;
}

void USJAssetManager::AddLoadedAsset(const UObject* Asset)
{
	if (ensureAlways(Asset))
	{
		FScopeLock LoadedAssetsLock(&LoadedAssetsCritical);
		LoadedAssets.Add(Asset);
	}
}

