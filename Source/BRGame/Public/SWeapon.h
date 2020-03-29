// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SWeapon.generated.h"

class USkeletalMeshComponent;
class UDamageType;
class UParticleSystem;
class UAnimationAsset;

USTRUCT()
struct FWeaponData
{
	GENERATED_USTRUCT_BODY()

	///** inifite ammo for reloads */
	//UPROPERTY(EditDefaultsOnly, Category = Ammo)
	//bool bInfiniteAmmo;

	///** infinite ammo in clip, no reload required */
	//UPROPERTY(EditDefaultsOnly, Category = Ammo)
	//bool bInfiniteClip;

	/** Clip size */
	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	int32 ClipSize;

	/** Max ammo */
	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	int32 MaxAmmo;

	/** Initial clips */
	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	int32 InitialClips;

	/** Time between two consecutive shots */
	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	float TimeBetweenShots;

	///** Failsafe reload duration if weapon doesn't have any animation for it */
	//UPROPERTY(EditDefaultsOnly, Category = WeaponStat)
	//float NoAnimReloadDuration;

	/** defaults */
	FWeaponData()
	{
		/*bInfiniteAmmo = false;
		bInfiniteClip = false;*/
		ClipSize = 30;
		MaxAmmo = 999;
		InitialClips = 2;
		TimeBetweenShots = 0.2f;
		//NoAnimReloadDuration = 1.0f;
	}
};

UCLASS()
class BRGAME_API ASWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASWeapon();

protected:

	virtual void BeginPlay() override;

	void PlayWeaponShotFX(FVector TraceEnd);

	void PlayImpactFX(EPhysicalSurface SurfaceType, FVector ImpactPoint);

	// Represents the visual element of all weapons
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USkeletalMeshComponent* MeshComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	TSubclassOf<UDamageType> DamageType;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	FName MuzzleSocketName;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	FName TracerTargetName;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	FName EjectionPortName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	UAnimationAsset* WeaponAnimation;

	/*UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	UParticleSystem* MuzzleFlashEffect;*/

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	UParticleSystem* DefaultImpactEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	UParticleSystem* FleshImpactEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	UParticleSystem* BulletTracerEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	UParticleSystem* ShellEjectionEffect;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	TSubclassOf<UCameraShake> FireCameraShake;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	float BaseDamage;
	
	// Traces the world from pawn eyes to crosshair location
	virtual void Fire();

	FTimerHandle TimerHandle_TimeBetweenShots;

	float LastTimeFired;

	// Bullets per minute fired by weapon
	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	float FireRate;

	// Derived from FireRate 
	float TimeBetweenShots;

public:
	
	void StartFire();

	void StopFire();
};
