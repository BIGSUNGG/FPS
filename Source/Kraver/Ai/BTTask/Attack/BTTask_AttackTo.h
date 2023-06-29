// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Kraver/Kraver.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_AttackTo.generated.h"

/**
 * 
 */
UCLASS()
class KRAVER_API UBTTask_AttackTo : public UBTTaskNode
{
	GENERATED_BODY()
public:
	UBTTask_AttackTo();
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	
protected:
	UPROPERTY(EditAnywhere, Category=Blackboard)
		struct FBlackboardKeySelector BlackboardKey;
};
