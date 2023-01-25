// Fill out your copyright notice in the Description page of Project Settings.


#include "SoldierAnimInstance.h"

USoldierAnimInstance::USoldierAnimInstance() : UCreatureAnimInstance()
{
}

void USoldierAnimInstance::NativeUpdateAnimation(float DeltaSeconds) 
{
	UCreatureAnimInstance::NativeUpdateAnimation(DeltaSeconds);

	Soldier = Cast<ASoldier>(Creature);
	if(Soldier == nullptr)
		return;

}
