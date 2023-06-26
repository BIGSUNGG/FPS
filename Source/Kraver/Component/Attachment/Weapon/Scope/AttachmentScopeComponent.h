// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Kraver/Component/Attachment/Weapon/AttachmentWeaponComponent.h"
#include "AttachmentScopeComponent.generated.h"

/**
 * 
 */
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class KRAVER_API UAttachmentScopeComponent : public UAttachmentWeaponComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UAttachmentScopeComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Component", meta = (AllowPrivateAccess = "true"))
		TObjectPtr<class UStaticMesh> ScopeStaticMesh;
	UStaticMeshComponent* ScopeMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Component|Attach", meta = (AllowPrivateAccess = "true"))
		FName ScopeSocketName = "SOCKET_Scope";
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Component|Attach", meta = (AllowPrivateAccess = "true"))
		FVector ScopeRelativeLocation;
};
