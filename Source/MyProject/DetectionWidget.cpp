// Fill out your copyright notice in the Description page of Project Settings.


#include "DetectionWidget.h"

#include "MyProjectCharacter.h"
#include "Components/ProgressBar.h"
#include "Kismet/GameplayStatics.h"

UDetectionWidget::UDetectionWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    //DetectionMeter = Cast<UProgressBar>(GetWidgetFromName(TEXT("DetectionMeter")));
}

void UDetectionWidget::UpdateProgress(float Progress) //this function is called on tick of player when in ai vision
{
    if (!DetectionMeter)
        return;
    //make meter visible
    if(DetectionMeter->GetVisibility()==ESlateVisibility::Hidden)
        DetectionMeter->SetVisibility(ESlateVisibility::Visible);

    // UE_LOG(LogTemp, Warning, TEXT("progress : %f"), Progress);
    //UE_LOG(LogTemp, Warning, TEXT("DETECTION PROGRESS : %f"), DetectionMeter->GetPercent());
    //increase meter progress
    DetectionMeter->SetPercent(DetectionMeter->GetPercent()+(Progress / 100));

    //if >=1, meter is full, player spotted and game over
    if(DetectionMeter->GetPercent()>=1.f)
    {
        if (auto* const Player = Cast<AMyProjectCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)))
            Player->GameOverSpotted();
    }
}

void UDetectionWidget::ResetProgress(float Progress)
{
    if (!DetectionMeter)
        return;
    if (DetectionMeter->GetVisibility() == ESlateVisibility::Hidden)
        return; //return if meter already hidden
    if ((DetectionMeter->GetVisibility() == ESlateVisibility::Visible) && DetectionMeter->GetPercent() <= 0.f)
    {
        //reset angle and visibility when 0
        DetectionMeter->SetRenderTransformAngle(0.f);
        DetectionMeter->SetVisibility(ESlateVisibility::Hidden);
    }
    else
        UpdateProgress(-Progress); //slowly decrease meter to 0
}

void UDetectionWidget::UpdateAngle(float Angle)
{
    if (!DetectionMeter)
        return;
    DetectionMeter->SetRenderTransformAngle(Angle);
}
