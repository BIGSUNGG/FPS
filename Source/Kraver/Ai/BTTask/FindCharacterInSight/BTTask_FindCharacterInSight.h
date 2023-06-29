// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Kraver/Kraver.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_FindCharacterInSight.generated.h"

/**
 * 
 */
UCLASS()
class KRAVER_API UBTTask_FindCharacterInSight : public UBTTaskNode
{
	GENERATED_BODY()
public:
	UBTTask_FindCharacterInSight();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

};
