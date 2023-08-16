// Fill out your copyright notice in the Description page of Project Settings.


#include "DamageDirectionWidget.h"

bool UDamageDirectionWidget::Initialize(AActor* InActor, FVector InLocation)
{
	Actor = InActor;
	HitLocation = InLocation;
	return true;
}

void UDamageDirectionWidget::StartWidget_Implementation()
{

}

void UDamageDirectionWidget::EndWidget_Implementation()
{

}
