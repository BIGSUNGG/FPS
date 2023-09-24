// Fill out your copyright notice in the Description page of Project Settings.


#include "ControlHud.h"

AControlHud::AControlHud()
{
	static ConstructorHelpers::FClassFinder<UContentWidget> CONTROL_WIDGET(TEXT("/Game/ProjectFile/GameBase/Widget/WBP_ControlGameWidget.WBP_ControlGameWidget"));
	if (CONTROL_WIDGET.Succeeded())
		ControlWidgetClass = CONTROL_WIDGET.Class;
}

void AControlHud::BeginPlay()
{
	Super::BeginPlay();

	APlayerController* PlayerController = GetOwningPlayerController();
	ControlWidget = CreateWidget<UUserWidget>(PlayerController, ControlWidgetClass);
	if (ControlWidget)
		ControlWidget->AddToViewport();

}
