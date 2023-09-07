// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Kraver/Kraver.h"
#include Weapon_h
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
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	// Function
	virtual bool OnServer_Equipped(ACreature* Character) override; // Character에게 장착됨
	virtual bool OnServer_UnEquipped() override; // 장착해제됨
	virtual bool OnLocal_Unholster() override; // Character 손에 들려짐
	virtual bool OnLocal_Holster() override; // Character 손에서 집어넣어짐

	virtual void OnLocal_AddOnOwnerDelegate() override;
	virtual void OnLocal_RemoveOnOwnerDelegate() override;

	virtual void OnAttackStartEvent() override; // 캐릭터의 공격이 시작하였을때 호출되는 함수
	virtual void SwingAttack();

	virtual void ComboStart();
	virtual void NextComboAttack();
	virtual void ComboEnd();

	virtual void StartSwingEvent();

	// Rpc
	UFUNCTION(Server, Reliable)
	virtual void Server_SwingResult(const TArray<FHitResult>& Results);

	UFUNCTION(Server, Reliable)
	virtual void Server_SwingStart(int Combo);
	UFUNCTION(NetMulticast, Reliable)
	virtual void Multicast_SwingStart(int Combo);

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
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Data|Combat|Combo", meta = (AllowPrivateAccess = "true"))
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
