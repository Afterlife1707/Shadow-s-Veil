// Fill out your copyright notice in the Description page of Project Settings.


#include "Waypoint.h"

#include "NavigationPath.h"
#include "NavigationSystem.h"
#include "Components/SplineComponent.h"

UWaypoint::UWaypoint()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UWaypoint::CreateWaypoint(float P_GoalDistance, const FVector& P_GoalLocation, float P_UpdateInterval, float P_UpdateDistance, float P_Z_Offset)  //create a waypoint
{
	GoalDistance = P_GoalDistance;
	GoalLocation = P_GoalLocation;
	UpdateInterval = P_UpdateInterval;
	UpdateDistance = P_UpdateDistance;
	Z_Offset = P_Z_Offset;

	CreateSplineComponent(); //implemented in BP

	Path.Empty(); //clear path array
	GetPath(); //set path array
}

void UWaypoint::GetPath()
{
	UNavigationPath* NavigationPath = UNavigationSystemV1::FindPathToLocationSynchronously(GetWorld(), GetOwner()->GetActorLocation(), GoalLocation); //returns a nav path to target destination
	if(NavigationPath->GetPathPoints().Num()>0) //if path returned is not null
	{
		SetWaypoint(NavigationPath->GetPathPoints()); 
		WaypointStartLocation = Path[0];
		RemoveWaypoints(); //implemented in BP, remove any previous waypoints
        for (int i = 0;i<Path.Num();i++)
        {
			FVector RaisedVector = Path[i];
			RaisedVector.Z +=  Z_Offset; //raise off ground by this offset
			//set spline
			SplineComponent->AddSplinePointAtIndex(RaisedVector, i, ESplineCoordinateSpace::World, false);
			SplineComponent->SetSplinePointType(i, ESplinePointType::Constant, false);
        }
		SplineComponent->UpdateSpline();
	    CreateSplineMeshes(); //implemented in BP
	}
	else
	{
		ResetDelay(); //implemented in BP
	}
}

void UWaypoint::SetWaypoint(const TArray<FVector>& P_Path)
{
	this->Path = P_Path;
}	
