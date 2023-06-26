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
	UFUNCTION()
		virtual void OnClientGiveDamageSuccessEvent(AActor* DamagedActor, float DamageAmount, FKraverDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser, FKraverDamageResult const& DamageResult);
	UFUNCTION()
		virtual void OnClientAfterTakeDamageEvent(float DamageAmount, FKraverDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser, FKraverDamageResult const& DamageResult);
	UFUNCTION()
		virtual void OnDestroyEvent(AActor* Actor);

protected:
	class AKraverPlayer* KraverPlayer;
	TMap<class ACreature*, class AFloatingDamage*> FloatingWidgets;
	bool DuplicationDamage = true;

	class UClass* DamagedDirectionClass;

};
