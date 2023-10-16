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
	void OnDisplayButtonClickEvent();
	UFUNCTION()
	void OnGraphicButtonClickEvent();
	UFUNCTION()
	void OnApplyButtonClickEvent();

		// Display
	UFUNCTION()
	void OnWindowModeNextClickEvent();
	UFUNCTION()
	void OnWindowModePrevClickEvent();

	UFUNCTION()
	void OnFrameLimitNextClickEvent();
	UFUNCTION()
	void OnFrameLimitPrevClickEvent();

		// Graphic
	UFUNCTION()
	void OnTextureQualityNextClickEvent();
	UFUNCTION()
	void OnTextureQualityPrevClickEvent();

	UFUNCTION()
	void OnShadowQualityNextClickEvent();
	UFUNCTION()	   
	void OnShadowQualityPrevClickEvent();

	UFUNCTION()
	void OnAntiAliasingQualityNextClickEvent();
	UFUNCTION()	 
	void OnAntiAliasingQualityPrevClickEvent();

	UFUNCTION()
	void OnPostProcessingQualityNextClickEvent();
	UFUNCTION()	   
	void OnPostProcessingQualityPrevClickEvent();

	UFUNCTION()
	void OnViewDistanceQualityNextClickEvent();
	UFUNCTION()	   
	void OnViewDistanceQualityPrevClickEvent();

	UFUNCTION()
	void OnVSyncCheckBoxStateChangeEvent(bool IsCheck);

	// Func
		// Display
	void RefreshWindowMode();
	void RefreshFrameLimit();

		// Graphic
	void RefreshTextureQuality();
	void RefreshShadowQuality();
	void RefreshAntiAliasingQuality();
	void RefreshPostProcessingQuality();
	void RefreshViewDistanceQuality();
	void RefreshVSync();

	void CheckCanApplySettings();

	void LoadDisplaySettingInfo();
	void LoadGraphicSettingInfo();
public:
	UFUNCTION(BlueprintCallable)
	void MenuSetup();
	UFUNCTION(BlueprintCallable)
	void MenuTearDown();

	void SetCanApplySettings(bool InValue);
	void SetMenuState(ESettingMenuState InValue);

protected:
	ESettingMenuState MenuState = ESettingMenuState::DISPLAY;

	FDisplaySettingInfo DisplaySettingInfo;
	FGraphicSettingInfo GraphicSettingInfo;

	// Top
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UButton* DisplayButton;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UButton* GraphicButton;
	
	// Apply
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UButton* ApplyButton;
	bool bCanApplySettings = false;

	// Display settings
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UCanvasPanel* DisplayPanel;

		// Window Mode
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UButton* WindowModeNextButton;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UButton* WindowModePrevButton;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* WindowModeText;

		// Frame Limit
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UButton* FrameLimitNextButton;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UButton* FrameLimitPrevButton;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* FrameLimitText;

	// Graphic settings
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UCanvasPanel* GraphicsPanel;

		// Texture Quality
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UButton* TextureQualityNextButton;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UButton* TextureQualityPrevButton;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* TextureQualityText;
		// Shadow Quality
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UButton* ShadowQualityNextButton;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UButton* ShadowQualityPrevButton;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* ShadowQualityText;
		// AntiAliasing Quality
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UButton* AntiAliasingQualityNextButton;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UButton* AntiAliasingQualityPrevButton;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* AntiAliasingQualityText;
		// PostProcessing Quality
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UButton* PostProcessingQualityNextButton;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UButton* PostProcessingQualityPrevButton;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* PostProcessingQualityText;
		// ViewDistance Quality
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UButton* ViewDistanceQualityNextButton;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UButton* ViewDistanceQualityPrevButton;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* ViewDistanceQualityText;

		// VSync
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UCheckBox* VSyncCheckbox;

};
