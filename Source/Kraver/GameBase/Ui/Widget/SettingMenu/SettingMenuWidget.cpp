// Fill out your copyright notice in the Description page of Project Settings.


#include "SettingMenuWidget.h"
#include SettingSubsystem_h

void USettingMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();

	LoadDisplaySettingInfo();
	LoadGraphicSettingInfo();

	DisplayButton->OnClicked.AddDynamic(this, &ThisClass::OnDisplayButtonClickEvent);
	GraphicButton->OnClicked.AddDynamic(this, &ThisClass::OnGraphicButtonClickEvent);

	// Display
		// Win Mode
	RefreshWindowMode();
	WindowModeNextButton->OnClicked.AddDynamic(this, &ThisClass::OnWindowModeNextClickEvent);
	WindowModePrevButton->OnClicked.AddDynamic(this, &ThisClass::OnWindowModePrevClickEvent);

		// Frame Limit
	RefreshFrameLimit();
	FrameLimitNextButton->OnClicked.AddDynamic(this, &ThisClass::OnFrameLimitNextClickEvent);
	FrameLimitPrevButton->OnClicked.AddDynamic(this, &ThisClass::OnFrameLimitPrevClickEvent);

	// Graphic
		// Texture
	RefreshTextureQuality();
	TextureQualityNextButton->OnClicked.AddDynamic(this, &ThisClass::OnTextureQualityNextClickEvent);
	TextureQualityPrevButton->OnClicked.AddDynamic(this, &ThisClass::OnTextureQualityPrevClickEvent);

		// Shadow
	RefreshShadowQuality();
	ShadowQualityNextButton->OnClicked.AddDynamic(this, &ThisClass::OnShadowQualityNextClickEvent);
	ShadowQualityPrevButton->OnClicked.AddDynamic(this, &ThisClass::OnShadowQualityPrevClickEvent);

		// Shadow
	RefreshAntiAliasingQuality();
	AntiAliasingQualityNextButton->OnClicked.AddDynamic(this, &ThisClass::OnAntiAliasingQualityNextClickEvent);
	AntiAliasingQualityPrevButton->OnClicked.AddDynamic(this, &ThisClass::OnAntiAliasingQualityPrevClickEvent);

		// Shadow
	RefreshPostProcessingQuality();
	PostProcessingQualityNextButton->OnClicked.AddDynamic(this, &ThisClass::OnPostProcessingQualityNextClickEvent);
	PostProcessingQualityPrevButton->OnClicked.AddDynamic(this, &ThisClass::OnPostProcessingQualityPrevClickEvent);

		// Shadow
	RefreshViewDistanceQuality();
	ViewDistanceQualityNextButton->OnClicked.AddDynamic(this, &ThisClass::OnViewDistanceQualityNextClickEvent);
	ViewDistanceQualityPrevButton->OnClicked.AddDynamic(this, &ThisClass::OnViewDistanceQualityPrevClickEvent);

		// VSync
	VSyncCheckbox->SetCheckedState(GraphicSettingInfo.bEnableVSync ? ECheckBoxState::Checked : ECheckBoxState::Unchecked);
	RefreshVSync();
	VSyncCheckbox->OnCheckStateChanged.AddDynamic(this, &ThisClass::OnVSyncCheckBoxStateChangeEvent);

	CheckCanApplySettings();

}

void USettingMenuWidget::OnDisplayButtonClickEvent()
{
	SetMenuState(ESettingMenuState::DISPLAY);
}

void USettingMenuWidget::OnGraphicButtonClickEvent()
{
	SetMenuState(ESettingMenuState::GRAPHIC);
}

void USettingMenuWidget::OnApplyButtonClickEvent()
{
	USettingSubsystem* SettingSubsystem = GetGameInstance()->GetSubsystem<USettingSubsystem>();
	if (SettingSubsystem)
	{
		SettingSubsystem->SetDisplaySettingInfo(DisplaySettingInfo);
		SettingSubsystem->SetGraphicSettingInfo(GraphicSettingInfo);
		SettingSubsystem->ApplySettings();
		SetCanApplySettings(false);
	}
}

