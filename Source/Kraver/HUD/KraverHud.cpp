// Fill out your copyright notice in the Description page of Project Settings.


#include "KraverHUD.h"
#include "Kraver/Creature/Creature.h"

AKraverHUD::AKraverHUD()
{
	InteractionWidget = CreateDefaultSubobject<UUserWidget>(TEXT("InteractionWidget"));
	CombatWidget = CreateDefaultSubobject<UUserWidget>(TEXT("CombatWidget"));
}

void AKraverHUD::DrawHUD()
{
	Super::DrawHUD();

	FVector2D ViewportSize;
	if (GEngine && bDrawCrosshair)
	{
		GEngine->GameViewport->GetViewportSize(ViewportSize);
		const FVector2D ViewportCenter(ViewportSize.X / 2.f, ViewportSize.Y / 2.f);

		float SpreadScaled = CrosshairSpreadMax * HUDPackage.CrosshairSpread;

		if (HUDPackage.CrosshairsCenter)
		{
			FVector2D Spread(0.f, 0.f);
			DrawCrosshair(HUDPackage.CrosshairsCenter, ViewportCenter, Spread);
		}
		if (HUDPackage.CrosshairsLeft)
		{
			FVector2D Spread(-SpreadScaled, 0.f);
			DrawCrosshair(HUDPackage.CrosshairsLeft, ViewportCenter, Spread);
		}
		if (HUDPackage.CrosshairsRight)
		{
			FVector2D Spread(SpreadScaled, 0.f);
			DrawCrosshair(HUDPackage.CrosshairsRight, ViewportCenter, Spread);
		}
		if (HUDPackage.CrosshairsTop)
		{
			FVector2D Spread(0.f, -SpreadScaled);
			DrawCrosshair(HUDPackage.CrosshairsTop, ViewportCenter, Spread);
		}
		if (HUDPackage.CrosshairsBottom)
		{
			FVector2D Spread(0.f, SpreadScaled);
			DrawCrosshair(HUDPackage.CrosshairsBottom, ViewportCenter, Spread);
		}

		if (HitmarkAppearanceTime > 0.f)
		{
			HitmarkAppearanceTime -= GetWorld()->GetDeltaSeconds();

			if(bHitmartCritical)
				DrawCrosshair(Hitmark, ViewportCenter, FVector2D::ZeroVector, FLinearColor::Red);
			else
				DrawCrosshair(Hitmark, ViewportCenter, FVector2D::ZeroVector);
		}
	}
}

void AKraverHUD::BeginPlay()
{
	Super::BeginPlay();

	APlayerController* PlayerController = GetOwningPlayerController();
	InteractionWidget = CreateWidget<UInteractionWidget>(PlayerController, InteractionWidgetClass);
	CombatWidget = CreateWidget<UCombatWidget>(PlayerController, CombatWidgetClass);
	if(CombatWidget)
		CombatWidget->AddToViewport();
	else
		UE_LOG(LogTemp, Fatal, TEXT("CombatWidget is null"));

	OwnerCreature = Cast<ACreature>(PlayerController->GetCharacter());
	if (OwnerCreature)
	{
		OwnerCreature->CombatComponent->OnGiveDamage.AddDynamic(this, &AKraverHUD::OnGiveDamageEvent);
	}
}

void AKraverHUD::DrawCrosshair(UTexture2D* Texture, FVector2D ViewportCenter, FVector2D Spread, FLinearColor Color)
{
	const float TextureWidth = Texture->GetSizeX();
	const float TextureHeight = Texture->GetSizeY();
	const FVector2D TextureDrawPoint(
		ViewportCenter.X - (TextureWidth / 2.f) + Spread.X,
		ViewportCenter.Y - (TextureHeight / 2.f) + Spread.Y
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
		Color
	);
}

void AKraverHUD::OnGiveDamageEvent(AActor* DamagedActor, float DamageAmount, FKraverDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if(Cast<ACreature>(DamagedActor))
		HitmarkAppearanceTime = 0.05f;

	bHitmartCritical = DamageEvent.HitInfo.BoneName == "head";
}

void AKraverHUD::SetInteractWidget(bool value)
{
	return;

	if (value == true && InteractionWidget)
	{
		FVector2D ViewportSize;
		if (GEngine)
		{
			GEngine->GameViewport->GetViewportSize(ViewportSize);
			const FVector2D ViewportCenter(ViewportSize.X / 2.f, ViewportSize.Y / 2.f);
			const FVector2D WidgetHalfSize(165, 30);
			float Y = ViewportCenter.Y * 0.6f;
			FVector2D WidgetPosition = ViewportCenter - WidgetHalfSize;
			WidgetPosition.Y += Y;
			InteractionWidget->SetPositionInViewport(WidgetPosition);
		}
		InteractionWidget->AddToViewport();
	}
	else if (value == false && InteractionWidget)
	{
		InteractionWidget->RemoveFromParent();
	}
}
