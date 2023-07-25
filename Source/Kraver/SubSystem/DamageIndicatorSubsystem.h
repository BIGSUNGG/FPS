// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Kraver/Kraver.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "DamageIndicatorSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class KRAVER_API UDamageIndicatorSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
public:
	UDamageIndicatorSubsystem();

	/** Implement this for initialization of instances of the system */
	virtual void Initialize(FSubsystemCollectionBase& Collection);

	/** Implement this for deinitialization of instances of the system */
	virtual void Deinitialize() override { Super::Deinitialize(); }

	void OnLocalPlayerBeginPlay(class AKraverPlayer* Player);
protected:
	// Delegate
	
		// Give Damage
	UFUNCTION()
		virtual void OnClientGiveDamagePointSuccessEvent(AActor* DamagedActor, float DamageAmount, FPointDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser, FKraverDamageResult const& DamageResult);
	UFUNCTION()
		virtual void OnClientGiveDamageRadialSuccessEvent(AActor* DamagedActor, float DamageAmount, FRadialDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser, FKraverDamageResult const& DamageResult);

		// Take Damage
	UFUNCTION()
		virtual void OnClientAfterTakePointDamageEvent(float DamageAmount, FPointDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser, FKraverDamageResult const& DamageResult);
	UFUNCTION()
		virtual void OnClientAfterTakeRadialDamageEvent(float DamageAmount, FRadialDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser, FKraverDamageResult const& DamageResult);

	UFUNCTION()
		virtual void OnDestroyEvent(AActor* Actor);

	// Function
	virtual void OnClientGiveDamageSuccessEvent(AActor* DamagedActor, float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser, FKraverDamageResult const& DamageResult);


protected:
	class AKraverPlayer* KraverPlayer;

	TMap<class ACreature*, class AFloatingDamage*> FloatingWidgets;
	bool DuplicationFloatingDamage = true;

	TMap<class ACreature*, class UDamageDirectionWidget*> DamagedDirectionWidgets;

	class UClass* DamagedDirectionClass;

};
