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

	virtual void OnAddOnDelegateEvent(UObject* Object) override;
	virtual void OnRemoveOnDelegateEvent(UObject* Object) override;

	virtual void Assassinate(AActor* Actor);
	virtual std::pair<bool, FHitResult> CalculateCanAssassinate();

	// Rpc
	UFUNCTION(Server, Reliable)
	virtual void Server_Assassinate(AActor* Actor);
	UFUNCTION(NetMulticast, Reliable)
	virtual void Multicast_Assassinate(AActor* Actor);
	UFUNCTION(Server, Reliable)
	virtual void Server_OnAssassinateAttackEvent();
	UFUNCTION(Server, Reliable)
	virtual void Server_OnAssassinateEndEvent();

	// Delegate
	UFUNCTION()
	void OnAssassinateAttackEvent();
	UFUNCTION()
	void OnAssassinateEndEvent();
	virtual void OnBeforeAttackEvent() override;

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
	float AssassinationDamage = 100.f;
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
