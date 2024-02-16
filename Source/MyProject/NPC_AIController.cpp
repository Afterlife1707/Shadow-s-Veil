// Fill out your copyright notice in the Description page of Project Settings.


#include "NPC_AIController.h"

#include "MyProjectCharacter.h"
#include "NPC.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Hearing.h"
#include "Perception/AISenseConfig_Sight.h"

ANPC_AIController::ANPC_AIController(FObjectInitializer const& ObjectInitializer)
{
    SetupPerceptionSystem();
}

void ANPC_AIController::OnPossess(APawn* InPawn)
{
    Super::OnPossess(InPawn);
    //setup and run behaviour tree
    if(ANPC* const NPC = Cast<ANPC>(InPawn))
    {
        if(UBehaviorTree* const tree = NPC->GetBehaviourTree())
        {
            UBlackboardComponent* b;
            UseBlackboard(tree->BlackboardAsset, b);
            Blackboard = b;
            RunBehaviorTree(tree);
        }
    }
}

void ANPC_AIController::SetupPerceptionSystem()
{
    //create and setup hear and sight senses
    SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("Sight Config"));
    if(SightConfig)
    {
        SetPerceptionComponent(*CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("Perception Component")));
        SightConfig->SightRadius = 1000.f;
        SightConfig->LoseSightRadius = SightConfig->SightRadius + 25.f;
        SightConfig->PeripheralVisionAngleDegrees = 90.f;
        SightConfig->SetMaxAge(5.f);
        SightConfig->AutoSuccessRangeFromLastSeenLocation = 1020.f;
        SightConfig->DetectionByAffiliation.bDetectEnemies = true;
        SightConfig->DetectionByAffiliation.bDetectFriendlies = true;
        SightConfig->DetectionByAffiliation.bDetectNeutrals = true;

        GetPerceptionComponent()->SetDominantSense(*SightConfig->GetSenseImplementation());
        GetPerceptionComponent()->ConfigureSense(*SightConfig);
        SightID = SightConfig->GetSenseID();
    }

    HearingConfig = CreateDefaultSubobject<UAISenseConfig_Hearing>(TEXT("Hearing Config"));
    if(HearingConfig)
    {
        HearingConfig->DetectionByAffiliation.bDetectFriendlies = true;
        HearingConfig->DetectionByAffiliation.bDetectNeutrals = true;
        HearingConfig->DetectionByAffiliation.bDetectEnemies = true;
        HearingConfig->HearingRange = 2500.f;
        GetPerceptionComponent()->ConfigureSense(*HearingConfig);
        HearID = HearingConfig->GetSenseID();
    }
    GetPerceptionComponent()->OnTargetPerceptionUpdated.AddDynamic(this, &ANPC_AIController::OnTargetDetected);
}

void ANPC_AIController::OnTargetDetected(AActor* Actor, FAIStimulus const Stimulus) //when player is detected
{
    if(auto* const Ch = Cast<AMyProjectCharacter>(Actor))
    {
        //if seen
        if (Stimulus.Type == SightID && Ch->IsPlayerInLight())
        {
            if(Ch->IsPlayerInLight()) //only detect if player is in light
            {
                //UE_LOG(LogTemp, Warning, TEXT("player seen"));
                GetBlackboardComponent()->SetValueAsBool("CanSeePlayer", Stimulus.WasSuccessfullySensed()); //set blackboard value

                FVector AILocation = GetPawn()->GetActorLocation();
                FVector PlayerLocation = Ch->GetActorLocation();
                FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(PlayerLocation, AILocation);
                FRotator AngleToLook = UKismetMathLibrary::NormalizedDeltaRotator(LookAtRotation, Ch->GetControlRotation());
                
                Ch->SpottedByNPC(AngleToLook.Yaw); //call spotted on player and start detecting and rotate meter towards AI
            }
        }
        //if heard
        else if (Stimulus.Type == HearID)
        {
            //set blackboard values
            GetBlackboardComponent()->SetValueAsBool("CanHearPlayer", Stimulus.WasSuccessfullySensed());
            GetBlackboardComponent()->SetValueAsVector("LastHeardLocation", Stimulus.StimulusLocation);
            Ch->HeardByNPC(); //call heard on player
        }
        else//not in senses
        {
            Ch->NPCLostSight();
           // UE_LOG(LogTemp, Warning, TEXT("player lost"));
        }
    }
}