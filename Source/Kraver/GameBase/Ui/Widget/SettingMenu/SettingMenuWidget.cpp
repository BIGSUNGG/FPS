// Fill out your copyright notice in the Description page of Project Settings.


#include "SettingMenuWidget.h"
#include SettingSubsystem_h

void USettingMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();

	USettingSubsystem* SettingSubsystem = GetGameInstance()->GetSubsystem<USettingSubsystem>();
	if (SettingSubsystem)
		SettingInfo = SettingSubsystem->GetSettingInfo();

	ApplyButton->OnClicked.AddDynamic(this, &ThisClass::OnApplyButtonClickEvent);

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
}

void USettingMenuWidget::OnApplyButtonClickEvent()
{
	SettingInfo.TextureQuality = TextureQualityIndex;
	SettingInfo.ShadowQuality = ShadowQualityIndex;
	SettingInfo.AntiAliasingQuality = AntiAliasingQualityIndex;
	SettingInfo.PostProcessingQuality = PostProcessingQualityIndex;
	SettingInfo.ViewDistanceQuality = ViewDistanceQualityIndex;

	USettingSubsystem* SettingSubsystem = GetGameInstance()->GetSubsystem<USettingSubsystem>();
	if (SettingSubsystem)
	{
		SettingSubsystem->SetSettingInfo(SettingInfo);
		SettingSubsystem->ApplySettings();
	}
}

void USettingMenuWidget::OnTextureQualityNextClickEvent()
{
	TextureQualityIndex = FMath::Clamp(TextureQualityIndex + 1, 0, 4);
	RefreshTextureQuality();
}

void USettingMenuWidget::OnTextureQualityPrevClickEvent()
{
	TextureQualityIndex = FMath::Clamp(TextureQualityIndex - 1, 0, 4);
	RefreshTextureQuality();
}

void USettingMenuWidget::OnShadowQualityNextClickEvent()
{
	ShadowQualityIndex = FMath::Clamp(ShadowQualityIndex + 1, 0, 4);
	RefreshShadowQuality();
}

void USettingMenuWidget::OnShadowQualityPrevClickEvent()
{
	ShadowQualityIndex = FMath::Clamp(ShadowQualityIndex - 1, 0, 4);
	RefreshShadowQuality();
}

void USettingMenuWidget::OnAntiAliasingQualityNextClickEvent()
{
	AntiAliasingQualityIndex = FMath::Clamp(AntiAliasingQualityIndex + 1, 0, 4);
	RefreshAntiAliasingQuality();
}

void USettingMenuWidget::OnAntiAliasingQualityPrevClickEvent()
{
	AntiAliasingQualityIndex = FMath::Clamp(AntiAliasingQualityIndex - 1, 0, 4);
	RefreshAntiAliasingQuality();
}

void USettingMenuWidget::OnPostProcessingQualityNextClickEvent()
{
	PostProcessingQualityIndex = FMath::Clamp(PostProcessingQualityIndex + 1, 0, 4);
	RefreshPostProcessingQuality();
}

void USettingMenuWidget::OnPostProcessingQualityPrevClickEvent()
{	
	PostProcessingQualityIndex = FMath::Clamp(PostProcessingQualityIndex - 1, 0, 4);
	RefreshPostProcessingQuality();
}

void USettingMenuWidget::OnViewDistanceQualityNextClickEvent()
{
	ViewDistanceQualityIndex = FMath::Clamp(ViewDistanceQualityIndex + 1, 0, 4);
	RefreshViewDistanceQuality();
}

void USettingMenuWidget::OnViewDistanceQualityPrevClickEvent()
{
	ViewDistanceQualityIndex = FMath::Clamp(ViewDistanceQualityIndex - 1, 0, 4);
	RefreshViewDistanceQuality();
}

void USettingMenuWidget::RefreshTextureQuality()
{
	FString Str;

	switch (TextureQualityIndex)
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
		Str = FString::FromInt(TextureQualityIndex);
		break;
	}

	TextureQualityText->SetText(FText::FromString(Str));
}

void USettingMenuWidget::RefreshShadowQuality()
{
	FString Str;

	switch (ShadowQualityIndex)
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
		Str = FString::FromInt(ShadowQualityIndex);
		break;
	}

	ShadowQualityText->SetText(FText::FromString(Str));
}

void USettingMenuWidget::RefreshAntiAliasingQuality()
{
	FString Str;

	switch (AntiAliasingQualityIndex)
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
		Str = FString::FromInt(AntiAliasingQualityIndex);
		break;
	}

	AntiAliasingQualityText->SetText(FText::FromString(Str));
}

void USettingMenuWidget::RefreshPostProcessingQuality()
{
	FString Str;

	switch (PostProcessingQualityIndex)
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
		Str = FString::FromInt(PostProcessingQualityIndex);
		break;
	}

	PostProcessingQualityText->SetText(FText::FromString(Str));
}

void USettingMenuWidget::RefreshViewDistanceQuality()
{
	FString Str;

	switch (ViewDistanceQualityIndex)
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
		Str = FString::FromInt(ViewDistanceQualityIndex);
		break;
	}

	ViewDistanceQualityText->SetText(FText::FromString(Str));
}
