// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Kraver/Anim/Creature/Soldier/SoldierAnimInstance.h"
#include "KraverPlayerAnimInstance.generated.h"

/**
 * 
 */

class AKraverPlayer;

UCLASS()
class KRAVER_API UKraverPlayerAnimInstance : public USoldierAnimInstance
{
	GENERATED_BODY()
	
public:
	virtual void NativeBeginPlay() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

protected:
	AKraverPlayer* KraverPlayer;

};
