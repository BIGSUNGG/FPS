// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_AttackTo.h"
#include "Kraver/Component/Combat/CombatComponent.h"

UBTTask_AttackTo::UBTTask_AttackTo()
{

}

EBTNodeResult::Type UBTTask_AttackTo::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);

	UCombatComponent* CombatComp = OwnerComp.GetAIOwner()->GetPawn()->FindComponentByClass<UCombatComponent>();
	if(!CombatComp)
		return EBTNodeResult::Failed;

	CombatComp->OnAttackStartDelegate.Broadcast();

	return Result;
}
