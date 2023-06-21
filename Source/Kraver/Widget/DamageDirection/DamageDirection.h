// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Kraver/Kraver.h"
#include "Blueprint/UserWidget.h"
#include "DamageDirection.generated.h"

/**
 * 
 */
UCLASS()
class KRAVER_API UDamageDirection : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
		inline void Initialize(AActor* InActor, FVector InLocation) { Actor = InActor; HitLocation = InLocation; }

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (AllowPrivateAccess = true))
		AActor* Actor;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (AllowPrivateAccess = true))
		FVector HitLocation;
};
