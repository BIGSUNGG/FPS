// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Kraver/Kraver.h"
#include "Engine/GameInstance.h"
#include "ExpolosionSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class KRAVER_API UExpolosionSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:
	void Explosion(AActor* ExplosionCauser, const FVector& ExplosionLocation, float ExplosionMaxDamage, float ExplosionRadius, FName ExplosionProfileName, const TArray<AActor*>& ExplosionIgnoreActors, class UCombatComponent* CombatComponent, UCurveFloat* ExplosionDecreaseCurve = nullptr);

};
