
#include "MyBTTask_PlaySoundEffect.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"


UMyBTTask_PlaySoundEffect::UMyBTTask_PlaySoundEffect()
{
    NodeName = TEXT("Play Sound");
}

EBTNodeResult::Type UMyBTTask_PlaySoundEffect::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    //check to see if sound was already played in this spot scenario
    bool bIsSoundPlaying = OwnerComp.GetBlackboardComponent()->GetValueAsBool(GetSelectedBlackboardKey());
    if(bIsSoundPlaying)
        return EBTNodeResult::Succeeded;

    //else play sound
    UGameplayStatics::PlaySound2D(GetWorld(), HeardSound, Volume);
    OwnerComp.GetBlackboardComponent()->SetValueAsBool(GetSelectedBlackboardKey(), true);
    return Super::ExecuteTask(OwnerComp, NodeMemory);
}
