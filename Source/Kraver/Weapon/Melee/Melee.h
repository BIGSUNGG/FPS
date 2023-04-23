// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Kraver/Weapon/Weapon.h"
#include "Melee.generated.h"

/**
 * 
 */
UCLASS()
class KRAVER_API AMelee : public AWeapon
{
	GENERATED_BODY()

public:
	AMelee();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

protected:
	// Function
	virtual bool Equipped(ACreature* Character); // Character에게 장착됨
	virtual bool UnEquipped(); // 장착해제됨
	virtual bool Hold(); // Character 손에 들려짐
	virtual bool Holster(); // Character 손에서 집어넣어짐

	virtual void AddOnOwnerDelegate() override;
	virtual void RemoveOnOwnerDelegate() override;

	virtual void Attack() override; // 공격할때 호출되는 함수
	virtual void AttackStartEvent() override; // 캐릭터의 공격이 시작하였을때 호출되는 함수

	virtual void ComboStart();
	virtual void NextComboAttack();
	virtual void ComboEnd();

	// Delegate
	UFUNCTION()
		void OnCanInputNextComboEvent();
	UFUNCTION()
		void OnSwingAttackEvent();
	UFUNCTION()
		void OnAttackNextComboEvent();
	UFUNCTION()
		void OnComboEndEvent();

public:
	virtual UAnimMontage* GetAttackMontageTpp() override { return AttackMontagesTpp[CurComboAttack]; }
	virtual UAnimMontage* GetAttackMontageFpp() override { return AttackMontagesFpp[CurComboAttack]; }

protected:
	// Combo
	int CurComboAttack = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack", meta = (AllowPrivateAccess = "true"))
		int MaxComboAttack = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack", meta = (AllowPrivateAccess = "true"))
		bool bAutomaticCombo = false;
	bool bInputNextCombo = false;
	bool bCanInputNextCombo = false;

	// Montage
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Montage", Meta = (AllowPrivateAccess = true))
		TArray<UAnimMontage*> AttackMontagesTpp;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Montage", Meta = (AllowPrivateAccess = true))
		TArray<UAnimMontage*> AttackMontagesFpp;
};
