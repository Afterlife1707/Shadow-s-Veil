
#include "MyPlayerStats.h"
#include "MyUserWidget.h"

UMyPlayerStats::UMyPlayerStats()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UMyPlayerStats::BeginPlay()
{
	Super::BeginPlay();

	t = 0;
}

int UMyPlayerStats::GetCharges() const
{
    return ShadowCharges;
}

void UMyPlayerStats::SetUserWidget(UMyUserWidget* UserWidget)
{
	MyUserWidget = UserWidget;
}

void UMyPlayerStats::UseCharge()
{
	if (MyUserWidget)
	{
		MyUserWidget->EffectVisibility(true);
		MyUserWidget->UseShadowCharge();
		ShadowCharges--;
	}
}

void UMyPlayerStats::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if(ShadowCharges<MAX_CHARGES) //dont run after reaching max charges
	{
		if(t>=ChargeRefillTime) //if timer has reached refill time
		{
			ShadowCharges++; //increment charges
			MyUserWidget->ShadowChargedRefilled(); //reset current filling shadow pointer to null
			t = 0; //reset timer
		}
		else
		{
			t += 1 * DeltaTime; //increment timer per sec

			RefillCharge(DeltaTime); //increment shadow progress bar
		}
	}
	if (ShadowCharges > MAX_CHARGES) //clamp shadow charges to max
		ShadowCharges = MAX_CHARGES;
}

void UMyPlayerStats::RefillCharge(float deltaTime)
{
	MyUserWidget->FillShadowCharge((100 / ChargeRefillTime)* deltaTime) ;
}

int UMyPlayerStats::GetCollectiblesCount()
{
	return TOTAL_COLLECTABLES;
}
