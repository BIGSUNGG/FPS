// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Kraver/Kraver.h"
#include WeaponComponent_h
#include "WeaponAssassinateComponent.generated.h"

/**
 * 
 */

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class KRAVER_API UWeaponAssassinateComponent : public UWeaponComponent
{
	GENERATED_BODY()

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void OnLocal_AddOnDelegateEvent(UObject* Object) override;
	virtual void OnLocal_RemoveOnDelegateEvent(UObject* Object) override;
	virtual void OnServer_AddOnDelegateEvent(UObject* Object) override;
	virtual void OnServer_RemoveOnDelegateEvent(UObject* Object) override;

	virtual void OnServer_Assassinate(AActor* Actor); // 암살 시작시 호출
	virtual void AssassinateEvent();
	virtual std::pair<bool, FHitResult> CalculateCanAssassinate();

	// Rpc
	UFUNCTION(NetMulticast, Reliable)
	virtual void Multicast_Assassinate(AActor* Actor);

	// Delegate
	UFUNCTION()
	void OnAssassinateAttackEvent(); // Montage에서 암살 데미지를 주는 노티파이가 호출되었을때 호출
	UFUNCTION()
	void OnAssassinateEndEvent(); // Montage에서 암살이 끝나는 노티파이가 호출되었을때 호출
	UFUNCTION()
	void OnServer_AssassinateAttackEvent(); // Montage에서 암살 데미지를 주는 노티파이가 호출되었을때 호출
	UFUNCTION()
	void OnServer_AssassinateEndEvent(); // Montage에서 암살이 끝나는 노티파이가 호출되었을때 호출

	UFUNCTION()
	virtual void OnServerBeforeAttackEvent(); // Melee에서 공격전에 암살이 가능한지 확인하기 위해 호출

public:
	FAssassinateDele OnAssassinate;
	FAssassinateEndDele OnAssassinateEnd;

protected:
	class AMelee* OwnerMelee = nullptr;

	bool IsAssassinating = false;
	UPROPERTY(Replicated)
	ACreature* CurAssassinatedCreature;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Combat|Assassination", meta = (AllowPrivateAccess = "true"))
	bool bCanAssassination = true;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Combat|Assassination", meta = (AllowPrivateAccess = "true"))
	float AssassinationDamage = 1000.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Combat|Assassination", meta = (AllowPrivateAccess = "true"))
	UAnimMontage* AssassinateMontagesTpp;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Combat|Assassination", meta = (AllowPrivateAccess = "true"))
	UAnimMontage* AssassinateMontagesFpp;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Combat|Assassination", meta = (AllowPrivateAccess = "true"))
	UAnimMontage* AssassinatedMontagesTpp;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Combat|Assassination", meta = (AllowPrivateAccess = "true"))
	UAnimMontage* AssassinatedMontagesFpp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Combat|Attack", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UKraverDamageType> AssassinateDamageType;
};
