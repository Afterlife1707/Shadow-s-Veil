// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "MyGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class MYPROJECT_API UMyGameInstance : public UGameInstance
{
	GENERATED_BODY()
	const FVector _PLAYER_START_LOCATION = FVector(-13695.475444, 3971.11285, 838.449952);
public:
	virtual void Init() override;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bGameIsFirstTime;

	void CreateSaveFile();

	UFUNCTION(BlueprintCallable, Category=MySaveGame)
	void SaveGame();
	void LoadGame();

};