void USettingMenuWidget::OnWindowModeNextClickEvent()
{
	switch (DisplaySettingInfo.WinMode)
	{
	case EWindowMode::Windowed:
		DisplaySettingInfo.WinMode = EWindowMode::WindowedFullscreen;
		break;
	case EWindowMode::WindowedFullscreen:
		DisplaySettingInfo.WinMode = EWindowMode::Fullscreen;
		break;
	case EWindowMode::Fullscreen:
		break;
	case EWindowMode::NumWindowModes:
		break;
	default:
		break;
	}	

	RefreshWindowMode();
}

void USettingMenuWidget::OnWindowModePrevClickEvent()
{
	switch (DisplaySettingInfo.WinMode)
	{
	case EWindowMode::Windowed:
		break;
	case EWindowMode::WindowedFullscreen:
		DisplaySettingInfo.WinMode = EWindowMode::Windowed;
		break;
	case EWindowMode::Fullscreen:
		DisplaySettingInfo.WinMode = EWindowMode::WindowedFullscreen;
		break;
	case EWindowMode::NumWindowModes:
		break;
	default:
		break;
	}

	RefreshWindowMode();
}

void USettingMenuWidget::OnFrameLimitNextClickEvent()
{
	switch ((int)DisplaySettingInfo.MaxFrame)
	{
	case 15:
		DisplaySettingInfo.MaxFrame = 30;
		break;
	case 30:
		DisplaySettingInfo.MaxFrame = 60;
		break;
	case 60:
		DisplaySettingInfo.MaxFrame = 120;
		break;
	case 120:
		DisplaySettingInfo.MaxFrame = 240;
		break;
	case 240:
		DisplaySettingInfo.MaxFrame = 0;
		break;
	case 0:
		break;
	}

	RefreshFrameLimit();
}

void USettingMenuWidget::OnFrameLimitPrevClickEvent()
{
	switch ((int)DisplaySettingInfo.MaxFrame)
	{
	case 15:
		break;
	case 30:
		DisplaySettingInfo.MaxFrame = 15;
		break;
	case 60:
		DisplaySettingInfo.MaxFrame = 30;
		break;
	case 120:
		DisplaySettingInfo.MaxFrame = 60;
		break;
	case 240:
		DisplaySettingInfo.MaxFrame = 120;
		break;
	case 0:
		DisplaySettingInfo.MaxFrame = 240;
		break;
	}
	RefreshFrameLimit();
}

void USettingMenuWidget::OnTextureQualityNextClickEvent()
{
	GraphicSettingInfo.TextureQuality = FMath::Clamp(GraphicSettingInfo.TextureQuality + 1, 0, 4);
	RefreshTextureQuality();
}

void USettingMenuWidget::OnTextureQualityPrevClickEvent()
{
	GraphicSettingInfo.TextureQuality = FMath::Clamp(GraphicSettingInfo.TextureQuality - 1, 0, 4);
	RefreshTextureQuality();
}

void USettingMenuWidget::OnShadowQualityNextClickEvent()
{
	GraphicSettingInfo.ShadowQuality = FMath::Clamp(GraphicSettingInfo.ShadowQuality + 1, 0, 4);
	RefreshShadowQuality();
}

void USettingMenuWidget::OnShadowQualityPrevClickEvent()
{
	GraphicSettingInfo.ShadowQuality = FMath::Clamp(GraphicSettingInfo.ShadowQuality - 1, 0, 4);
	RefreshShadowQuality();
}

void USettingMenuWidget::OnAntiAliasingQualityNextClickEvent()
{
	GraphicSettingInfo.AntiAliasingQuality = FMath::Clamp(GraphicSettingInfo.AntiAliasingQuality + 1, 0, 4);
	RefreshAntiAliasingQuality();
}

void USettingMenuWidget::OnAntiAliasingQualityPrevClickEvent()
{
	GraphicSettingInfo.AntiAliasingQuality = FMath::Clamp(GraphicSettingInfo.AntiAliasingQuality - 1, 0, 4);
	RefreshAntiAliasingQuality();
}

