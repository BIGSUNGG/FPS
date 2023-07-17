#include "KraverFunction.h"

bool LineTraceMultiByChannel_ExceptWorldObject(UWorld* World, TArray<struct FHitResult>& OutHits, const FVector& Start, const FVector& End, ECollisionChannel TraceChannel, const FCollisionQueryParams& Params /*= FCollisionQueryParams::DefaultQueryParam*/, const FCollisionResponseParams& ResponseParam /*= FCollisionResponseParams::DefaultResponseParam*/)
{
	if (OutHits.Num() > 0)
	{
		OutHits.Empty();
	}

	TArray<struct FHitResult> FirstHits;

	bool bResult = World->LineTraceMultiByChannel(
		FirstHits,
		Start,
		End,
		TraceChannel,
		Params,
		ResponseParam
	);

	if (bResult)
	{
		for (auto& Result : FirstHits)
		{
			if (Result.bBlockingHit && IsValid(Result.GetActor()))
			{
				// 오브젝트 사이에 다른 월드 오브젝트가 있는지
				FHitResult ObjectHitResult;
				bool bObjectResult = World->LineTraceSingleByChannel(
					ObjectHitResult,
					Start,
					Result.ImpactPoint,
					ECC_ONLY_OBJECT,
					Params
				);
				if (bObjectResult == false || IsValid(ObjectHitResult.GetActor()) == false)
				{
					OutHits.Add(Result);
				}
			}
		}
	}

	return (OutHits.Num() > 0);
}

bool SweepMultiByChannel_ExceptWorldObject(UWorld* World, TArray<struct FHitResult>& OutHits, const FVector& Start, const FVector& End, const FQuat& Rot, ECollisionChannel TraceChannel, const FCollisionShape& CollisionShape, const FCollisionQueryParams& Params /*= FCollisionQueryParams::DefaultQueryParam*/, const FCollisionResponseParams& ResponseParam /*= FCollisionResponseParams::DefaultResponseParam*/)
{
	if (OutHits.Num() > 0)
	{
		OutHits.Empty();
	}

	TArray<struct FHitResult> FirstHits;

	bool bResult = World->SweepMultiByChannel(
		FirstHits,
		Start,
		End,
		Rot,
		TraceChannel,
		CollisionShape,
		Params,
		ResponseParam
	);

	if (bResult)
	{
		for (auto& Result : FirstHits)
		{
			if (Result.bBlockingHit && IsValid(Result.GetActor()))
			{
				// 오브젝트 사이에 다른 월드 오브젝트가 있는지
				FHitResult ObjectHitResult;
				bool bObjectResult = World->LineTraceSingleByChannel(
					ObjectHitResult,
					Start,
					Result.ImpactPoint,
					ECC_ONLY_OBJECT,
					Params
				);
				if (bObjectResult == false || IsValid(ObjectHitResult.GetActor()) == false)
				{
					OutHits.Add(Result);
				}
			}
		}

	}

	return (OutHits.Num() > 0);
}

bool SweepMultiByProfile_ExceptWorldObject(UWorld* World, TArray<struct FHitResult>& OutHits, const FVector& Start, const FVector& End, const FQuat& Rot, FName ProfileName, const FCollisionShape& CollisionShape, const FCollisionQueryParams& Params /*= FCollisionQueryParams::DefaultQueryParam*/)
{
	if (OutHits.Num() > 0)
	{
		OutHits.Empty();
	}

	TArray<struct FHitResult> FirstHits;

	bool bResult = World->SweepMultiByProfile(
		FirstHits,
		Start,
		End,
		Rot,
		ProfileName,
		CollisionShape,
		Params
		);

	if (bResult)
	{
		for (auto& Result : FirstHits)
		{
			if (Result.bBlockingHit && IsValid(Result.GetActor()))
			{
				// 오브젝트 사이에 다른 월드 오브젝트가 있는지
				FHitResult ObjectHitResult;
				bool bObjectResult = World->LineTraceSingleByChannel(
					ObjectHitResult,
					Start,
					Result.ImpactPoint,
					ECC_ONLY_OBJECT,
					Params
				);
				if (bObjectResult == false || IsValid(ObjectHitResult.GetActor()) == false)
				{
					OutHits.Add(Result);
				}
			}
		}

	}

	return (OutHits.Num() > 0);
}
