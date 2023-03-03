// Fill out your copyright notice in the Description page of Project Settings.


#include "Kraver/Anim/Creature/Soldier/KraverPlayer/KraverPlayerAnimInstance.h"
#include "Kraver/Creature/Soldier/KraverPlayer/KraverPlayer.h"

void UKraverPlayerAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	USoldierAnimInstance::NativeUpdateAnimation(DeltaSeconds);

	KraverPlayer = Cast<AKraverPlayer>(Soldier);
	if (KraverPlayer == nullptr)
		return;

}
