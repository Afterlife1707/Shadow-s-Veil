#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "MyBTTask_FindRandomLocation.generated.h"

/**
 * 
 */
UCLASS()
class MYPROJECT_API UMyBTTask_FindRandomLocation : public UBTTask_BlackboardBase
{
    GENERATED_BODY()

public:
    explicit UMyBTTask_FindRandomLocation(FObjectInitializer const& ObjectInitializer);
    virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

private:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=AI, meta=(AllowPrivateAccess="true"))
    float SearchRadius = 1500.f;
};
