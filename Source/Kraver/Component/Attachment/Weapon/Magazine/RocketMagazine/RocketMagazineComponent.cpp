// Fill out your copyright notice in the Description page of Project Settings.


#include "RocketMagazineComponent.h"
#include "Kraver/Actor/Weapon/Gun/Gun.h"

void URocketMagazineComponent::BeginPlay()
{
	Super::BeginPlay();

	OwnerGun = Cast<AGun>(OwnerWeapon);
	if (!OwnerGun)
		KR_LOG(Error, TEXT("Owner is not gun class"));

}

void URocketMagazineComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (OwnerGun->GetCurAmmo() > 0)
	{

	}
	else
	{

	}
}
