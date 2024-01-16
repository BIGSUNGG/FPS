// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_AttackTo.h"
#include CombatComponent_h
#include "Kraver/Ai/Controller/GuardAIController.h"
#include Creature_h

UBTTask_AttackTo::UBTTask_AttackTo()
{

}

EBTNodeResult::Type UBTTask_AttackTo::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);

	UCombatComponent* CombatComp = OwnerComp.GetAIOwner()->GetPawn()->FindComponentByClass<UCombatComponent>();
	if (!CombatComp)
	{
		CombatComp->OnLocal_AttackEndDelegate.Broadcast();
		return EBTNodeResult::Failed;
	}

	ACreature* Creature = dynamic_cast<ACreature*>(OwnerComp.GetAIOwner()->GetBlackboardComponent()->GetValueAsObject(AGuardAIController::FindEnemyKey));
	if (!Creature)
	{
		CombatComp->OnLocal_AttackEndDelegate.Broadcast();
		return EBTNodeResult::Failed;
	}

	FHitResult HitResult;
	bool bSuccess = GetWorld()->LineTraceSingleByChannel(
		HitResult,
		OwnerComp.GetAIOwner()->GetPawn()->FindComponentByClass<UCameraComponent>()->GetComponentLocation(),
		Creature->GetActorLocation(),
		ECC_ONLY_OBJECT
		);

	if (bSuccess)
	{
		CombatComp->OnLocal_AttackEndDelegate.Broadcast();
		return EBTNodeResult::Failed;
	}

	CombatComp->OnLocalAttackStartDelegate.Broadcast();

	return Result;
}
