#pragma once

#include "EngineMinimal.h"
#include "Kraver/Utility/Macro/KraverMacro.h"

// Trace
bool LineTraceMultiByChannel_ExceptWorldObject(UWorld* World, TArray<struct FHitResult>& OutHits, const FVector& Start, const FVector& End, ECollisionChannel TraceChannel, const FCollisionQueryParams& Params = FCollisionQueryParams::DefaultQueryParam, const FCollisionResponseParams& ResponseParam = FCollisionResponseParams::DefaultResponseParam);

bool SweepMultiByChannel_ExceptWorldObject(UWorld* World, TArray<struct FHitResult>& OutHits, const FVector& Start, const FVector& End, const FQuat& Rot, ECollisionChannel TraceChannel, const FCollisionShape& CollisionShape, const FCollisionQueryParams& Params = FCollisionQueryParams::DefaultQueryParam, const FCollisionResponseParams& ResponseParam = FCollisionResponseParams::DefaultResponseParam);
bool SweepMultiByProfile_ExceptWorldObject(UWorld* World, TArray<struct FHitResult>& OutHits, const FVector& Start, const FVector& End, const FQuat& Rot, FName ProfileName, const FCollisionShape& CollisionShape, const FCollisionQueryParams& Params = FCollisionQueryParams::DefaultQueryParam);

// Find
template<class T>
T* FindComponentByClassIncludeOwner(AActor* FirstActor)
{
	AActor* CurActor = FirstActor;
	T* FindComp = nullptr;

	while (CurActor)
	{
		FindComp = CurActor->FindComponentByClass<T>();
		if (FindComp)
			return FindComp;

		CurActor = CurActor->GetOwner();
	}

	return nullptr;
}

template<class T>
T* FindOwnerByClass(AActor* FirstActor)
{
	AActor* CurActor = FirstActor;
	T* Result = nullptr;

	while (CurActor)
	{
		Result = Cast<T>(CurActor);
		if(Result)
			return Result;

		CurActor = CurActor->GetOwner();
	}

	return nullptr;
}