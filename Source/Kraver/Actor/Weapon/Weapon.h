// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Kraver/Kraver.h"
#include "GameFramework/Actor.h"
#include "Weapon.generated.h"

class UWeaponComponent;

UCLASS()
class KRAVER_API AWeapon : public AActor
{
	GENERATED_BODY()

public:	
	// Sets default values for this actor's properties
	AWeapon();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual bool OnServer_Equipped(ACreature* Character); // Character에게 장착됨 Server에서만 호출됨
	virtual bool OnServer_UnEquipped(); // 장착해제됨
	virtual bool OnLocal_Unholster(); // Character 손에 들려짐
	virtual bool OnLocal_Holster(); // Character 손에서 집어넣어짐

	virtual void OnLocal_AddOnOwnerDelegate();
	virtual void OnLocal_RemoveOnOwnerDelegate();
	virtual void OnServer_AddOnOwnerDelegate();
	virtual void OnServer_RemoveOnOwnerDelegate();

	virtual void AttackCancel(); // 다음 공격 또는 공격 전에 공격을 취소하기 위해 호출

	// Delegate
	UFUNCTION()
	virtual void OnLocalAttackStartEvent(); // 캐릭터의 공격이 시작하였을때 호출되는 함수
	UFUNCTION()
	virtual void OnLocalAttackEndEvent(); // 캐릭터의 공격이 끝났을때 호출되는 함수

	UFUNCTION()
	virtual void OnLocalSubAttackStartEvent(); // 캐릭터의 공격이 시작하였을때 호출되는 함수
	UFUNCTION()
	virtual void OnLocalSubAttackEndEvent(); // 캐릭터의 공격이 끝났을때 호출되는 함수
protected:
	// Rpc
	UFUNCTION(Server, Reliable)
	virtual void Server_Unholster();
	UFUNCTION(Server, Reliable)
	virtual void Server_Holster();

	UFUNCTION(Server, Reliable)
	virtual void Server_OnAttackStartEvent();
	UFUNCTION(Server, Reliable)
	virtual void Server_OnAttackEndEvent(); 

	UFUNCTION(Server, Reliable)
	virtual void Server_OnSubAttackStartEvent(); 
	UFUNCTION(Server, Reliable)
	virtual void Server_OnSubAttackEndEvent();

	UFUNCTION(Server, Reliable)
	virtual void Server_Attack();
	UFUNCTION(NetMulticast, Reliable)
	virtual void Multicast_Attack();

	UFUNCTION(Server, Reliable)
	void Server_TakeImpulseAtLocation(FVector Impulse, FVector ImpactPoint); // Mesh에 Impulse를 주기 위해 사용
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_TakeImpulseAtLocation(FVector Impulse, FVector ImpactPoint);

	// Delegate
	virtual void TryAttack(); // 공격을 시도할 때 호출 AttackCancel을 통해 취소될 수 있음 취소되지 않을 경우 Attack 함수 호출
	virtual void Attack(); // 공격이 취소되지 않았을때 호출
	
	// OnRep
	UFUNCTION()
	virtual void OnRep_WeaponState(EWeaponState PrevWeaponState);

	// Func
	virtual void MakeFppPrimitiveInfo() final; // 1인칭 PrimitiveInfo를 추가

		// Weapon State에 따라 함수 호출
	virtual void EquipEvent();
	virtual void UnEquipEvent();
	virtual void HolsterEvent();
	virtual void UnholsterEvent();

public:
	// Getter Setter
	virtual bool CanAttack();
	virtual bool CanSubAttack();
	virtual bool CanInteracted();

	bool IsAttacking() { return bIsAttacking; }
	bool IsSubAttacking() { return bIsSubAttacking; }
	bool GetbAttackWhileSprint() { return bAttackWhileSprint; }
	bool GetbSubAttackWhileSprint() { return bSubAttackWhileSprint; }

	FName GetFppHandSocketName() { return FppHandSocketName; }
	FName GetTppHandSocketName() { return TppHandSocketName; }
	ACreature* GetOwnerCreature() { return OwnerCreature; }
	EWeaponType GetWeaponType() { return WeaponType; }
	EWeaponState GetWeaponState() { return WeaponState; }
	USkeletalMeshComponent* GetTppWeaponMesh() { return TppWeaponMesh; }
	USkeletalMeshComponent* GetFppWeaponMesh() { return FppWeaponMesh; }
	TMap<FString, UPrimitiveComponent*>& GetTppWeaponPrimitiveInfo() { return WeaponTppPrimitiveInfo; }
	TMap<FString, UPrimitiveComponent*>& GetFppWeaponPrimitiveInfo() { return WeaponFppPrimitiveInfo; }

	UAnimSequence* GetAnimIdleTpp() { return AnimIdleTpp; }
	UAnimSequence* GetAnimIdleFpp() { return AnimIdleFpp; }
	UAnimSequence* GetAnimSprintFpp() { return AnimSprintFpp; }
	UAnimSequence* GetAnimSprintTpp() { return AnimSprintTpp; }
	UAnimSequence* GetSubAttackTpp() { return SubAttackAnimTpp; }
	UAnimSequence* GetSubAttackFpp() { return SubAttackAnimFpp; }
	UBlendSpace* GetAnimMovementTpp() { return AnimMovementTpp; }
	UBlendSpace* GetAnimMovementFpp() { return AnimMovementFpp; }

