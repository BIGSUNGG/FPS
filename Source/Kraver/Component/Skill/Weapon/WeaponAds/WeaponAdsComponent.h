// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Kraver/Kraver.h"
#include WeaponComponent_h
#include "WeaponAdsComponent.generated.h"

/**
 * 
 */
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class KRAVER_API UWeaponAdsComponent : public UWeaponComponent
{
	GENERATED_BODY()
	
public:
	// Getter Setter
	const float& GetReduceFov() { return ReduceFov; }
	const float& GetInterpSpeed() { return InterpSpeed; }

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ads", Meta = (AllowPrivateAccess = true))
	float ReduceFov = 25.f;	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ads", Meta = (AllowPrivateAccess = true))
	float InterpSpeed = 15.f;

};
