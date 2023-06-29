// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Kraver/Kraver.h"
#include "AIController.h"
#include "GuardAIController.generated.h"

/**
 * 
 */
UCLASS()
class KRAVER_API AGuardAIController : public AAIController
{
	GENERATED_BODY()
	AGuardAIController();

protected:
	// Delegate
	UFUNCTION()
		virtual void OnTargetPerceptionUpdatedEvent(AActor* Actor, FAIStimulus Stimulus);
	
public:
	static const FName FindEnemyKey;

protected:
	UPROPERTY(BlueprintReadOnly,VisibleAnywhere, Category = AI)
	TObjectPtr<UAIPerceptionComponent> AIPerceptionComponent = nullptr;
	TObjectPtr<class UAISenseConfig_Sight> AISenseConfigSight = nullptr;

};
