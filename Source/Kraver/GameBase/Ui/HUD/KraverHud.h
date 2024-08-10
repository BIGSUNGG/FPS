// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Kraver/Kraver.h"
#include "GameFramework/HUD.h"
#include "Kraver/GameBase/Ui/Widget/Interaction/InteractionWidget.h"
#include "Kraver/GameBase/Ui/Widget/Combat/CombatWidget.h"
#include "KraverHUD.generated.h"

/**
 * 
 */
UCLASS()
class KRAVER_API AKraverHud : public AHUD
{
	GENERATED_BODY()
public:
	AKraverHud();
	virtual void BeginPlay() override;
	virtual void DrawHUD() override;

private:	
	// 조준점 그리기
	// Texture : 그릴 텍스쳐
	// ViewportCenter : 화면 중앙 위치
	// Spread : 현재 탄퍼짐 정도
	// Color : 조준점 색
	virtual void DrawCrosshair(UTexture2D* Texture, FVector2D ViewportCenter, FVector2D Spread, FLinearColor Color = FLinearColor::White);

protected:
	// Delegate			
		// Creature
			// Give Damage
	UFUNCTION()
	void OnClientGiveDamageSuccessEvent(AActor* DamagedActor, float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser, FKraverDamageResult const& DamageResult);
	UFUNCTION()
	void OnClientGiveDamagePointSuccessEvent(AActor* DamagedActor, float DamageAmount, FPointDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser, FKraverDamageResult const& DamageResult);
	UFUNCTION()
	void OnClientGiveDamageRadialSuccessEvent(AActor* DamagedActor, float DamageAmount, FRadialDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser, FKraverDamageResult const& DamageResult);
			// Take Damage
	UFUNCTION()
	void OnClientAfterTakePointDamageEvent(float DamageAmount, FPointDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser, FKraverDamageResult const& DamageResult);
	UFUNCTION()
	void OnClientAfterTakeRadialDamageEvent(float DamageAmount, FRadialDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser, FKraverDamageResult const& DamageResult);	
			// Death
	UFUNCTION()
	void OnClientDeathEvent(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser, FKraverDamageResult const& DamageResult);

		// Player State
	UFUNCTION()
	void OnNewLocalPlayerEvent(AKraverPlayer* NewCreature);

		// Game State
	UFUNCTION()
	void OnAnyCreatureDeathEvent(class ACreature* DeadCreature, class AController* VictimController, AActor* AttackerActor, AController* AttackerController, FKraverDamageResult const& DamageResult);
	UFUNCTION()
	void OnGameFinishEvent(ETeam WinTeam);

		// Destroy
	UFUNCTION()
	void OnFloatingDamageDestroyEvent(AActor* Actor);

	// Func
	void CreateDamagedDirection(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser, FKraverDamageResult const& DamageResult);
	void CreateFloatingDamage(AActor* DamagedActor, float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser, FKraverDamageResult const& DamageResult);

	void FindGameState();
	void FindPlayerState();

public:
	// Getter Setter
	FORCEINLINE FCrosshairsPackage GetHUDPackage() { return HUDPackage; }

	FORCEINLINE void SetCrosshairsPackage(const FCrosshairsPackage& Package) { HUDPackage = Package; } // HUDPackage를 설정하는 함수
	FORCEINLINE void SetbDrawCrosshair(bool value) { bDrawCrosshair = value; }
	void SetInteractWidget(bool value); // InteractionWidget를 렌더링할지 설정하는 함수

private:
	class AKraverPlayer* KraverPlayer;
	class AKraverGameState* GameState;
	class AKraverPlayerState* PlayerState;

	FCrosshairsPackage HUDPackage;

	// GameFinish
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|GameFinish", meta = (AllowPrivateAccess = "true"))
	ULevelSequence* LevelFadeSquence;
	bool bGameFinish = false;

	// Hitmark
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Combat", meta = (AllowPrivateAccess = "true"))
	UTexture2D* Hitmark;
	float HitmarkAppearanceTime = 0.f;
	bool bHitmartCritical = false;

	// Crosshairs
	bool bDrawCrosshair = true;
	float CrosshairSpreadMax = 16.f;

	// Floating Damage
	bool bEnableFloatingDamage = true;
	TMap<class ACreature*, class AFloatingDamage*> FloatingWidgets;
	bool DuplicationFloatingDamage = false;

	// Damaged Direction
	bool bEnableDamagedDirection = true;
	TMap<class ACreature*, class UDamageDirectionWidget*> DamagedDirectionWidgets;
	class TSubclassOf<class UDamageDirectionWidget> DamagedDirectionClass;

	// Widget
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Widget", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class UCombatWidget> CombatWidgetClass; // CombatWidget의 클래스 레퍼런스를 가지는 변수
	class UCombatWidget* CombatWidget; // PlayerCharacter의 상태를 알려주는 위젯

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Widget", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class UInteractionWidget> InteractionWidgetClass; // InteractionWidget의 클래스 레퍼런스를 가지는 변수
	class UInteractionWidget* InteractionWidget; // PlayerCharacter가 장착가능한 무기를 찾았을때 렌더링되는 위젯

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Widget", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class UKillLogWidget> KillLogWidgetClass; // InteractionWidget의 클래스 레퍼런스를 가지는 변수
	TArray<class UKillLogWidget*> KillLogWidgets; // PlayerCharacter가 장착가능한 무기를 찾았을때 렌더링되는 위젯

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Widget", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class URespawnTimerWidget> RespawnTimerWidgeClass; // InteractionWidget의 클래스 레퍼런스를 가지는 변수
	class URespawnTimerWidget* RespawnTimerWidge; // PlayerCharacter가 장착가능한 무기를 찾았을때 렌더링되는 위젯

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Widget", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class UGameFinishWidget> GameFinishWidgeClass; // InteractionWidget의 클래스 레퍼런스를 가지는 변수
	class UGameFinishWidget* GameFinishWidge; // PlayerCharacter가 장착가능한 무기를 찾았을때 렌더링되는 위젯
};