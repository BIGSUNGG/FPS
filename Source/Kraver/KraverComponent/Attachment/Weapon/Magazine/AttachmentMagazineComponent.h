// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Kraver/KraverComponent/Attachment/Weapon/AttachmentWeaponComponent.h"
#include "AttachmentMagazineComponent.generated.h"

/**
 * 
 */
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class KRAVER_API UAttachmentMagazineComponent : public UAttachmentWeaponComponent
{
	GENERATED_BODY()
	
public:
	// Sets default values for this component's properties
	UAttachmentMagazineComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Component", meta = (AllowPrivateAccess = "true"))
		TObjectPtr<class UStaticMesh> MagazineStaticMesh;
	UStaticMeshComponent* MagazineMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Component|Attach", meta = (AllowPrivateAccess = "true"))
		FName MagazineSocketName = "SOCKET_Magazine";
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Component|Attach", meta = (AllowPrivateAccess = "true"))
		FVector MagazineRelativeLocation;
};
