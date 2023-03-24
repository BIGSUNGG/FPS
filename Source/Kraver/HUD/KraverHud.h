// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Kraver/Kraver.h"
#include "GameFramework/HUD.h"
#include "Kraver/Widget/InteractionWidget.h"
#include "Kraver/Widget/CombatWidget.h"
#include "KraverHUD.generated.h"

// 크로스헤어의 이미지를 가지는 구조체
USTRUCT(BlueprintType)
struct FCrosshairsPackage
{
	GENERATED_BODY()
public:
	class UTexture2D* CrosshairCenter;
	class UTexture2D* CrosshairLeft;
	class UTexture2D* CrosshairRight;
	class UTexture2D* CrosshairTop;
	class UTexture2D* CrosshairBottom;
};
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
	void DrawCrosshair(UTexture2D* Texture, FVector2D ViewportCenter); // 크로스헤어 이미지 하나를 렌더링

public:
	// Getter Setter
	FORCEINLINE void SetCrosshairsPackage(const FCrosshairsPackage& Package) { HUDPackage = Package; } // HUDPackage를 설정하는 함수
	FORCEINLINE void SetbDrawCrosshair(bool value) { bDrawCrosshair = value; }
	FORCEINLINE void SetInteractWidget(bool value); // InteractionWidget를 렌더링할지 설정하는 함수
private:
	FCrosshairsPackage HUDPackage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widget", meta = (AllowPrivateAccess = "true"))
		TSubclassOf<UUserWidget> CombatWidgetClass; // CombatWidget의 클래스 레퍼런스를 가지는 변수
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Interaction, meta = (AllowPrivateAccess = "true"))
		UUserWidget* CombatWidget; // PlayerCharacter의 상태를 알려주는 위젯

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Interaction, meta = (AllowPrivateAccess = "true"))
		TSubclassOf<class UUserWidget> InteractionWidgetClass; // InteractionWidget의 클래스 레퍼런스를 가지는 변수
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Interaction, meta = (AllowPrivateAccess = "true"))
		UUserWidget* InteractionWidget; // PlayerCharacter가 장착가능한 무기를 찾았을때 렌더링되는 위젯

	bool bDrawCrosshair = true;
};