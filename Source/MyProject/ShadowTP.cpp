// Fill out your copyright notice in the Description page of Project Settings.


#include "ShadowTP.h"

#include "Components/WidgetComponent.h"
#include "Kismet/KismetMathLibrary.h"

AShadowTP::AShadowTP()
{
	PrimaryActorTick.bCanEverTick = true;

	bCanBeUsed = true;
	
}

void AShadowTP::BeginPlay()
{
	Super::BeginPlay();
	WidgetComponent = FindComponentByClass<UWidgetComponent>();
}

void AShadowTP::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if(!bIsRaycasting && WidgetComponent) //if player is not looking at this
	{
		if (!WidgetComponent->IsVisible())
			return;
		WidgetComponent->SetVisibility(false);
		bCanBeUsed = false;
	}
	else if(bIsRaycasting && bCanBeUsed && WidgetComponent)//if player is looking at this
	{
		FVector Direction = GetActorLocation() - WidgetComponent->GetComponentLocation();
		FRotator Rot = UKismetMathLibrary::FindLookAtRotation(WidgetComponent->GetComponentLocation(), ActorLocation);
		WidgetComponent->SetWorldRotation(Rot); //rotate towards player
	    if(WidgetComponent->IsVisible()) //set visible
	        return;
	    WidgetComponent->SetVisibility(true);
	}
}

bool AShadowTP::CanBeUsed() const
{
	return bCanBeUsed;
}

void AShadowTP::DisableTP()
{
    bIsRaycasting = false;
	if (WidgetComponent)
	{
		WidgetComponent->SetVisibility(false);
	}
}

void AShadowTP::EnableTP()
{
	bCanBeUsed = true;
}

void AShadowTP::isRayCasting(bool b, const FVector& actorLocation)
{
	bIsRaycasting = b;
	ActorLocation = actorLocation;
}

