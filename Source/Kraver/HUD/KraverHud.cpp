// Fill out your copyright notice in the Description page of Project Settings.


#include "KraverHUD.h"

AKraverHUD::AKraverHUD()
{
	InteractionWidget = CreateDefaultSubobject<UInteractionWidget>(TEXT("InteractionWidget"));
	CombatWidget = CreateDefaultSubobject<UCombatWidget>(TEXT("CombatWidget"));
}

void AKraverHUD::DrawHUD()
{
	Super::DrawHUD();

	FVector2D ViewportSize;
	GEngine->GameViewport->GetViewportSize(ViewportSize);

	if (GEngine)
	{
		const FVector2D ViewportCenter(ViewportSize.X / 2.f,ViewportSize.Y/2.f);

		if (HUDPackage.CrosshairCenter)
		{
			DrawCrosshair(HUDPackage.CrosshairCenter,ViewportCenter);
		}
		if (HUDPackage.CrosshairLeft)
		{
			DrawCrosshair(HUDPackage.CrosshairLeft, ViewportCenter);
		}
		if (HUDPackage.CrosshairRight)
		{
			DrawCrosshair(HUDPackage.CrosshairRight, ViewportCenter);
		}
		if (HUDPackage.CrosshairTop)
		{
			DrawCrosshair(HUDPackage.CrosshairTop, ViewportCenter);
		}
		if (HUDPackage.CrosshairBottom)
		{
			DrawCrosshair(HUDPackage.CrosshairBottom, ViewportCenter);
		}

	}
}

void AKraverHUD::BeginPlay()
{
	Super::BeginPlay();

	APlayerController* PlayerController = GetOwningPlayerController();
	InteractionWidget = CreateWidget<UInteractionWidget>(PlayerController, InteractionWidgetClass);
	CombatWidget = CreateWidget<UCombatWidget>(PlayerController, CombatWidgetClass);
	if (CombatWidget)
		CombatWidget->AddToViewport();
}

void AKraverHUD::DrawCrosshair(UTexture2D* Texture, FVector2D ViewportCenter)
{
	const float TextureWidth = Texture->GetSizeX();
	const float TextureHeight = Texture->GetSizeY();
	const FVector2D TextureDrawPoint(
		ViewportCenter.X - (TextureWidth / 2.f),
		ViewportCenter.Y - (TextureHeight / 2.f)
	);
	DrawTexture(
		Texture,
		TextureDrawPoint.X,
		TextureDrawPoint.Y,
		TextureWidth,
		TextureHeight,
		0.f,
		0.f,
		1.f,
		1.f,
		FLinearColor::White
	);
}

void AKraverHUD::SetInteractWidget(bool value)
{
	if (value == true && InteractionWidget != nullptr && !InteractionWidget->IsVisible())
	{
		FVector2D ViewportSize;
		if (GEngine)
		{
			GEngine->GameViewport->GetViewportSize(ViewportSize);
			const FVector2D ViewportCenter(ViewportSize.X / 2.f, ViewportSize.Y / 2.f);
			const FVector2D WidgetHalfSize(165,30);
			float Y = ViewportCenter.Y * 0.6f;
			FVector2D WidgetPosition = ViewportCenter - WidgetHalfSize;
			WidgetPosition.Y += Y;
			InteractionWidget->SetPositionInViewport(WidgetPosition);
		}
		InteractionWidget->AddToViewport();
	}
	else if(value == false && InteractionWidget != nullptr && InteractionWidget->IsVisible())
	{
		InteractionWidget->RemoveFromParent();
	}
}
	