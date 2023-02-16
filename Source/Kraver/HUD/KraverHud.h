// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "EngineMinimal.h"
#include "GameFramework/HUD.h"
#include "Kraver/HUD/InteractionWidget.h"
#include "KraverHUD.generated.h"

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
	void DrawCrosshair(UTexture2D* Texture, FVector2D ViewportCenter);

public:
	// Getter Setter
	FORCEINLINE void SetCrosshairsPackage(const FCrosshairsPackage& Package) { HUDPackage = Package; }
	void SetInteractWidget(bool value);
private:
	FCrosshairsPackage HUDPackage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Interaction, meta = (AllowPrivateAccess = "true"))
		TSubclassOf<class UUserWidget> InteractionWidgetClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Interaction, meta = (AllowPrivateAccess = "true"))
		UUserWidget* InteractionWidget;
};
