// Fill out your copyright notice in the Description page of Project Settings.


#include "SPickupActor.h"
#include "Components/SphereComponent.h"
#include "Components/DecalComponent.h"
#include "SCollectableItemActor.h"
#include "TimerManager.h"

// Sets default values
ASPickupActor::ASPickupActor()
{
	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	SphereComponent->SetSphereRadius(75.0f);
	RootComponent = SphereComponent;

	DecalComponent = CreateDefaultSubobject<UDecalComponent>(TEXT("DecalComponent"));
	DecalComponent->SetRelativeRotation(FRotator(90, 0.0f, 0.0f));
	DecalComponent->DecalSize = FVector(64, 75, 75);
	DecalComponent->SetupAttachment(RootComponent);

	CooldownDuration = 10.f;
}

// Called when the game starts or when spawned
void ASPickupActor::BeginPlay()
{
	Super::BeginPlay();
	
	Respawn();
}

void ASPickupActor::Respawn()
{
	if (CollectableItemClass == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("CollectableItemClass is nullptr in %s. Please update your Blueprint"), *GetName());
		return;
	}

	FActorSpawnParameters SpawnParameters;
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	CollectableItemInstance = GetWorld()->SpawnActor<ASCollectableItemActor>(CollectableItemClass, GetTransform(), SpawnParameters);
}

void ASPickupActor::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);

	if (CollectableItemInstance != nullptr)
	{
		CollectableItemInstance->ActivateCollectableItem();
		CollectableItemInstance = nullptr;

		// Sets Timer to respawn Collectable Item
		GetWorldTimerManager().SetTimer(TimerHandle_RespawnTimer, this, &ASPickupActor::Respawn, CooldownDuration);
	}
	
}
