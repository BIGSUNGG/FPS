// Fill out your copyright notice in the Description page of Project Settings.


#include "KraverHUD.h"
#include "Kraver/GameBase/Ui/Widget/Interaction/InteractionWidget.h"
#include "Kraver/GameBase/Ui/Widget/Combat/CombatWidget.h"
#include "Kraver/GameBase/Ui/Widget/KillLog/KillLogWidget.h"
#include "Kraver/GameBase/Ui/Widget/Combat/CombatWidget.h"
#include Creature_h
#include KraverPlayer_h
#include KraverGameState_h
#include KraverPlayerState_h
#include TeamPlayerState_h
#include RespawnTimerWidget_h
#include GameFinishWidget_h
#include FloatingDamage_h
#include "Kraver/GameBase/Ui/Widget/DamageDirection/DamageDirectionWidget.h"

AKraverHud::AKraverHud()
{
	// Object
	static ConstructorHelpers::FObjectFinder<ULevelSequence> LEVEL_FADE_SEQUENCE(TEXT("/Script/LevelSequence.LevelSequence'/Game/InfimaGames/LowPolyShooterPack/Data/Sequences/LS_Fade_Level.LS_Fade_Level'"));
	if (LEVEL_FADE_SEQUENCE.Succeeded())
		LevelFadeSquence = LEVEL_FADE_SEQUENCE.Object;

	// Class
		// Widget
	static ConstructorHelpers::FClassFinder<UCombatWidget> COMBAT_WIDGET(TEXT("/Game/ProjectFile/GameBase/Widget/WBP_CombatWidget.WBP_CombatWidget_C"));
	if (COMBAT_WIDGET.Succeeded())
		CombatWidgetClass = COMBAT_WIDGET.Class;

	static ConstructorHelpers::FClassFinder<UInteractionWidget> INTERACTION_WIDGET(TEXT("/Game/ProjectFile/GameBase/Widget/WBP_InteractionWidget.WBP_InteractionWidget_C"));
	if (INTERACTION_WIDGET.Succeeded())
		InteractionWidgetClass = INTERACTION_WIDGET.Class;

	static ConstructorHelpers::FClassFinder<UKillLogWidget> KILL_LOG_WIDGET(TEXT("/Game/ProjectFile/GameBase/Widget/WBP_KillLog.WBP_KillLog_C"));
	if (KILL_LOG_WIDGET.Succeeded())
		KillLogWidgetClass = KILL_LOG_WIDGET.Class;

	static ConstructorHelpers::FClassFinder<URespawnTimerWidget> RESPAWN_TIMER_WIDGET(TEXT("/Game/ProjectFile/GameBase/Widget/WBP_RespawnTimer.WBP_RespawnTimer_C"));
	if (RESPAWN_TIMER_WIDGET.Succeeded())
		RespawnTimerWidgeClass = RESPAWN_TIMER_WIDGET.Class;

	static ConstructorHelpers::FClassFinder<UGameFinishWidget> GAMEFINISH_WIDGET(TEXT("/Game/ProjectFile/GameBase/Widget/WBP_GameFinishWidget.WBP_GameFinishWidget_C"));
	if (GAMEFINISH_WIDGET.Succeeded())
		GameFinishWidgeClass = GAMEFINISH_WIDGET.Class;

		// FloatingDamage
	static ConstructorHelpers::FClassFinder<UDamageDirectionWidget> DamagedDirectionFinder(TEXT("/Game/ProjectFile/GameBase/Widget/WBP_DamagedDirection.WBP_DamagedDirection_C"));
	if (DamagedDirectionFinder.Succeeded())
		DamagedDirectionClass = DamagedDirectionFinder.Class;

	KillLogWidgets.SetNum(5);
}

void AKraverHud::BeginPlay()
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

	RespawnTimerWidge = CreateWidget<URespawnTimerWidget>(PlayerController, RespawnTimerWidgeClass);
	if (RespawnTimerWidge)
		RespawnTimerWidge->AddToViewport();

	GameFinishWidge = CreateWidget<UGameFinishWidget>(PlayerController, GameFinishWidgeClass);
	if(GameFinishWidge)
		GameFinishWidge->AddToViewport();

	FindGameState();
	FindPlayerState();
}

