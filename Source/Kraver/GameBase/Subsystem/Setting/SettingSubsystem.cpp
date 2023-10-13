// Fill out your copyright notice in the Description page of Project Settings.


#include "SettingSubsystem.h"

void USettingSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	UGameUserSettings* GameUserSettings = GEngine->GetGameUserSettings();
	if (GameUserSettings) 
	{
		SettingInfo.WinMode				= GameUserSettings->GetFullscreenMode();
		SettingInfo.DisplayResolution	= GameUserSettings->GetScreenResolution();
		SettingInfo.MaxFrame			= GameUserSettings->GetFrameRateLimit();
		SettingInfo.TextureQuality		= GameUserSettings->GetTextureQuality();
		SettingInfo.ShadowQuality		= GameUserSettings->GetShadowQuality();
		SettingInfo.AntiAliasingQuality		= GameUserSettings->GetAntiAliasingQuality();
		SettingInfo.PostProcessingQuality		= GameUserSettings->GetPostProcessingQuality();
		SettingInfo.ViewDistanceQuality		= GameUserSettings->GetViewDistanceQuality();
	}
}

void USettingSubsystem::Deinitialize()
{
	Super::Deinitialize();

}

void USettingSubsystem::ApplySettings()
{
	UGameUserSettings* GameUserSettings = GEngine->GetGameUserSettings();
	if (!GameUserSettings) return;

	GameUserSettings->SetFullscreenMode(SettingInfo.WinMode);
	GameUserSettings->SetScreenResolution(SettingInfo.DisplayResolution);
	GameUserSettings->SetFrameRateLimit(SettingInfo.MaxFrame);
	GameUserSettings->SetTextureQuality(static_cast<int>(SettingInfo.TextureQuality));
	GameUserSettings->SetShadowQuality(static_cast<int>(SettingInfo.ShadowQuality));
	GameUserSettings->SetAntiAliasingQuality(static_cast<int>(SettingInfo.AntiAliasingQuality));
	GameUserSettings->SetPostProcessingQuality(static_cast<int>(SettingInfo.PostProcessingQuality));
	GameUserSettings->SetViewDistanceQuality(static_cast<int>(SettingInfo.ViewDistanceQuality));

	GameUserSettings->ApplySettings(false);
	GameUserSettings->SaveSettings();
	KR_LOG(Log, TEXT("Apply Settings"));

}
