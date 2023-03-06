// Fill out your copyright notice in the Description page of Project Settings.


#include "Kraver/Anim/Creature/Soldier/KraverPlayer/KraverPlayerAnimInstance.h"
#include "Kraver/Creature/Soldier/KraverPlayer/KraverPlayer.h"

void UKraverPlayerAnimInstance::NativeBeginPlay()
{
	USoldierAnimInstance::NativeBeginPlay();

	KraverPlayer = Cast<AKraverPlayer>(Soldier);
}

void UKraverPlayerAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	USoldierAnimInstance::NativeUpdateAnimation(DeltaSeconds);

	if (KraverPlayer == nullptr)
		return;

}