void AKraverHud::DrawHUD()
{
	Super::DrawHUD();

	if (HitmarkAppearanceTime > 0.f)
	{
		HitmarkAppearanceTime -= GetWorld()->GetDeltaSeconds();
		if (HitmarkAppearanceTime < 0.f)
			HitmarkAppearanceTime = 0.f;
	}

	// 화면 센터 위치 구하기
	FVector2D ViewportSize;
	GEngine->GameViewport->GetViewportSize(ViewportSize);
	const FVector2D ViewportCenter(ViewportSize.X / 2.f, ViewportSize.Y / 2.f);

	// 킬로그 Y축 위치 정렬
	for (int i = 0; i < KillLogWidgets.Num(); i++)
	{
		if (KillLogWidgets[i])
			KillLogWidgets[i]->SetPositionInViewport(FVector2D(ViewportCenter.X, 40 * i), true);
	}

	if (GEngine)
	{
		float SpreadScaled = CrosshairSpreadMax * HUDPackage.CrosshairSpread;

		// 1인칭 무기 렌더
		if (KraverPlayer && KraverPlayer->GetFppCaptureComp() && KraverPlayer->GetFppCaptureComp()->TextureTarget)
		{
			//KraverPlayer->GetFppCaptureComp()->TextureTarget->SizeX = ViewportSize.X;
			//KraverPlayer->GetFppCaptureComp()->TextureTarget->SizeY = ViewportSize.Y;

			//UTextureRenderTarget2D* Texture = KraverPlayer->GetFppCaptureComp()->TextureTarget;
			//const float TextureWidth = ViewportSize.X;
			//const float TextureHeight = ViewportSize.Y;
			//const FVector2D TextureDrawPoint(
			//	ViewportCenter.X - (TextureWidth / 2.f),
			//	ViewportCenter.Y - (TextureHeight / 2.f)
			//);
			//DrawTexture(
			//	Texture,
			//	TextureDrawPoint.X,
			//	TextureDrawPoint.Y,
			//	TextureWidth,
			//	TextureHeight,
			//	0.f,
			//	0.f,
			//	1.f,
			//	1.f
			//);
		}

		// 크로스헤어 렌더
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

		// 히트마크 렌더
		if (HitmarkAppearanceTime > 0.f)
		{
			if (bHitmartCritical)
				DrawCrosshair(Hitmark, ViewportCenter, FVector2D::ZeroVector, FLinearColor::Red);
			else
				DrawCrosshair(Hitmark, ViewportCenter, FVector2D::ZeroVector);
		}
	}
}

