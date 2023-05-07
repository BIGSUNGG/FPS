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
	virtual bool Equipped(ACreature* Character) override; // Character에게 장착됨
	virtual bool UnEquipped() override; // 장착해제됨
	virtual bool Hold() override; // Character 손에 들려짐
	virtual bool Holster() override; // Character 손에서 집어넣어짐

	virtual void AddOnOwnerDelegate() override;
	virtual void RemoveOnOwnerDelegate() override;

	virtual void OnAttackStartEvent() override; // 캐릭터의 공격이 시작하였을때 호출되는 함수
	virtual void SwingAttack();

	virtual void ComboStart();
	virtual void NextComboAttack();
	virtual void ComboEnd();

	// Rpc

	// Delegate
	UFUNCTION()
		void OnCanInputNextComboEvent();
	UFUNCTION()
		void OnSwingAttackEvent();
	UFUNCTION()
		void OnAttackNextComboEvent();
	UFUNCTION()
		void OnComboEndEvent();
	virtual void OnAttackEvent() override;

public:
	// Getter Setter
	FORCEINLINE bool IsComboAttacking() { return CurComboAttack != 0; }

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Component", meta = (AllowPrivateAccess = "true"))
	TArray<UWeaponComponent*> WeaponComps;

protected:	
	// Combo
	int CurComboAttack = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Combat|Combo", meta = (AllowPrivateAccess = "true"))
		int MaxComboAttack = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Combat|Combo", meta = (AllowPrivateAccess = "true"))
		bool bAutomaticCombo = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Combat|Combo", meta = (AllowPrivateAccess = "true"))
		bool bAutomaticRepeatCombo = false;
	bool bInputNextCombo = false;
	bool bCanInputNextCombo = false;

	// Montage
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Montage", Meta = (AllowPrivateAccess = true))
		TArray<UAnimMontage*> AttackMontagesTpp;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Montage", Meta = (AllowPrivateAccess = true))
		TArray<UAnimMontage*> AttackMontagesFpp;
};