void USettingMenuWidget::OnPostProcessingQualityNextClickEvent()
{
	GraphicSettingInfo.PostProcessingQuality = FMath::Clamp(GraphicSettingInfo.PostProcessingQuality + 1, 0, 4);
	RefreshPostProcessingQuality();
}

void USettingMenuWidget::OnPostProcessingQualityPrevClickEvent()
{	
	GraphicSettingInfo.PostProcessingQuality = FMath::Clamp(GraphicSettingInfo.PostProcessingQuality - 1, 0, 4);
	RefreshPostProcessingQuality();
}

void USettingMenuWidget::OnViewDistanceQualityNextClickEvent()
{
	GraphicSettingInfo.ViewDistanceQuality = FMath::Clamp(GraphicSettingInfo.ViewDistanceQuality + 1, 0, 4);
	RefreshViewDistanceQuality();
}

void USettingMenuWidget::OnViewDistanceQualityPrevClickEvent()
{
	GraphicSettingInfo.ViewDistanceQuality = FMath::Clamp(GraphicSettingInfo.ViewDistanceQuality - 1, 0, 4);
	RefreshViewDistanceQuality();
}

void USettingMenuWidget::OnVSyncCheckBoxStateChangeEvent(bool IsCheck)
{
	GraphicSettingInfo.bEnableVSync = IsCheck;
	RefreshVSync();
}

void USettingMenuWidget::RefreshWindowMode()
{
	FString Str;
	switch (DisplaySettingInfo.WinMode)
	{
	case EWindowMode::Fullscreen:
		Str = "Fullscreen";
		break;
	case EWindowMode::WindowedFullscreen:
		Str = "Borderless";
		break;
	case EWindowMode::Windowed:
		Str = "Windowed";
		break;
	case EWindowMode::NumWindowModes:
		break;
	default:
		break;
	}

	WindowModeText->SetText(FText::FromString(Str));
	CheckCanApplySettings();
}

void USettingMenuWidget::RefreshFrameLimit()
{
	FString Str = DisplaySettingInfo.MaxFrame == 0 ? "Unlimit" : FString::FromInt(DisplaySettingInfo.MaxFrame);
	FrameLimitText->SetText(FText::FromString(Str));
	CheckCanApplySettings();
}

void USettingMenuWidget::RefreshTextureQuality()
{
	FString Str;

	switch (GraphicSettingInfo.TextureQuality)
	{
	case 0:
		Str = "Very Low";
		break;
	case 1:
		Str = "Low";
		break;
	case 2:
		Str = "Medium";
		break;
	case 3:
		Str = "High";
		break;
	case 4:
		Str = "Very High";
		break;
	default:
		Str = FString::FromInt(GraphicSettingInfo.TextureQuality);
		break;
	}

	TextureQualityText->SetText(FText::FromString(Str));
	CheckCanApplySettings();
}

void USettingMenuWidget::RefreshShadowQuality()
{
	FString Str;

	switch (GraphicSettingInfo.ShadowQuality)
	{
	case 0:
		Str = "Very Low";
		break;
	case 1:
		Str = "Low";
		break;
	case 2:
		Str = "Medium";
		break;
	case 3:
		Str = "High";
		break;
	case 4:
		Str = "Very High";
		break;
	default:
		Str = FString::FromInt(GraphicSettingInfo.ShadowQuality);
		break;
	}

	ShadowQualityText->SetText(FText::FromString(Str));
	CheckCanApplySettings();
}

void USettingMenuWidget::RefreshAntiAliasingQuality()
{
	FString Str;

	switch (GraphicSettingInfo.AntiAliasingQuality)
	{
	case 0:
		Str = "Very Low";
		break;
	case 1:
		Str = "Low";
		break;
	case 2:
		Str = "Medium";
		break;
	case 3:
		Str = "High";
		break;
	case 4:
		Str = "Very High";
		break;
	default:
		Str = FString::FromInt(GraphicSettingInfo.AntiAliasingQuality);
		break;
	}

	AntiAliasingQualityText->SetText(FText::FromString(Str));
	CheckCanApplySettings();
}

