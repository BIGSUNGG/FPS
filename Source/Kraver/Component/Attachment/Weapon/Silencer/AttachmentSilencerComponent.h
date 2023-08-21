// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Kraver/Component/Attachment/Weapon/AttachmentWeaponComponent.h"
#include "AttachmentSilencerComponent.generated.h"

/**
 * 
 */
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class KRAVER_API UAttachmentSilencerComponent : public UAttachmentWeaponComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UAttachmentSilencerComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	// Getter Setter
	const FName& GetSilencerSocketName() { return SilencerSocketName; }
	bool GetbFireEffect() { return bFireEffect; }
	class USoundCue* GetSilencerFireSound() { return SilencerFireSound; }

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Component", meta = (AllowPrivateAccess = "true"))
		TObjectPtr<class UStaticMesh> SilencerStaticMesh;
	UStaticMeshComponent* SilencerMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Component|Attach", meta = (AllowPrivateAccess = "true"))
		FName SilencerSocketName = "SOCKET_Muzzle";
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Component|Attach", meta = (AllowPrivateAccess = "true"))
		FVector SilencerRelativeLocation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack", meta = (AllowPrivateAccess = "true"))
		bool bFireEffect = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack", meta = (AllowPrivateAccess = "true"))
		class USoundCue* SilencerFireSound;
};
