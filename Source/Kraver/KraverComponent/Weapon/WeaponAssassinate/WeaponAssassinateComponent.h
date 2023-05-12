// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Kraver/KraverComponent/Weapon/WeaponComponent.h"
#include "WeaponAssassinateComponent.generated.h"

class AMelee;

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
	UFUNCTION(Server, reliable)
		virtual void Server_Assassinate(AActor* Actor);
	UFUNCTION(Server, reliable)
		virtual void Server_OnAssassinateAttackEvent();
	UFUNCTION(NetMulticast, reliable)
		virtual void Multicast_OnAssassinateAttackEvent();
	UFUNCTION(Server, reliable)
		virtual void Server_OnAssassinateEndEvent();
	UFUNCTION(NetMulticast, reliable)
		virtual void Multicast_OnAssassinateEndEvent();

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
	AMelee* OwnerMelee = nullptr;

	bool IsAssassinating = false;
	UPROPERTY(Replicated)
		ACreature* CurAssassinatedCreature;
	UPROPERTY(BlueprintReadWrite, Category = "Data|Combat|Assassination", meta = (AllowPrivateAccess = "true"))
		bool bCanAssassination = true;
	UPROPERTY(BlueprintReadWrite, Category = "Data|Combat|Assassination", meta = (AllowPrivateAccess = "true"))
		float AssassinationDamage = 100.f;
	UPROPERTY(BlueprintReadWrite, Category = "Data|Combat|Assassination", meta = (AllowPrivateAccess = "true"))
		UAnimMontage* AssassinateMontagesTpp;
	UPROPERTY(BlueprintReadWrite, Category = "Data|Combat|Assassination", meta = (AllowPrivateAccess = "true"))
		UAnimMontage* AssassinateMontagesFpp;
	UPROPERTY(BlueprintReadWrite, Category = "Data|Combat|Assassination", meta = (AllowPrivateAccess = "true"))
		UAnimMontage* AssassinatedMontagesTpp;
	UPROPERTY(BlueprintReadWrite, Category = "Data|Combat|Assassination", meta = (AllowPrivateAccess = "true"))
		UAnimMontage* AssassinatedMontagesFpp;
};
