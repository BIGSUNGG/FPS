// Fill out your copyright notice in the Description page of Project Settings.


#include "RocketMagazineComponent.h"
#include Gun_h
#include Creature_h
#include SoldierAnimInstance_h

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

	if (OwnerGun->GetOwnerCreature() && OwnerGun->GetOwnerCreature()->CombatComponent->GetCurWeapon() != OwnerGun)
	{
		SetMagazineVisibility(false);
		return;
	}
	
	// 남은 탄약이 있다면 보여지기
	if (OwnerGun->GetCurAmmo() > 0)
	{
		SetMagazineVisibility(true);
	}
	// 남은 탄약이 없다면 가리기
	else if(!IsGrabMagazine)
	{
		SetMagazineVisibility(false);
	}
}

void URocketMagazineComponent::SetMagazineVisibility(bool Value)
{
	if(MagazineMesh->IsVisible() == Value)
		return;

	OwnerGun->GetTppWeaponPrimitiveInfo()["Magazine"]->SetVisibility(Value);
	OwnerGun->GetFppWeaponPrimitiveInfo()["Magazine"]->SetVisibility(Value);
}

void URocketMagazineComponent::OnAddOnDelegateEvent(UObject* Object)
{
	Super::OnAddOnDelegateEvent(Object);

	ACreature* Creature = Cast<ACreature>(Object);
	USoldierAnimInstance* AnimInstance = Cast<USoldierAnimInstance>(Creature->GetMesh()->GetAnimInstance());
	if (AnimInstance)
	{
		AnimInstance->OnReload_Grab_Magazine.AddDynamic(this, &URocketMagazineComponent::OnGrabMagazineEvent);
		AnimInstance->OnReload_Insert_Magazine.AddDynamic(this, &URocketMagazineComponent::OnInsertMagazineEvent);
	}
}

void URocketMagazineComponent::OnRemoveOnDelegateEvent(UObject* Object)
{
	Super::OnRemoveOnDelegateEvent(Object);

	ACreature* Creature = Cast<ACreature>(Object);
	USoldierAnimInstance* AnimInstance = Cast<USoldierAnimInstance>(Creature->GetMesh()->GetAnimInstance());
	if (AnimInstance)
	{
		AnimInstance->OnReload_Grab_Magazine.RemoveDynamic(this, &URocketMagazineComponent::OnGrabMagazineEvent);
		AnimInstance->OnReload_Insert_Magazine.RemoveDynamic(this, &URocketMagazineComponent::OnInsertMagazineEvent);
	}
}

void URocketMagazineComponent::OnGrabMagazineEvent()
{
	SetMagazineVisibility(true);
	IsGrabMagazine = true;
}

void URocketMagazineComponent::OnInsertMagazineEvent()
{
	IsGrabMagazine = false;

}
