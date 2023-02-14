// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "EngineMinimal.h"
#include "GameFramework/HUD.h"
#include "KraverHUD.generated.h"

USTRUCT(BlueprintType)
struct FHUDPackage
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
	virtual void DrawHUD() override;

private:
	void DrawCrosshair(UTexture2D* Texture, FVector2D ViewportCenter);

public:
	// Getter Setter
	FORCEINLINE void SetHUDPackage(const FHUDPackage& Package) { HUDPackage = Package; }
private:
	FHUDPackage HUDPackage;
	
};
