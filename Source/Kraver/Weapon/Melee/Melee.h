// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Kraver/Weapon/Weapon.h"
#include "Melee.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAssassinateDele, AActor*, AssassinatedActor);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FAssassinateEndDele);
/**
 * 
 */

USTRUCT(BlueprintType)
struct FAssassinateInfo
{
	GENERATED_BODY()
public:
	UAnimMontage* AssassinatedMontagesTpp;
	UAnimMontage* AssassinatedMontagesFpp;
};

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

	virtual void Attack() override; // 공격할때 호출되는 함수
	virtual void AttackStartEvent() override; // 캐릭터의 공격이 시작하였을때 호출되는 함수
	virtual void SwingAttack();

	virtual void ComboStart();
	virtual void NextComboAttack();
	virtual void ComboEnd();

	virtual std::pair<bool, FHitResult> CalculateCanAssassinate();
	virtual void Assassinate(AActor* Character);

	// Rpc
	UFUNCTION(Server, Reliable)
		virtual void Server_Assassinate(AActor* Actor);

	// Delegate
	UFUNCTION()
		void OnCanInputNextComboEvent();
	UFUNCTION()
		void OnSwingAttackEvent();
	UFUNCTION()
		void OnAttackNextComboEvent();
	UFUNCTION()
		void OnComboEndEvent();
	UFUNCTION()
		void OnAssassinateAttackEvent();
	UFUNCTION()
		void OnAssassinateEndEvent();

public:
	FORCEINLINE bool IsComboAttacking() { return CurComboAttack != 0; }
	virtual UAnimMontage* GetAttackMontageTpp() override;
	virtual UAnimMontage* GetAttackMontageFpp() override;

public:
	FAssassinateDele OnAssassinate;
	FAssassinateEndDele OnAssassinateEnd;

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

	// Assassination
	bool IsAssassinating = false;
	ACreature* CurAssasinatedCreature;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Combat|Assassination", meta = (AllowPrivateAccess = "true"))
		bool bCanAssassination = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Combat|Assassination", meta = (AllowPrivateAccess = "true"))
		float AssassinationDamage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Combat|Assassination", meta = (AllowPrivateAccess = "true"))
		UAnimMontage* AssassinateMontagesTpp;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Combat|Assassination", meta = (AllowPrivateAccess = "true"))
		UAnimMontage* AssassinateMontagesFpp;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Combat|Assassination", meta = (AllowPrivateAccess = "true"))
		UAnimMontage* AssassinatedMontagesTpp;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Combat|Assassination", meta = (AllowPrivateAccess = "true"))
		UAnimMontage* AssassinatedMontagesFpp;

	// Montage
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Montage", Meta = (AllowPrivateAccess = true))
		TArray<UAnimMontage*> AttackMontagesTpp;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Montage", Meta = (AllowPrivateAccess = true))
		TArray<UAnimMontage*> AttackMontagesFpp;
};
