// Fill out your copyright notice in the Description page of Project Settings.


#include "MyUserWidget.h"

#include "Components/Image.h"
#include "Components/ProgressBar.h"

//this class sets up the user widget HUD which displays the count of shadow charges

UMyUserWidget::UMyUserWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    for (int i = 0; i < 3; i++)
    {
        ShadowCharges.Add(nullptr);
    }
    CurrentShadowFilling = nullptr;
}

void UMyUserWidget::ShadowChargedRefilled() //reset pointer when filled
{
    CurrentShadowFilling = nullptr;
}

void UMyUserWidget::FillShadowCharge(float value)
{
    if (CurrentShadowFilling)
        CurrentShadowFilling->SetPercent(CurrentShadowFilling->GetPercent() + value / 100); //to avoid extra loops
    else 
    {
        for (int i = ShadowCharges.Num() - 1; i >= 0; i--)
        {
            if (ShadowCharges[i] && ShadowCharges[i]->GetPercent() <= 1.0f)
            {
                CurrentShadowFilling = ShadowCharges[i];
                CurrentShadowFilling->SetPercent(CurrentShadowFilling->GetPercent() + value / 100);
                break;
            }
        }
    }
}

void UMyUserWidget::UseShadowCharge()
{
    for (int i = ShadowCharges.Num() - 1; i >= 0; i--)
    {
        if (ShadowCharges[i] && ShadowCharges[i]->GetPercent() >= 1.0f)
        {
            ShadowCharges[i]->SetPercent(0.f);
            break;
        }
    }
}

void UMyUserWidget::EffectVisibility(bool b)
{
    EffectImage->SetVisibility(b?ESlateVisibility::Visible:ESlateVisibility::Hidden);
    bIsEffectVisible = b;
}

UImage* UMyUserWidget::GetImage()
{
    return EffectImage;
}
