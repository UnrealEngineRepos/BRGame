// Fill out your copyright notice in the Description page of Project Settings.

#include "SExplosiveBarrel.h"
#include "SHealthComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "PhysicsEngine/RadialForceComponent.h"


// Sets default values
ASExplosiveBarrel::ASExplosiveBarrel()
{
	HealthComponent = CreateDefaultSubobject<USHealthComponent>(TEXT("HealthComponent"));
	HealthComponent->OnHealthChanged.AddDynamic(this, &ASExplosiveBarrel::OnHealthChanged);

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	MeshComponent->SetSimulatePhysics(true);
	
	// Set to physics body to let radial component affect us (eg. when a nearby barrel explodes)
	MeshComponent->SetCollisionObjectType(ECC_PhysicsBody);
	RootComponent = MeshComponent;

	RadialForceComponent = CreateDefaultSubobject<URadialForceComponent>(TEXT("RadialForceComponent"));
	RadialForceComponent->SetupAttachment(MeshComponent);
	RadialForceComponent->Radius = 400;
	RadialForceComponent->bImpulseVelChange = true;

	// Triggers only on FireImpulse(), not on tick
	RadialForceComponent->bAutoActivate = false;

	// Ignores itself
	RadialForceComponent->bIgnoreOwningActor = true;

	ExplosionImpulse = 1200;

	SetReplicates(true);
	SetReplicateMovement(true);
}


void ASExplosiveBarrel::OnHealthChanged(USHealthComponent* OwningHealthComponent, float Health, float HealthDelta, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
{
	if (bExploded == true)
	{
		// Nothing left to do, already exploded.
		return;
	}

	if (Health <= 0.f)
	{
		// Explode!
		bExploded = true;

		// Boosts the barrel upwards
		FVector BoostIntensity = FVector::UpVector * ExplosionImpulse;
		MeshComponent->AddImpulse(BoostIntensity, NAME_None, true);

		// Spawns particle effect and changes barrel's material to black
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplosionEffect, GetActorLocation());

		// Overrides material on mesh with blackened version
		MeshComponent->SetMaterial(0, ExplodedMaterial);

		// Blasts away nearby physics actors
		RadialForceComponent->FireImpulse();
	}
}