// Fill out your copyright notice in the Description page of Project Settings.


#include "ControlHud.h"

void AControlHud::BeginPlay()
{
	Super::BeginPlay();

	APlayerController* PlayerController = GetOwningPlayerController();
	ControlWidget = CreateWidget<UUserWidget>(PlayerController, ControlWidgetClass);
	if (ControlWidget)
		ControlWidget->AddToViewport();

}
