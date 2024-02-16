// Fill out your copyright notice in the Description page of Project Settings.


#include "NPC.h"

ANPC::ANPC()
{
	PrimaryActorTick.bCanEverTick = true;
	//FootstepAudioComponent->AttenuationSettings = Attenuation;
}

UBehaviorTree* ANPC::GetBehaviourTree()
{
	return Tree;
}

APatrolPath* ANPC::GetPatrolPath() const
{
	return PatrolPath;
}

