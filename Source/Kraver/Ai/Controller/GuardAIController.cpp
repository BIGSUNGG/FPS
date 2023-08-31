// Fill out your copyright notice in the Description page of Project Settings.


#include "GuardAIController.h"
#include Creature_h
#include "Kraver/Character/Creature/Soldier/KraverPlayer/KraverPlayer.h"

const FName AGuardAIController::FindEnemyKey(TEXT("FindEnemy"));

AGuardAIController::AGuardAIController()
{
	AIPerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>("AIPerceptionComponent");
	AISenseConfigSight = CreateDefaultSubobject<UAISenseConfig_Sight>("AISenseConfigSight");
	AISenseConfigSight->SightRadius = 500.f;
	AISenseConfigSight->LoseSightRadius = 500.f; 
	AISenseConfigSight->PeripheralVisionAngleDegrees = 60.f;
	AISenseConfigSight->DetectionByAffiliation.bDetectEnemies = true;
	AISenseConfigSight->DetectionByAffiliation.bDetectFriendlies = true;
	AISenseConfigSight->DetectionByAffiliation.bDetectNeutrals = true;

	AIPerceptionComponent->ConfigureSense(*AISenseConfigSight);
	AIPerceptionComponent->SetDominantSense(UAISenseConfig_Sight::StaticClass());

	// Add an team id. I use e.g enums for the affiliation types	
	AAIController::SetGenericTeamId(FGenericTeamId(1));
	AIPerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(this, &ThisClass::OnTargetPerceptionUpdatedEvent);
}

void AGuardAIController::OnTargetPerceptionUpdatedEvent(AActor* Actor, FAIStimulus Stimulus)
{
	TArray<AActor*> OutActors;
	AIPerceptionComponent->GetCurrentlyPerceivedActors(AISenseConfigSight->Implementation, OutActors);

	if (OutActors.Contains(Actor))
	{
		AKraverPlayer* Player = Cast<AKraverPlayer>(Actor);	
		if (Player)
		{
			GetBlackboardComponent()->SetValueAsObject(FindEnemyKey, Player);

			FString Message = "Find : " + Actor->GetName();
			GEngine->AddOnScreenDebugMessage(
				0,
				1.f,
				FColor::White,
				Message
			);
		}
	}
	else
	{
		if(GetBlackboardComponent()->GetValueAsObject(FindEnemyKey) == Actor)
			GetBlackboardComponent()->SetValueAsObject(FindEnemyKey, nullptr);

		FString Message = "Lose : " + Actor->GetName();
		GEngine->AddOnScreenDebugMessage(
			0,
			1.f,
			FColor::White,
			Message
		);
	}
}
