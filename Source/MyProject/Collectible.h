// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Collectible.generated.h"

UCLASS()
class MYPROJECT_API ACollectible : public AActor
{
	GENERATED_BODY()
	
public:

    UFUNCTION()
    void OnBeginOverlapComponentEvent(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	// Sets default values for this actor's properties
	ACollectible();

protected:
	UPROPERTY(EditDefaultsOnly, Category = Collectible)
	TObjectPtr<class UStaticMeshComponent> MeshComponent;

	UPROPERTY(EditDefaultsOnly, Category = Collectible)
	TObjectPtr<USoundBase> Sound;

	UPROPERTY(EditDefaultsOnly, Category = Collectible)
	TObjectPtr<class USphereComponent> ColliderComponent;

public:	
};
