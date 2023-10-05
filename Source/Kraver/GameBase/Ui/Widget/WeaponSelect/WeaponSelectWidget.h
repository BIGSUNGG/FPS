// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#pragma warning( disable : 4996 ) 

#include "Kraver/Kraver.h"
#include "Blueprint/UserWidget.h"
#include "WeaponSelectWidget.generated.h"

/**
 * 
 */
UCLASS()
class KRAVER_API UWeaponSelectWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

public:
	UFUNCTION(BlueprintCallable)
	virtual void SelectWeapon(TSubclassOf<AWeapon> WeaponClass, FString WeaponName, float Size, int Index);

protected:
	static TTuple<FString, float> MainWeaponTextInfo;
	static TTuple<FString, float> SubWeaponTextInfo;
	static TTuple<FString, float> SpecialWeaponTextInfo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* MainWeaponText;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* SubWeaponText;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* SpecialWeaponText;

};
