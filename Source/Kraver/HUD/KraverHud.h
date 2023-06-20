// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Kraver/Kraver.h"
#include "GameFramework/HUD.h"
#include "Kraver/Widget/Interaction/InteractionWidget.h"
#include "Kraver/Widget/Combat/CombatWidget.h"
#include "KraverHUD.generated.h"

class ACreature;

/**
 * 
 */
UCLASS()
class KRAVER_API AKraverHUD : public AHUD
{
	GENERATED_BODY()
public:
	AKraverHUD();
	virtual void DrawHUD() override;
	virtual void BeginPlay() override;

private:	
	virtual void DrawCrosshair(UTexture2D* Texture, FVector2D ViewportCenter, FVector2D Spread, FLinearColor Color = FLinearColor::White);

protected:
	// Delegate
	UFUNCTION()
		void OnClientGiveDamageSuccessEvent(AActor* DamagedActor, float DamageAmount, FKraverDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser, FKraverDamageResult const& DamageResult);

public:
	// Getter Setter
	FCrosshairsPackage GetHUDPackage() { return HUDPackage; }

	FORCEINLINE void SetCrosshairsPackage(const FCrosshairsPackage& Package) { HUDPackage = Package; } // HUDPackage를 설정하는 함수
	FORCEINLINE void SetbDrawCrosshair(bool value) { bDrawCrosshair = value; }
	void SetInteractWidget(bool value); // InteractionWidget를 렌더링할지 설정하는 함수

private:
	ACreature* OwnerCreature;

	FCrosshairsPackage HUDPackage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Combat", meta = (AllowPrivateAccess = "true"))
		UTexture2D* Hitmark;
	float HitmarkAppearanceTime = 0.f;
	bool bHitmartCritical = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Widget", meta = (AllowPrivateAccess = "true"))
		TSubclassOf<UUserWidget> CombatWidgetClass; // CombatWidget의 클래스 레퍼런스를 가지는 변수
	UUserWidget* CombatWidget; // PlayerCharacter의 상태를 알려주는 위젯

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Widget", meta = (AllowPrivateAccess = "true"))
		TSubclassOf<class UUserWidget> InteractionWidgetClass; // InteractionWidget의 클래스 레퍼런스를 가지는 변수
	UUserWidget* InteractionWidget; // PlayerCharacter가 장착가능한 무기를 찾았을때 렌더링되는 위젯

	bool bDrawCrosshair = true;
	UPROPERTY(EditAnywhere)
		float CrosshairSpreadMax = 16.f;
};