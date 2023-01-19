// Fill out your copyright notice in the Description page of Project Settings.


#include "Soldier.h"
#include "Net/UnrealNetwork.h"

ASoldier::ASoldier()
	: ACreature()
{
	ArmMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("ARM_MESH"));
	ArmMesh->SetupAttachment(Camera);
	ArmMesh->SetCastShadow(false);
	ArmMesh->SetOnlyOwnerSee(true);
}

void ASoldier::BeginPlay()
{
	ACreature::BeginPlay();

	ChangeView();
}

void ASoldier::Tick(float DeltaTime)
{
	ACreature::Tick(DeltaTime);
}

void ASoldier::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	ACreature::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction(TEXT("ChangeView"), EInputEvent::IE_Pressed,this, &ASoldier::ChangeView);
}

void ASoldier::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

}

void ASoldier::ChangeView()
{
	switch (ViewType)
	{
	case EViewType::FIRST_PERSON:
		ViewType = EViewType::THIRD_PERSON;
		SpringArm->TargetArmLength = 300.f;
		SpringArm->SetRelativeLocation(FVector(-80.0, 0.f, 150.0));
		GetMesh()->SetOwnerNoSee(false);
		ArmMesh->SetOwnerNoSee(true);
		break;
	case EViewType::THIRD_PERSON:
		ViewType = EViewType::FIRST_PERSON;
		SpringArm->TargetArmLength = 0.f;
		SpringArm->SetRelativeLocation(FVector(0.f, 0.f, 165.f));
		GetMesh()->SetOwnerNoSee(true);
		ArmMesh->SetOwnerNoSee(false);
		break;
	default:
		break;
	}
}

