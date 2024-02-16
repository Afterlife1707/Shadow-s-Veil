// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "MyBTTask_PlaySoundEffect.generated.h"

/**
 * 
 */
UCLASS()
class MYPROJECT_API UMyBTTask_PlaySoundEffect : public UBTTask_BlackboardBase
{
	GENERATED_BODY()
	UMyBTTask_PlaySoundEffect();

public:
    virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

    UPROPERTY(EditAnywhere, Category = "Audio")
	TObjectPtr<USoundWave> HeardSound;
	UPROPERTY(EditAnywhere, Category = "Audio")
	float Volume = 1.f;
	
};