void USettingMenuWidget::RefreshPostProcessingQuality()
{
	FString Str;

	switch (GraphicSettingInfo.PostProcessingQuality)
	{
	case 0:
		Str = "Very Low";
		break;
	case 1:
		Str = "Low";
		break;
	case 2:
		Str = "Medium";
		break;
	case 3:
		Str = "High";
		break;
	case 4:
		Str = "Very High";
		break;
	default:
		Str = FString::FromInt(GraphicSettingInfo.PostProcessingQuality);
		break;
	}

	PostProcessingQualityText->SetText(FText::FromString(Str));
	CheckCanApplySettings();
}

void USettingMenuWidget::RefreshViewDistanceQuality()
{
	FString Str;

	switch (GraphicSettingInfo.ViewDistanceQuality)
	{
	case 0:
		Str = "Very Low";
		break;
	case 1:
		Str = "Low";
		break;
	case 2:
		Str = "Medium";
		break;
	case 3:
		Str = "High";
		break;
	case 4:
		Str = "Very High";
		break;
	default:
		Str = FString::FromInt(GraphicSettingInfo.ViewDistanceQuality);
		break;
	}

	ViewDistanceQualityText->SetText(FText::FromString(Str));
	CheckCanApplySettings();
}

void USettingMenuWidget::RefreshVSync()
{
	CheckCanApplySettings();
}

void USettingMenuWidget::CheckCanApplySettings()
{
	USettingSubsystem* SettingSubsystem = GetGameInstance()->GetSubsystem<USettingSubsystem>();
	if (!SettingSubsystem)
		return;

	if (GraphicSettingInfo == SettingSubsystem->GetGraphicSettingInfo() && 
		DisplaySettingInfo == SettingSubsystem->GetDisplaySettingInfo())
		SetCanApplySettings(false);
	else
		SetCanApplySettings(true);

}

void USettingMenuWidget::LoadDisplaySettingInfo()
{
	USettingSubsystem* SettingSubsystem = GetGameInstance()->GetSubsystem<USettingSubsystem>();
	if (SettingSubsystem)
		DisplaySettingInfo = SettingSubsystem->GetDisplaySettingInfo();
}

void USettingMenuWidget::LoadGraphicSettingInfo()
{
	USettingSubsystem* SettingSubsystem = GetGameInstance()->GetSubsystem<USettingSubsystem>();
	if (SettingSubsystem)
		GraphicSettingInfo = SettingSubsystem->GetGraphicSettingInfo();
}

void USettingMenuWidget::MenuSetup()
{
	SetVisibility(ESlateVisibility::Visible);
	SetMenuState(ESettingMenuState::DISPLAY);

	LoadDisplaySettingInfo();
	LoadGraphicSettingInfo();

}

void USettingMenuWidget::MenuTearDown()
{
	SetVisibility(ESlateVisibility::Hidden);

}

void USettingMenuWidget::SetCanApplySettings(bool InValue)
{
	bCanApplySettings = InValue;
	ApplyButton->SetIsEnabled(InValue);

	if (bCanApplySettings)
		ApplyButton->OnClicked.AddDynamic(this, &ThisClass::OnApplyButtonClickEvent);
	else
		ApplyButton->OnClicked.RemoveDynamic(this, &ThisClass::OnApplyButtonClickEvent);

}

void USettingMenuWidget::SetMenuState(ESettingMenuState InValue)
{
	MenuState = InValue;

	switch (MenuState)
	{
	case ESettingMenuState::DISPLAY:
		DisplayButton->SetIsEnabled(false);
		DisplayPanel->SetVisibility(ESlateVisibility::Visible);

		GraphicButton->SetIsEnabled(true);
		GraphicsPanel->SetVisibility(ESlateVisibility::Hidden);
		break;
	case ESettingMenuState::GRAPHIC:
		DisplayButton->SetIsEnabled(true);
		DisplayPanel->SetVisibility(ESlateVisibility::Hidden);

		GraphicButton->SetIsEnabled(false);
		GraphicsPanel->SetVisibility(ESlateVisibility::Visible);
		break;
	default:
		break;
	}
}
