// Fill out your copyright notice in the Description page of Project Settings.


#include "LookCameraWidgetComponent.h"

ULookCameraWidgetComponent::ULookCameraWidgetComponent()
{
	SetVisibility(true);
	SetWidgetSpace(EWidgetSpace::World);
}

void ULookCameraWidgetComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// 카메라 바라보기
	APlayerCameraManager* CameraManager = UGameplayStatics::GetPlayerCameraManager(this, 0);
	if (CameraManager)
	{
		FRotator LookRotation = UKismetMathLibrary::FindLookAtRotation(GetComponentLocation(), CameraManager->GetTransformComponent()->GetComponentLocation());
		SetWorldRotation(LookRotation);
	}
}
