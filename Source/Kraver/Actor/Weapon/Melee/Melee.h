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

	virtual void ComboStart(); // 콤보를 시작할 때 호출
	virtual void NextComboAttack(); // 다음 콤보 공격할 때 호출
	virtual void ComboEnd(); // 콤보가 끝날때 호출

	virtual void StartSwingEvent(); // 무기를 휘두를 때 호출

	// Rpc
	UFUNCTION(Server, Reliable)
	virtual void Server_SwingResult(const TArray<FHitResult>& Results);

	UFUNCTION(Server, Reliable)
	virtual void Server_SwingStart(int Combo);
	UFUNCTION(NetMulticast, Reliable)
	virtual void Multicast_SwingStart(int Combo);

	// Delegate
	UFUNCTION()
	void OnCanInputNextComboEvent(); // 몽타주에서 다음 공격 입력이 가능한지 알리는 노티파이가 호출되었을 때 호출
	UFUNCTION()
	void OnSwingAttackEvent(); // 몽타주에서 공격을 확인하는 노티파이가 호출되었을 때 호출
	UFUNCTION()
	void OnAttackNextComboEvent(); // 몽타주에서 다음 콤보를 실행할지 확인하는 노티파이가 호출되었을 때 호출
	UFUNCTION()
	void OnComboEndEvent(); // 다음 콤보가 실행되지않고 몽타주에서 콤보가 끝났다는것을 알리는 노티파이가 호출되었을 때 호출
	virtual void Attack() override;

public:
	// Getter Setter
	FORCEINLINE bool IsComboAttacking() { return CurComboAttack != 0; }

protected:	
	// Combo
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Data|Combat|Combo", meta = (AllowPrivateAccess = "true"))
	int CurComboAttack = 0; // 현재 무슨 콤보 중인지
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Combat|Combo", meta = (AllowPrivateAccess = "true"))
	int MaxComboAttack = 0; // 최대 콤보 수
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Combat|Combo", meta = (AllowPrivateAccess = "true"))
	bool bAutomaticCombo = false; // 자동 콤보가 가능한지 (계속 공격 중일경우 bCanInputNextCombo가 true로 변경 되었을때 자동으로 bInputNextCombo도 true로 변경)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Combat|Combo", meta = (AllowPrivateAccess = "true"))
	bool bAutomaticRepeatCombo = false; // 자동으로 콤보가 반복이 가능한지 (MaxComboAttack다음 이후 다시 0으로 시작)
	bool bInputNextCombo = false; // 다음 콤보 공격이 예약되었는지
	bool bCanInputNextCombo = false; // 다음 콤보 공격 입력이 가능한지

	// Montage
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Montage", Meta = (AllowPrivateAccess = true))
	TArray<UAnimMontage*> AttackMontagesTpp;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Montage", Meta = (AllowPrivateAccess = true))
	TArray<UAnimMontage*> AttackMontagesFpp;
};
