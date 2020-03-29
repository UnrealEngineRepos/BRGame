// Fill out your copyright notice in the Description page of Project Settings.


#include "SWeapon.h"
#include "BRGame.h"
#include "Components/SkeletalMeshComponent.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"
#include "Components/SkeletalMeshComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "Animation/AnimationAsset.h"


static int32 DebugWeaponDrawing = 0;

FAutoConsoleVariableRef CVARDebugWeaponDrawing(
	TEXT("BRGAME.DebugWeapons"), 
	DebugWeaponDrawing, 
	TEXT("Draw Debug Lines for Weapons"), 
	ECVF_Cheat);


// Sets default values
ASWeapon::ASWeapon()
{
	MeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComponent"));
	RootComponent = MeshComponent;

	MuzzleSocketName = "MuzzleFlash";
	TracerTargetName = "BeamEnd";
	EjectionPortName = "AmmoEject";

	BaseDamage = 31.f;

	FireRate = 480;
}

void ASWeapon::BeginPlay()
{
	Super::BeginPlay();

	TimeBetweenShots = 60 / FireRate;
}

void ASWeapon::Fire()
{
	AActor* MyOwner = GetOwner();
	
	if (MyOwner != nullptr)
	{
		FVector EyeLocation;
		FRotator EyeRotation;
		MyOwner->GetActorEyesViewPoint(EyeLocation, EyeRotation);

		FVector ShotDirection = EyeRotation.Vector();
		FVector TraceEnd = EyeLocation + (20000 * ShotDirection);

		FCollisionQueryParams QueryParams;

		// Ignores the owner and the weapon
		QueryParams.AddIgnoredActor(MyOwner);
		QueryParams.AddIgnoredActor(this);
		
		// Traces against each individual triangle of the mesh that we're hitting; more precise
		QueryParams.bTraceComplex = true;
		
		QueryParams.bReturnPhysicalMaterial = true;

		// Target parameter for particles
		FVector TracerEndPoint = TraceEnd;

		// Holds all of the data (what was hit, how far it was)
		FHitResult Hit;
		
		if (GetWorld()->LineTraceSingleByChannel(Hit, EyeLocation, TraceEnd, COLLISION_WEAPON, QueryParams) == true)
		{
			// Processes damage
			AActor* HitActor = Hit.GetActor();

			//SurfaceType = UPhysicalMaterial::DetermineSurfaceType(Hit.PhysMaterial.Get());

			//float ActualDamage = BaseDamage;
			//if (SurfaceType == SURFACE_FLESHVULNERABLE)
			//{
			//	ActualDamage *= 4.0f;
			//}

			// TODO delete
			//UE_LOG(LogTemp, Warning, TEXT("-50HP"));
			
			// Gets the surface type
			EPhysicalSurface SurfaceType = UPhysicalMaterial::DetermineSurfaceType(Hit.PhysMaterial.Get());
			
			float ActualDamage = BaseDamage;
			if (SurfaceType == SURFACE_FLESHVULNERABLE)
			{
				ActualDamage *= 4.f;
			}
			
			UGameplayStatics::ApplyPointDamage(HitActor, ActualDamage, ShotDirection, Hit, MyOwner->GetInstigatorController(), MyOwner, DamageType);

			PlayImpactFX(SurfaceType, Hit.ImpactPoint);

			TracerEndPoint = Hit.ImpactPoint;

		}

		if (DebugWeaponDrawing > 0)
		{
			DrawDebugLine(GetWorld(), EyeLocation, TraceEnd, FColor::Yellow, false, 1.0f, 0, 1.0f);
		}
		
		PlayWeaponShotFX(TracerEndPoint);



		LastTimeFired = GetWorld()->TimeSeconds;

		/*PlayFireEffects(TracerEndPoint);

		if (Role == ROLE_Authority)
		{
			HitScanTrace.TraceTo = TracerEndPoint;
			HitScanTrace.SurfaceType = SurfaceType;
		}

		LastFireTime = GetWorld()->TimeSeconds;*/
	}
}

void ASWeapon::StartFire()
{
	// To avoid LMB spamming to shoot faster 
	float FirstDelay = FMath::Max(LastTimeFired + TimeBetweenShots - GetWorld()->TimeSeconds, 0.f);

	GetWorldTimerManager().SetTimer(TimerHandle_TimeBetweenShots, this, &ASWeapon::Fire, TimeBetweenShots, true, FirstDelay);
}

void ASWeapon::StopFire()
{
	GetWorldTimerManager().ClearTimer(TimerHandle_TimeBetweenShots);
}

void ASWeapon::PlayWeaponShotFX(FVector TraceEnd)
{
	MeshComponent->PlayAnimation(WeaponAnimation, false);

	/*if (MuzzleFlashEffect != nullptr)
	{
		UGameplayStatics::SpawnEmitterAttached(MuzzleFlashEffect, MeshComponent, MuzzleSocketName);
	}*/

	if (BulletTracerEffect != nullptr)
	{
		FVector MuzzleLocation = MeshComponent->GetSocketLocation(MuzzleSocketName);
		UParticleSystemComponent* TracerComponent = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), BulletTracerEffect, MuzzleLocation);
		
		if (TracerComponent != nullptr)
		{
			TracerComponent->SetVectorParameter(TracerTargetName, TraceEnd);
		}
	}

	if (ShellEjectionEffect != nullptr)
	{
		UGameplayStatics::SpawnEmitterAttached(ShellEjectionEffect, MeshComponent, EjectionPortName);
	}

	// Camera shake effect while firing
	APawn* MyOwner = Cast<APawn>(GetOwner());
	
	if (MyOwner != nullptr)
	{
		APlayerController* PlayerController = Cast<APlayerController>(MyOwner->GetController());
		
		if (PlayerController != nullptr)
		{
			PlayerController->ClientPlayCameraShake(FireCameraShake);
		}
	}
}

void ASWeapon::PlayImpactFX(EPhysicalSurface SurfaceType, FVector ImpactPoint)
{
	UParticleSystem* SelectedEffect = nullptr;

	switch (SurfaceType)
	{
	case SURFACE_FLESHDEFAULT:
	case SURFACE_FLESHVULNERABLE:
		SelectedEffect = FleshImpactEffect;
		break;
	default:	
		SelectedEffect = DefaultImpactEffect;
		break;
	}

	if (SelectedEffect != nullptr)
	{
		FVector MuzzleLocation = MeshComponent->GetSocketLocation(MuzzleSocketName);
		FVector ShotDirection = ImpactPoint - MuzzleLocation;
		ShotDirection.Normalize();

		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), SelectedEffect, ImpactPoint, ShotDirection.Rotation());
	}
}
