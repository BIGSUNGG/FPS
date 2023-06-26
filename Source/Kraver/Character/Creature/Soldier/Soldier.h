// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Kraver/Character/Creature/Creature.h"
#include "Soldier.generated.h"

/**
 * 
 */

UCLASS()
class KRAVER_API ASoldier : public ACreature
{
	GENERATED_BODY()

public:
	ASoldier();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	// Delegate
	UFUNCTION()
		virtual void OnReload_Grab_MagazineEvent();
	UFUNCTION()
		virtual void OnReload_Insert_MagazineEvent();
};
