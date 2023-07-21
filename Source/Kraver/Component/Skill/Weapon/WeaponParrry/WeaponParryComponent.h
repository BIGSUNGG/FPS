// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Kraver/Component/Skill/Weapon/WeaponComponent.h"
#include "WeaponParryComponent.generated.h"

/**
 * 
 */
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class KRAVER_API UWeaponParryComponent : public UWeaponComponent
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;

protected:
	virtual void ParryStart();
	virtual void ParryEnd();
	virtual void ParryDelayEnd();
	virtual void ParrySuccess();

	// Delegate
	UFUNCTION()
		virtual void OnSubAttackStartEvent();
	UFUNCTION()
		virtual void OnServerBeforeTakeDamageEvent(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser, FKraverDamageResult const& DamageResult);
	// Rpc
	UFUNCTION(Server, Reliable)
		virtual void Server_ParryStart();
	UFUNCTION(Client, Reliable)
		virtual void Client_ParryStart();

	UFUNCTION(Server, Reliable)
		virtual void Server_ParryEnd();
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Montage", Meta = (AllowPrivateAccess = true))
		float ParryDelay = 0.6f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Montage", Meta = (AllowPrivateAccess = true))
		float ParryDuration = 0.4f;
	bool IsParrying = false;
	bool bCanParry = true;
	FTimerHandle ParryDelayTimer;
	FTimerHandle ParryDurationTimer;

	// Montage
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Montage", Meta = (AllowPrivateAccess = true))
		UAnimMontage* ParryMontageTpp;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Montage", Meta = (AllowPrivateAccess = true))
		UAnimMontage* ParryMontageFpp;
};
