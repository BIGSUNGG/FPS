// Fill out your copyright notice in the Description page of Project Settings.


#include "SoldierAnimInstance.h"

USoldierAnimInstance::USoldierAnimInstance() : UCreatureAnimInstance()
{
}

void USoldierAnimInstance::NativeUpdateAnimation(float DeltaSeconds) 
{
	UCreatureAnimInstance::NativeUpdateAnimation(DeltaSeconds);

	Player = Cast<ASoldier>(Creature);
	if(Player == nullptr) 
		return;

}
