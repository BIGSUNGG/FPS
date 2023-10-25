// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Kraver/Kraver.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "SettingSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class KRAVER_API USettingSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

public:
	void ApplySettings();
	void LoadSettings();

public:
	// Getter Setter
	FORCEINLINE const FDisplaySettingInfo& GetDisplaySettingInfo() { return DisplaySettingInfo ; }
	FORCEINLINE const FGraphicSettingInfo& GetGraphicSettingInfo() { return GraphicSettingInfo; }

	FORCEINLINE void SetDisplaySettingInfo(const FDisplaySettingInfo& InInfo) { DisplaySettingInfo = InInfo; }
	FORCEINLINE void SetGraphicSettingInfo(const FGraphicSettingInfo& InInfo) { GraphicSettingInfo = InInfo; }

protected:
	FDisplaySettingInfo DisplaySettingInfo;
	FGraphicSettingInfo GraphicSettingInfo;

};
