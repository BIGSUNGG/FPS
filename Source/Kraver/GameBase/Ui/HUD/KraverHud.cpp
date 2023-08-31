// Fill out your copyright notice in the Description page of Project Settings.


#include "KraverHUD.h"
#include "Kraver/GameBase/Ui/Widget/Interaction/InteractionWidget.h"
#include "Kraver/GameBase/Ui/Widget/Combat/CombatWidget.h"
#include "Kraver/GameBase/Ui/Widget/KillLog/KillLogWidget.h"
#include "Kraver/GameBase/Ui/Widget/Combat/CombatWidget.h"
#include Creature_h
#include "Kraver/Character/Creature/Soldier/KraverPlayer/KraverPlayer.h"
#include KraverPlayerState_h
#include KraverGameState_h

AKraverHUD::AKraverHUD()
{
	KillLogWidgets.SetNum(5);
}

void AKraverHUD::BeginPlay()
{
	Super::BeginPlay();

	APlayerController* PlayerController = GetOwningPlayerController();
	OnNewLocalPlayerEvent(Cast<AKraverPlayer>(PlayerController->GetCharacter()));

	InteractionWidget = CreateWidget<UInteractionWidget>(PlayerController, InteractionWidgetClass);
	if (InteractionWidget)
		InteractionWidget->AddToViewport();

	CombatWidget = CreateWidget<UCombatWidget>(PlayerController, CombatWidgetClass);
	if(CombatWidget)
		CombatWidget->AddToViewport();
		
	for (int i = 0; i < KillLogWidgets.Num(); i++)
	{
		UKillLogWidget* KillLogWidget = CreateWidget<UKillLogWidget>(PlayerController, KillLogWidgetClass);
		if (KillLogWidget)
		{
			KillLogWidgets[i] = KillLogWidget;
			KillLogWidget->AddToViewport();
		}
	}

}

void AKraverHUD::DrawHUD()
{
	Super::DrawHUD();

	if (!PlayerState)
	{
		PlayerState = GetOwningPlayerController()->GetPlayerState<AKraverPlayerState>();
		if (PlayerState)
		{
			PlayerState->OnNewLocalPlayer.AddDynamic(this, &ThisClass::OnNewLocalPlayerEvent);
		}
	}
	if (!GameState)
	{
		GameState = GetWorld()->GetGameState<AKraverGameState>();
		if (GameState)
		{
			GameState->OnCreatureDeath.AddDynamic(this, &ThisClass::OnCreatureDeathEvent);
		}
	}

	if (HitmarkAppearanceTime > 0.f)
	{
		HitmarkAppearanceTime -= GetWorld()->GetDeltaSeconds();
	}

	FVector2D ViewportSize;
	if (GEngine)
	{
		GEngine->GameViewport->GetViewportSize(ViewportSize);
		const FVector2D ViewportCenter(ViewportSize.X / 2.f, ViewportSize.Y / 2.f);

		float SpreadScaled = CrosshairSpreadMax * HUDPackage.CrosshairSpread;

		if(bDrawCrosshair)
		{
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
		}

		if (HitmarkAppearanceTime > 0.f)
		{
			if (bHitmartCritical)
				DrawCrosshair(Hitmark, ViewportCenter, FVector2D::ZeroVector, FLinearColor::Red);
			else
				DrawCrosshair(Hitmark, ViewportCenter, FVector2D::ZeroVector);
		}
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

void AKraverHUD::ApplyKillLogPos()
{
	for (int i = 0; i < KillLogWidgets.Num(); i++)
		KillLogWidgets[i]->SetPositionInViewport(FVector2D(0, 80 * i));
}

void AKraverHUD::OnClientGiveDamageSuccessEvent(AActor* DamagedActor, float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser, FKraverDamageResult const& DamageResult)
{
	if(DamageResult.bAlreadyDead)
		return;

	ACreature* Creature = Cast<ACreature>(DamagedActor);
	if(Creature)
		HitmarkAppearanceTime = 0.05f;

	bHitmartCritical = DamageResult.bCritical;
}

void AKraverHUD::OnNewLocalPlayerEvent(AKraverPlayer* NewCreature)
{
	OwnerCreature = NewCreature;
	if (OwnerCreature)
	{
		OwnerCreature->CombatComponent->OnClientGiveAnyDamageSuccess.AddDynamic(this, &AKraverHUD::OnClientGiveDamageSuccessEvent);
	}
}

void AKraverHUD::OnCreatureDeathEvent(class ACreature* DeadCreature, class AController* VictimController, AActor* AttackerActor, AController* AttackerController, FKraverDamageResult const& DamageResult)
{
	for (int i = KillLogWidgets.Num() - 1; i >= 1; i--)
	{
		UUserWidget* TempWidget = KillLogWidgets[i - 1];
		KillLogWidgets[i - 1] = KillLogWidgets[i];
		KillLogWidgets[i] = TempWidget;
	}

	UKillLogWidget* KillLogWidget = Cast<UKillLogWidget>(KillLogWidgets[0]);
	KillLogWidget->Initialize(AttackerController ? AttackerController->GetName() : AttackerActor->GetName(), VictimController ? VictimController->GetName() : DeadCreature->GetName());
	ApplyKillLogPos();
}

void AKraverHUD::SetInteractWidget(bool value)
{
	if(value)
	{
		InteractionWidget->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		InteractionWidget->SetVisibility(ESlateVisibility::Hidden);
	}

}
