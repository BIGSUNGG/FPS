// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_FindCharacterInSight.h"

UBTTask_FindCharacterInSight::UBTTask_FindCharacterInSight()
{

}

EBTNodeResult::Type UBTTask_FindCharacterInSight::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);

	return Result;
}
