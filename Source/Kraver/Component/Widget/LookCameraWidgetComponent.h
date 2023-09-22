// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Kraver/Kraver.h"
#include "Components/WidgetComponent.h"
#include "LookCameraWidgetComponent.generated.h"

/**
 * 
 */
UCLASS()
class KRAVER_API ULookCameraWidgetComponent : public UWidgetComponent
{
	GENERATED_BODY()
	
public:
	ULookCameraWidgetComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

};
