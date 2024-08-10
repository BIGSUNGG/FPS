// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Kraver/Kraver.h"
#include "GameFramework/PlayerController.h"
#include "KraverPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class KRAVER_API AKraverPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	virtual void OnPossess(APawn* aPawn) override;
	virtual void SetupInputComponent() override;

	// 메인 메뉴 레벨로 이동
	UFUNCTION(BlueprintCallable)
	virtual void ReturnToMainMenu();

	// 게임 종료 시 호출
	UFUNCTION(BlueprintCallable)
	virtual void ExitGame();

protected:
	// Rpc
	UFUNCTION(Server, Reliable)
	virtual void Server_SetDefaultWeapons(TSubclassOf<class AWeapon> InValue, int Index);

	// Delegate
	 
	// ReturnToMainMenu이후 세션 종료 시 호출
	UFUNCTION()
	void OnDestroySessionEvent_ReturnToMainMenu(bool bWasSuccessful);
	UFUNCTION()
	// ExitGame이후 세션 종료 시 호출
	void OnDestroySessionEvent_ExitGame(bool bWasSuccessful);

	// Func
	void ShowPauseMenu();

public:
	virtual void SetDefaultWeapons(const TSubclassOf<class AWeapon>& InValue, int Index);

protected:
	// Return
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Widget", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class UPauseMenuWidget> PauseMenuWidgetClass;
	UPauseMenuWidget* PauseMenuWidget;

};
