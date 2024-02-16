// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DetectionWidget.generated.h"

class UProgressBar;
/**
 * 
 */
UCLASS()
class MYPROJECT_API UDetectionWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UDetectionWidget(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Detection, meta = (BindWidget))
	TObjectPtr<UProgressBar> DetectionMeter;

	void UpdateProgress(float Progress);
	void ResetProgress(float Progress);
	void UpdateAngle(float Angle);
};
