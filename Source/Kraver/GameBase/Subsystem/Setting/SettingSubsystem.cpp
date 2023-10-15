// Fill out your copyright notice in the Description page of Project Settings.


#include "SettingSubsystem.h"

void USettingSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	CurSettings();
}

void USettingSubsystem::Deinitialize()
{
	Super::Deinitialize();

}

void USettingSubsystem::ApplySettings()
{
	UGameUserSettings* GameUserSettings = GEngine->GetGameUserSettings();
	if (!GameUserSettings) return;

	// Display
	GameUserSettings->SetFullscreenMode(DisplaySettingInfo.WinMode);
	GameUserSettings->SetFrameRateLimit(DisplaySettingInfo.MaxFrame);

	// Graphic
	GameUserSettings->SetTextureQuality(GraphicSettingInfo.TextureQuality);
	GameUserSettings->SetShadowQuality(GraphicSettingInfo.ShadowQuality);
	GameUserSettings->SetAntiAliasingQuality(GraphicSettingInfo.AntiAliasingQuality);
	GameUserSettings->SetPostProcessingQuality(GraphicSettingInfo.PostProcessingQuality);
	GameUserSettings->SetViewDistanceQuality(GraphicSettingInfo.ViewDistanceQuality);
	GameUserSettings->SetVSyncEnabled(GraphicSettingInfo.bEnableVSync);

	GameUserSettings->ApplySettings(false);
	GameUserSettings->SaveSettings();
	KR_LOG(Log, TEXT("Apply Settings"));

}

void USettingSubsystem::CurSettings()
{
	UGameUserSettings* GameUserSettings = GEngine->GetGameUserSettings();
	if (GameUserSettings)
	{
		DisplaySettingInfo.WinMode = GameUserSettings->GetFullscreenMode();
		DisplaySettingInfo.MaxFrame = GameUserSettings->GetFrameRateLimit();

		GraphicSettingInfo.TextureQuality = GameUserSettings->GetTextureQuality();
		GraphicSettingInfo.ShadowQuality = GameUserSettings->GetShadowQuality();
		GraphicSettingInfo.AntiAliasingQuality = GameUserSettings->GetAntiAliasingQuality();
		GraphicSettingInfo.PostProcessingQuality = GameUserSettings->GetPostProcessingQuality();
		GraphicSettingInfo.ViewDistanceQuality = GameUserSettings->GetViewDistanceQuality();
		GraphicSettingInfo.bEnableVSync = GameUserSettings->IsVSyncEnabled();
	}
}
