// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Kraver/Kraver.h"
#include "Blueprint/UserWidget.h"
#include "SettingMenuWidget.generated.h"

/**
 * 
 */
UCLASS()
class KRAVER_API USettingMenuWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;

protected:
	// Delegate
	UFUNCTION()
	virtual void OnApplyButtonClickEvent();

	UFUNCTION()
	virtual void OnTextureQualityNextClickEvent();
	UFUNCTION()
	virtual void OnTextureQualityPrevClickEvent();

	UFUNCTION()
	virtual void OnShadowQualityNextClickEvent();
	UFUNCTION()	   
	virtual void OnShadowQualityPrevClickEvent();

	UFUNCTION()
	virtual void OnAntiAliasingQualityNextClickEvent();
	UFUNCTION()	 
	virtual void OnAntiAliasingQualityPrevClickEvent();

	UFUNCTION()
	virtual void OnPostProcessingQualityNextClickEvent();
	UFUNCTION()	   
	virtual void OnPostProcessingQualityPrevClickEvent();

	UFUNCTION()
	virtual void OnViewDistanceQualityNextClickEvent();
	UFUNCTION()	   
	virtual void OnViewDistanceQualityPrevClickEvent();

	// Func
	virtual void RefreshTextureQuality();
	virtual void RefreshShadowQuality();
	virtual void RefreshAntiAliasingQuality();
	virtual void RefreshPostProcessingQuality();
	virtual void RefreshViewDistanceQuality();

protected:
	FSettingInfo SettingInfo;

	// Widget
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UButton* ApplyButton;

	// Texture Quality
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UButton* TextureQualityNextButton;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UButton* TextureQualityPrevButton;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* TextureQualityText;
	int16 TextureQualityIndex;

	// Shadow Quality
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UButton* ShadowQualityNextButton;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UButton* ShadowQualityPrevButton;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* ShadowQualityText;
	int16 ShadowQualityIndex;

	// AntiAliasing Quality
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UButton* AntiAliasingQualityNextButton;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UButton* AntiAliasingQualityPrevButton;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* AntiAliasingQualityText;
	int16 AntiAliasingQualityIndex;

	// PostProcessing Quality
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UButton* PostProcessingQualityNextButton;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UButton* PostProcessingQualityPrevButton;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* PostProcessingQualityText;
	int16 PostProcessingQualityIndex;

	// ViewDistance Quality
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UButton* ViewDistanceQualityNextButton;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UButton* ViewDistanceQualityPrevButton;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* ViewDistanceQualityText;
	int16 ViewDistanceQualityIndex;
};
