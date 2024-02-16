// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PatrolPath.h"
#include "BehaviorTree/BehaviorTree.h"
#include "GameFramework/Character.h"
#include "NPC.generated.h"

UCLASS()
class MYPROJECT_API ANPC : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ANPC();

	UBehaviorTree* GetBehaviourTree();
	APatrolPath* GetPatrolPath() const;

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AI, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UBehaviorTree> Tree;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AI, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<APatrolPath> PatrolPath;

};
