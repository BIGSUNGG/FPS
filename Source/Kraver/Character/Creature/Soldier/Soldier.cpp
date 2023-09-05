// Fill out your copyright notice in the Description page of Project Settings.


#include "Soldier.h"
#include SoldierAnimInstance_h
#include AttachmentMagazineComponent_h
#include WeaponReloadComponent_h

ASoldier::ASoldier()
	: Super()
{

}

void ASoldier::BeginPlay()
{
	Super::BeginPlay();

	USoldierAnimInstance* AnimInstance = Cast<USoldierAnimInstance>(GetMesh()->GetAnimInstance());
	AnimInstance->OnReload_Grab_Magazine.AddDynamic(this, &ASoldier::OnReload_Grab_MagazineEvent);
	AnimInstance->OnReload_Insert_Magazine.AddDynamic(this, &ASoldier::OnReload_Insert_MagazineEvent);
}

void ASoldier::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ASoldier::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void ASoldier::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

}

void ASoldier::OnReload_Grab_MagazineEvent()
{
	if (CombatComponent->GetCurWeapon() && CombatComponent->GetCurWeapon()->GetTppWeaponPrimitiveInfo().Contains("Magazine"))
	{
		UPrimitiveComponent* MagazineComp = CombatComponent->GetCurWeapon()->GetTppWeaponPrimitiveInfo()["Magazine"];
		UWeaponReloadComponent* ReloadComp = CombatComponent->GetCurWeapon()->FindComponentByClass<UWeaponReloadComponent>();

		MagazineComp->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, "SOCKET_Magazine");
		MagazineComp->SetRelativeLocation(ReloadComp->GetGrabRelativeLocation());
		MagazineComp->SetRelativeRotation(ReloadComp->GetGrabRelativeRotation());
	}
}

void ASoldier::OnReload_Insert_MagazineEvent()
{
	if (CombatComponent->GetCurWeapon() && CombatComponent->GetCurWeapon()->GetTppWeaponPrimitiveInfo().Contains("Magazine"))
	{
		UAttachmentMagazineComponent* MagazineComp = CombatComponent->GetCurWeapon()->FindComponentByClass<UAttachmentMagazineComponent>();
		CombatComponent->GetCurWeapon()->GetTppWeaponPrimitiveInfo()["Magazine"]->AttachToComponent(CombatComponent->GetCurWeapon()->GetTppWeaponMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, MagazineComp->GetMagazineSocketName());
	}
}
