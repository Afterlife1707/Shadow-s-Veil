// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MyPlayerStats.generated.h"

class UWidget;
class UMyUserWidget;
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MYPROJECT_API UMyPlayerStats : public UActorComponent
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = TP, meta = (AllowPrivateAccess = "true"));
	int ShadowCharges = 3;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = TP, meta = (AllowPrivateAccess = "true"));
    int MAX_CHARGES = 3;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = TP, meta = (AllowPrivateAccess = "true"));
	float ChargeRefillTime = 25.f;
	float t; //temp timer float

    UMyUserWidget* MyUserWidget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Collectible, meta = (AllowPrivateAccess = "true"));
	int TOTAL_COLLECTABLES = 11; //total count of collectibles/scrolls in the game

public:	
	// Sets default values for this component's properties
	UMyPlayerStats();
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	int GetCharges() const;
	void SetUserWidget(UMyUserWidget* UserWidget);
	void UseCharge();
	void RefillCharge(float deltaTime);
	UFUNCTION(BlueprintCallable, Category = Collectible)
    int GetCollectiblesCount();
protected:
	// Called when the game starts
	virtual void BeginPlay() override;
};
