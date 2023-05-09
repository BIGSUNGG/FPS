// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Kraver/KraverComponent/Weapon/WeaponComponent.h"
#include "WeaponParrryComponent.generated.h"

/**
 * 
 */
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class KRAVER_API UWeaponParrryComponent : public UWeaponComponent
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;

protected:
	// Delegate
	virtual void OnSubAttackStartEvent();
	virtual void OnSubAttackEndEvent();

protected:
	// Montage
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Montage", Meta = (AllowPrivateAccess = true))
		TArray<UAnimMontage*> ParryMontagesTpp;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Montage", Meta = (AllowPrivateAccess = true))
		TArray<UAnimMontage*> ParryMontagesFpp;
};