	virtual UAnimMontage* GetAttackMontageTpp() { return AttackMontageTpp; }
	virtual UAnimMontage* GetAttackMontageFpp() { return AttackMontageFpp; }

	void SetWeaponVisibility(bool Value, bool bDoTpp = true, bool bDoFpp = true);

public:
	FAttackDele OnAttack; // 공격

	FAttackDele OnBeforeAttack;
	FAttackDele OnServerBeforeAttack;

	FAttackStartDele OnAttackStart;
	FAttackStartDele OnSubAttackStart;
	FAttackEndDele OnAttackEnd;
	FAttackEndDele OnSubAttackEnd;

	FSkillDele OnSkillFirst; // R
	FSkillDele OnSkillSecond; // Q
	FSkillDele OnSkillThird; // E

	FPlayMontageDele OnPlayTppMontage;
	FPlayMontageDele OnPlayFppMontage;

	FAddOnDele OnLocalAddOnDelegate;
	FRemoveOnDele OnLocalRemoveOnDelegate;
	FAddOnDele OnServerAddOnDelegate;
	FRemoveOnDele OnServerRemoveOnDelegate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Combat|Component", Meta = (AllowPrivateAccess = true))
	TArray<UWeaponComponent*> WeaponComponents;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Crosshairs")
	class UTexture2D* CrosshairsCenter;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Crosshairs")
	UTexture2D* CrosshairsLeft;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Crosshairs")
	UTexture2D* CrosshairsRight;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Crosshairs")
	UTexture2D* CrosshairsTop;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Crosshairs")
	UTexture2D* CrosshairsBottom;

protected:
	UPROPERTY(Replicated)
	class ACreature* OwnerCreature = nullptr;

	UPROPERTY(ReplicatedUsing = OnRep_WeaponState)
	EWeaponState WeaponState = EWeaponState::IDLE; // 무기 상태
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Data|State", meta = (AllowPrivateAccess = "true"))
	EWeaponType WeaponType = EWeaponType::NONE; // 무기 종류

	// Montage
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Montage", Meta = (AllowPrivateAccess = true))
	UAnimMontage* AttackMontageTpp;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Montage", Meta = (AllowPrivateAccess = true))
	UAnimMontage* AttackMontageFpp;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Montage", Meta = (AllowPrivateAccess = true))
	UAnimMontage* AttackMontageWep;
	
	// Anim
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Animation", Meta = (AllowPrivateAccess = true))
	UAnimSequence* AnimIdleTpp;	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Animation", Meta = (AllowPrivateAccess = true))
	UAnimSequence* AnimIdleFpp;	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Animation", Meta = (AllowPrivateAccess = true))
	UAnimSequence* AnimSprintTpp;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Animation", Meta = (AllowPrivateAccess = true))
	UAnimSequence* AnimSprintFpp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Animation", Meta = (AllowPrivateAccess = true))
	UAnimSequence* SubAttackAnimTpp;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Animation", Meta = (AllowPrivateAccess = true))
	UAnimSequence* SubAttackAnimFpp;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Animation", Meta = (AllowPrivateAccess = true))
	UBlendSpace* AnimMovementTpp;	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Animation", Meta = (AllowPrivateAccess = true))
	UBlendSpace* AnimMovementFpp;	

	// Sound
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Sound", Meta = (AllowPrivateAccess = true))
	class USoundCue* AttackSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Component|Mesh", meta = (AllowPrivateAccess = "true"))
	FName FppHandSocketName = "SOCKET_Weapon"; // Weapon을 Attach할 스켈레탈 본 이름
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Component|Mesh", meta = (AllowPrivateAccess = "true"))
	FName TppHandSocketName = "SOCKET_Weapon"; // Weapon을 Attach할 스켈레탈 본 이름

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Data|Component", meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* TppWeaponMesh;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Data|Component", meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* FppWeaponMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Data|Component", meta = (AllowPrivateAccess = "true"))
	TMap<FString, UPrimitiveComponent*> WeaponTppPrimitiveInfo;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Data|Component", meta = (AllowPrivateAccess = "true"))
	TMap<FString, UPrimitiveComponent*> WeaponFppPrimitiveInfo; // 추가적인 WeaponMesh를 가지는 배열

	UPROPERTY(Replicated)
	bool bIsAttacking = false; // 공격중인지
	UPROPERTY(Replicated)
	bool bIsSubAttacking = false; // 보조 공격중인지

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Combat|Attack", meta = (AllowPrivateAccess = "true"))
	bool bAttackWhileSprint = true; // Sprint 중에 공격이 가능한지
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Combat|Attack", meta = (AllowPrivateAccess = "true"))
	bool bSubAttackWhileSprint = true; // Sprint 중에 보조 공격이 가능한지
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Combat|Attack", meta = (AllowPrivateAccess = "true"))
	bool bCanSubAttack = true; // 보조 공격이 가능한지
	bool bCanAttack = true; // 공격이 가능한지
	bool bAttackCancel = false; // TryAttack 함수에서 Attack함수를 호출할지

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Combat|Attack", meta = (AllowPrivateAccess = "true"))
	float AttackDamage = 10.f; // 공격을 하였을때 주는 데미지	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Combat|Attack", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UKraverDamageType> AttackDamageType;


};
