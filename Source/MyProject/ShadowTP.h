// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ShadowTP.generated.h"

class UWidgetComponent;

UCLASS()
class MYPROJECT_API AShadowTP : public AActor
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite, Category = TP,meta = (AllowPrivateAccess = "true"));
	bool bCanBeUsed;
	bool bIsRaycasting;
	UPROPERTY(VisibleAnywhere, Category=TP);
	TObjectPtr<UWidgetComponent> WidgetComponent;

	FVector ActorLocation;
public:	
	AShadowTP();

	UFUNCTION(BlueprintCallable, Category = TP)
	bool CanBeUsed() const;
	UFUNCTION(BlueprintCallable, Category = TP)
	void DisableTP();
	UFUNCTION(BlueprintCallable, Category = TP)
	void EnableTP();
    void isRayCasting(bool b, const FVector& ActorLocation);
protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

};
