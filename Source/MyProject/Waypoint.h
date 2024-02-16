// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Waypoint.generated.h"


class USplineComponent;
class AWaypointManager;

UCLASS(Blueprintable, BlueprintType,  ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MYPROJECT_API UWaypoint : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UWaypoint();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Waypoints)
	float GoalDistance;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Waypoints)
	FVector GoalLocation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Waypoints)
	float UpdateInterval;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Waypoints)
	float UpdateDistance;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Waypoints)
	float Z_Offset;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Waypoints)
	FVector WaypointStartLocation;

	UFUNCTION(BlueprintCallable, Category = Waypoints)
	void CreateWaypoint(float P_GoalDistance, const FVector& P_GoalLocation, float P_UpdateInterval, float P_UpdateDistance, float P_Z_Offset); //P for parametered value
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = Waypoints)
	void RemoveWaypoints();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Waypoints)
	TObjectPtr<USplineComponent> SplineComponent;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Waypoints)
	TArray<FVector> Path;

	void GetPath();
	void SetWaypoint(const TArray<FVector>& P_Path);
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = Waypoints)
	void CreateSplineComponent();
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = Waypoints)
	void CreateSplineMeshes();
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = Waypoints)
	void ResetDelay();
};
