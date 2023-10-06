#pragma once

// Actor
#define Bullet_h "Kraver/Actor/Bullet/Bullet.h"
#define Rocket_h "Kraver/Actor/Bullet/Rocket/Rocket.h"

#define FloatingDamage_h "Kraver/Actor/FloatingDamage/FloatingDamage.h"

#define ControlArea_h "Kraver/Actor/ControlArea/ControlArea.h"

#define TeamPlayerStart_h "Kraver/Actor/PlayerStart/TeamPlayerStart.h"

#define Weapon_h		"Kraver/Actor/Weapon/Weapon.h"
#define Melee_h			"Kraver/Actor/Weapon/Melee/Melee.h"
#define Gun_h			"Kraver/Actor/Weapon/Gun/Gun.h"
#define ProjectileGun_h "Kraver/Actor/Weapon/Gun/ProjectileGun/ProjectileGun.h"
#define HitscanGun_h	"Kraver/Actor/Weapon/Gun/HitscanGun/HitscanGun.h"
#define ShotGun_h		"Kraver/Actor/Weapon/Gun/HitscanGun/ShotGun/ShotGun.h"

// Animation
#define CreatureAnimInstance_h			"Kraver/Animation/Creature/CreatureAnimInstance.h"
#define SoldierAnimInstance_h			"Kraver/Animation/Creature/Soldier/SoldierAnimInstance.h"
#define KraverPlayerAnimInstance_h		"Kraver/Animation/Creature/Soldier/KraverPlayer/KraverPlayerAnimInstance.h"
#define KraverPlayerFppAnimInstance_h	"Kraver/Animation/Creature/Soldier/KraverPlayer/Fpp/KraverPlayerFppAnimInstance.h"
#define KraverPlayerTppAnimInstance_h	"Kraver/Animation/Creature/Soldier/KraverPlayer/Tpp/KraverPlayerTppAnimInstance.h"

// Character
#define Creature_h		"Kraver/Character/Creature/Creature.h"
#define Soldier_h		"Kraver/Character/Creature/Soldier/Soldier.h"
#define KraverPlayer_h	"Kraver/Character/Creature/Soldier/KraverPlayer/KraverPlayer.h"

// Component
#define AttachmentComponent_h			"Kraver/Component/Attachment/AttachmentComponent.h"
#define AttachmentWeaponComponent_h		"Kraver/Component/Attachment/Weapon/AttachmentWeaponComponent.h"
#define AttachmentMagazineComponent_h	"Kraver/Component/Attachment/Weapon/Magazine/AttachmentMagazineComponent.h"
#define RocketMagazineComponent_h		"Kraver/Component/Attachment/Weapon/Magazine/RocketMagazine/RocketMagazineComponent.h"
#define AttachmentScopeComponent_h		"Kraver/Component/Attachment/Weapon/Scope/AttachmentScopeComponent.h"
#define AttachmentSilencerComponent_h	"Kraver/Component/Attachment/Weapon/Silencer/AttachmentSilencerComponent.h"

#define CombatComponent_h "Kraver/Component/Combat/CombatComponent.h"

#define CreatureMovementComponent_h "Kraver/Component/Movement/CreatureMovementComponent.h"
#define AdvanceMovementComponent_h	"Kraver/Component/Movement/Advance/AdvanceMovementComponent.h"

#define WeaponComponent_h				"Kraver/Component/Skill/Weapon/WeaponComponent.h"
#define WeaponReloadComponent_h			"Kraver/Component/Skill/Weapon/WeaponReload/WeaponReloadComponent.h"
#define WeaponParryComponent_h			"Kraver/Component/Skill/Weapon/WeaponParrry/WeaponParryComponent.h"
#define WeaponAssassinateComponent_h	"Kraver/Component/Skill/Weapon/WeaponAssassinate/WeaponAssassinateComponent.h"
#define WeaponAdsComponent_h			"Kraver/Component/Skill/Weapon/WeaponAds/WeaponAdsComponent.h"

#define LookCameraWidgetComponent_h "Kraver/Component/Widget/LookCameraWidgetComponent.h"

// GameBase
#define KraverGameMode_h	"Kraver/GameBase/GameMode/KraverGameMode.h"
#define LobbyGameMode_h		"Kraver/GameBase/GameMode/LobbyGameMode/LobbyGameMode.h"
#define TeamGameMode_h		"Kraver/GameBase/GameMode/TeamGameMode/TeamGameMode.h"

#define KraverGameState_h	"Kraver/GameBase/GameState/KraverGameState.h"
#define TeamGameState_h		"Kraver/GameBase/GameState/TeamGameState/TeamGameState.h"
#define ControlGameState_h	"Kraver/GameBase/GameState/TeamGameState/ControlGameState/ControlGameState.h"

#define KraverPlayerController_h "Kraver/GameBase/PlayerController/KraverPlayerController.h"

#define KraverPlayerState_h "Kraver/GameBase/PlayerState/KraverPlayerState.h"
#define TeamPlayerState_h	"Kraver/GameBase/PlayerState/TeamPlayerState/TeamPlayerState.h"

#define KraverSpectator_h "Kraver/GameBase/Spectator/KraverSpectator.h"

#define DamageIndicatorSubsystem_h	"Kraver/GameBase/SubSystem/DamageIndicator/DamageIndicatorSubsystem.h"
#define DataSubsystem_h				"Kraver/GameBase/SubSystem/Data/DataSubsystem.h"

#define KraverHud_h		"Kraver/GameBase/Ui/HUD/KraverHUD.h"
#define ControlHUD_h	"Kraver/GameBase/Ui/HUD/Control/ControlHUD.h"

// Widget
#define CombatWidget_h			"Kraver/GameBase/Ui/Widget/Combat/CombatWidget.h"
#define ControlGameWidget_h		"Kraver/GameBase/Ui/Widget/Control/ControlGameWidget.h"
#define DamageDirectionWidget_h "Kraver/GameBase/Ui/Widget/DamageDirection/DamageDirectionWidget.h"
#define FloatingDamageWidget_h	"Kraver/GameBase/Ui/Widget/FloatingDamage/FloatingDamageWidget.h"
#define InteractionWidget_h		"Kraver/GameBase/Ui/Widget/Interaction/InteractionWidget.h"
#define KillLogWidget_h			"Kraver/GameBase/Ui/Widget/KillLog/KillLogWidget.h"
#define ReturnMainMenuWidget_h	"Kraver/GameBase/Ui/Widget/Menu/ReturnMainMenu/ReturnMainMenuWidget.h"
#define HpBarWidget_h			"Kraver/GameBase/Ui/Widget/HpBar/HpBarWidget.h"
#define RespawnTimerWidget_h	"Kraver/GameBase/Ui/Widget/RespawnTimer/RespawnTimerWidget.h"
#define PauseMenuWidget_h		"Kraver/GameBase/Ui/Widget/PauseMenu/PauseMenuWidget.h"
