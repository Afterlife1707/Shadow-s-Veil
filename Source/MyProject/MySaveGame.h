// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "Collectible.h"
#include "Engine/TriggerBox.h"
#include "MySaveGame.generated.h"

/**
 * 
 */
UCLASS()
class MYPROJECT_API UMySaveGame : public USaveGame
{
	GENERATED_BODY()
public:
	UMySaveGame();

	//save file variables
	UPROPERTY(EditAnywhere)
	TArray<ATriggerBox*> Triggers;
	UPROPERTY(EditAnywhere)
	TArray<ACollectible*> Collectibles;
	UPROPERTY(EditAnywhere)
	FVector SavedPlayerPos;
	UPROPERTY(EditAnywhere)
	int CountOfCollectiblesCollected;
	UPROPERTY(EditAnywhere)
	bool bIsBeforeFirstSave = true;
};