void AKraverHud::DrawCrosshair(UTexture2D* Texture, FVector2D ViewportCenter, FVector2D Spread, FLinearColor Color)
{
	if (bGameFinish) return;

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

void AKraverHud::OnClientAfterTakePointDamageEvent(float DamageAmount, FPointDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser, FKraverDamageResult const& DamageResult)
{
	CreateDamagedDirection(DamageAmount, DamageEvent, EventInstigator, DamageCauser, DamageResult);
}

void AKraverHud::OnClientAfterTakeRadialDamageEvent(float DamageAmount, FRadialDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser, FKraverDamageResult const& DamageResult)
{
	CreateDamagedDirection(DamageAmount, DamageEvent, EventInstigator, DamageCauser, DamageResult);
}

void AKraverHud::OnClientGiveDamageSuccessEvent(AActor* DamagedActor, float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser, FKraverDamageResult const& DamageResult)
{
	if(DamageResult.bAlreadyDead)
		return;

	ACreature* Creature = Cast<ACreature>(DamagedActor);
	if(Creature)
		HitmarkAppearanceTime = 0.05f;

	bHitmartCritical = DamageResult.bCritical;
}

void AKraverHud::OnClientGiveDamagePointSuccessEvent(AActor* DamagedActor, float DamageAmount, FPointDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser, FKraverDamageResult const& DamageResult)
{
	CreateFloatingDamage(DamagedActor, DamageAmount, DamageEvent, EventInstigator, DamageCauser, DamageResult);
}

void AKraverHud::OnClientGiveDamageRadialSuccessEvent(AActor* DamagedActor, float DamageAmount, FRadialDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser, FKraverDamageResult const& DamageResult)
{
	CreateFloatingDamage(DamagedActor, DamageAmount, DamageEvent, EventInstigator, DamageCauser, DamageResult);
}

void AKraverHud::OnClientDeathEvent(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser, FKraverDamageResult const& DamageResult)
{
	RespawnTimerWidge->TimerStart();
}

void AKraverHud::OnNewLocalPlayerEvent(AKraverPlayer* NewCreature)
{
	KraverPlayer = NewCreature;
	if (KraverPlayer)
	{
		KraverPlayer->CombatComponent->OnClientGiveAnyDamageSuccess.AddDynamic(this, &AKraverHud::OnClientGiveDamageSuccessEvent);
		KraverPlayer->CombatComponent->OnClientDeath.AddDynamic(this, &AKraverHud::OnClientDeathEvent);
		KraverPlayer->CombatComponent->OnClientGivePointDamageSuccess.AddDynamic(this, &AKraverHud::OnClientGiveDamagePointSuccessEvent);
		KraverPlayer->CombatComponent->OnClientGiveRadialDamageSuccess.AddDynamic(this, &AKraverHud::OnClientGiveDamageRadialSuccessEvent);
		KraverPlayer->CombatComponent->OnClientAfterTakePointDamageSuccess.AddDynamic(this, &AKraverHud::OnClientAfterTakePointDamageEvent);
		KraverPlayer->CombatComponent->OnClientAfterTakeRadialDamageSuccess.AddDynamic(this, &AKraverHud::OnClientAfterTakeRadialDamageEvent);
	}
}

void AKraverHud::OnAnyCreatureDeathEvent(class ACreature* DeadCreature, class AController* VictimController, AActor* AttackerActor, AController* AttackerController, FKraverDamageResult const& DamageResult)
{
	for (int i = KillLogWidgets.Num() - 1; i >= 1; i--)
	{
		UKillLogWidget* TempWidget = KillLogWidgets[i - 1];
		KillLogWidgets[i - 1] = KillLogWidgets[i];
		KillLogWidgets[i] = TempWidget;
	}

	UKillLogWidget* KillLogWidget = Cast<UKillLogWidget>(KillLogWidgets[0]);
	KillLogWidget->Initialize(AttackerController ? AttackerController->GetName() : AttackerActor->GetName(), VictimController ? VictimController->GetName() : DeadCreature->GetName());
}

void AKraverHud::OnGameFinishEvent(ETeam WinTeam)
{
	bGameFinish = true;

	// 게임을 이겼는지 졌는지 확인
	ATeamPlayerState* TeamPlayerState = Cast<ATeamPlayerState>(PlayerState);
	GameFinishWidge->GameFinish(FTeamInfo::CheckIsTeam(WinTeam, TeamPlayerState ? TeamPlayerState->GetPlayerTeam() : ETeam::TEAM_ALL));

	// Start Level Fade
	if (LevelFadeSquence)
	{
		ALevelSequenceActor* OutActor = nullptr;
		ULevelSequencePlayer* SequencePlayer = ULevelSequencePlayer::CreateLevelSequencePlayer(this, LevelFadeSquence, FMovieSceneSequencePlaybackSettings(), OutActor);

		//Sequence Play
		if (SequencePlayer)
		{
			SequencePlayer->PlayReverse();
		}
	}
}

void AKraverHud::OnFloatingDamageDestroyEvent(AActor* Actor)
{
	AFloatingDamage* FloatingDamage = Cast<AFloatingDamage>(Actor);
	if (!FloatingDamage)
		return;

	if (!DuplicationFloatingDamage)
		FloatingWidgets.Remove(FloatingDamage->GetInchargeActor());
}

void AKraverHud::CreateDamagedDirection(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser, FKraverDamageResult const& DamageResult)
{
	if (!bEnableDamagedDirection)
		return;

	if (!DamageCauser)
		return;

	if (DamageResult.ActualDamage <= 0)
		return;

	if (DamageResult.bAlreadyDead)
		return;

	ACreature* Creature;
	AActor* FindActor = DamageCauser;
	while (true)
	{
		Creature = Cast<ACreature>(FindActor);
		if (Creature)
			break;

		if (!FindActor->GetOwner())
			return;

		FindActor = FindActor->GetOwner();
	}
	
	if (!Creature) 
		return;	

	// 해당 Creature에게 할당된 DamagedDirectionWidget이 있을 경우
	if (DamagedDirectionWidgets.Contains(Creature))
	{
		// 해당 위젯 종료
		UDamageDirectionWidget* ExistWidget = DamagedDirectionWidgets[Creature];
		if (ExistWidget)
			ExistWidget->EndWidget();
	}

	// 위젯 생성
	FVector WidgetLocation; // 위젯 스폰 위치 설정
	if (DamageEvent.IsOfType(FPointDamageEvent::ClassID))
	{
		FPointDamageEvent const& PointDamageEvent = static_cast<FPointDamageEvent const&>(DamageEvent);
		WidgetLocation = PointDamageEvent.HitInfo.TraceStart;
	}
	else if(DamageEvent.IsOfType(FRadialDamageEvent::ClassID))
	{
		FRadialDamageEvent const& RadialDamageEvent = static_cast<FRadialDamageEvent const&>(DamageEvent);
		WidgetLocation = RadialDamageEvent.Origin;
	}
	else
	{
		KR_LOG(Warning, TEXT("Unknown Damage Event Type"));
		return;
	}

	UDamageDirectionWidget* Widget = Cast<UDamageDirectionWidget>(CreateWidget<UDamageDirectionWidget>(GetWorld(), DamagedDirectionClass));
	Widget->Initialize(KraverPlayer, WidgetLocation);
	Widget->AddToViewport();
	DamagedDirectionWidgets.Add(Creature, Widget);
}

void AKraverHud::CreateFloatingDamage(AActor* DamagedActor, float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser, FKraverDamageResult const& DamageResult)
{
	if (!bEnableFloatingDamage)
		return;

	if (DamageResult.ActualDamage == 0)
		return;

	if (DamageResult.bAlreadyDead)
		return;

	ACreature* Creature = Cast<ACreature>(DamagedActor);
	if (!Creature)
		return;

	UKraverDamageType* DamageType = DamageEvent.DamageTypeClass->GetDefaultObject<UKraverDamageType>();
	if (DamageType->AttackType == EKraverDamageType::ASSASSINATION)
		return;

	AFloatingDamage* FloatingDamage;

	if (!DuplicationFloatingDamage && FloatingWidgets.Contains(Creature))
		FloatingDamage = FloatingWidgets[Creature];
	else
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = DamageCauser;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		FloatingDamage = GetWorld()->SpawnActor<AFloatingDamage>(SpawnParams);
		FloatingDamage->OnDestroy.AddDynamic(this, &AKraverHud::OnFloatingDamageDestroyEvent);

		if (!DuplicationFloatingDamage)
			FloatingWidgets.Add(Creature, FloatingDamage);
	}

	FloatingDamage->ShowFloatingDamage(DamagedActor, DamageEvent, DamageResult);
}

void AKraverHud::FindGameState()
{
	if (!GameState)
	{
		GameState = GetWorld()->GetGameState<AKraverGameState>();
		if (GameState)
		{
			GameState->OnCreatureDeath.AddDynamic(this, &ThisClass::OnAnyCreatureDeathEvent);
			GameState->OnGameFinish.AddDynamic(this, &ThisClass::OnGameFinishEvent);
		}
		else
			GetWorldTimerManager().SetTimerForNextTick(this, &ThisClass::FindGameState);
	}
}

void AKraverHud::FindPlayerState()
{
	if (!PlayerState)
	{
		PlayerState = GetOwningPlayerController()->GetPlayerState<AKraverPlayerState>();
		if (PlayerState)
		{
			PlayerState->OnNewPlayer.AddDynamic(this, &ThisClass::OnNewLocalPlayerEvent);
		}
		else
			GetWorldTimerManager().SetTimerForNextTick(this, &ThisClass::FindPlayerState);
	}
}

void AKraverHud::SetInteractWidget(bool value)
{
	if (!InteractionWidget)
		return;

	if(value)
	{
		InteractionWidget->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		InteractionWidget->SetVisibility(ESlateVisibility::Hidden);
	}

}
