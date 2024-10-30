#include "SJAssetManager.h"
#include "SJLogChannels.h"

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
