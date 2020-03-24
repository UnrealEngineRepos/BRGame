// Fill out your copyright notice in the Description page of Project Settings.


#include "SFollowingBot.h"
#include "SHealthComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NavigationSystem.h"
#include "NavigationPath.h"
#include "DrawDebugHelpers.h"

// Sets default values
ASFollowingBot::ASFollowingBot()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	
	MeshComponent->SetCanEverAffectNavigation(false);
	MeshComponent->SetSimulatePhysics(true);

	RootComponent = MeshComponent;

	HealthComponent = CreateDefaultSubobject<USHealthComponent>(TEXT("HealthComponent"));
	
	// Subscribes to OnHealthChanged
	HealthComponent->OnHealthChanged.AddDynamic(this, &ASFollowingBot::HandleTakeDamage);

	bUseVelocityChange = false;
	MovementForce = 1000;
	RequiredDistanceToTarget = 100;
}

// Called when the game starts or when spawned
void ASFollowingBot::BeginPlay()
{
	Super::BeginPlay();

	// Find initial move to
	NextPathPoint = GetNextPathPoint();
	
}

void ASFollowingBot::HandleTakeDamage(USHealthComponent* OwningHealthComponent, float Health, float HealthDelta, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
	// Explode on hitpoints == 0

	// Pulses the material on hit

	UE_LOG(LogTemp, Log, TEXT("%s HP: %s"), *GetName(), *FString::SanitizeFloat(Health))
}

FVector ASFollowingBot::GetNextPathPoint()
{
	AActor* BestTarget = nullptr;
	float NearestTargetDistance = FLT_MAX;

	for (FConstPawnIterator It = GetWorld()->GetPawnIterator(); It; It++)
	{
		APawn* TestPawn = It->Get();

		/*if (TestPawn == nullptr || USHealthComponent::IsFriendly(TestPawn, this))
		{
			continue;
		}*/

		USHealthComponent* TestPawnHealthComp = Cast<USHealthComponent>(TestPawn->GetComponentByClass(USHealthComponent::StaticClass()));
		//if (TestPawnHealthComp && TestPawnHealthComp->GetHealth() > 0.0f)
		if (TestPawnHealthComp)
		{
			float Distance = (TestPawn->GetActorLocation() - GetActorLocation()).Size();

			if (Distance < NearestTargetDistance)
			{
				BestTarget = TestPawn;
				NearestTargetDistance = Distance;
			}
		}
	}

	if (BestTarget)
	{
		UNavigationPath* NavigationPath = UNavigationSystemV1::FindPathToActorSynchronously(this, GetActorLocation(), BestTarget);

		/*GetWorldTimerManager().ClearTimer(TimerHandle_RefreshPath);
		GetWorldTimerManager().SetTimer(TimerHandle_RefreshPath, this, &ASTrackerBot::RefreshPath, 5.0f, false);*/

		if (NavigationPath && NavigationPath->PathPoints.Num() > 1)
		{
			// Return next point in the path
			return NavigationPath->PathPoints[1];
		}
	}

	// Failed to find path
	return GetActorLocation();
}

// Called every frame
void ASFollowingBot::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	float DistanceToTarget = (GetActorLocation() - NextPathPoint).Size();

	if (DistanceToTarget <= RequiredDistanceToTarget)
	{
		NextPathPoint = GetNextPathPoint();

		//DrawDebugString(GetWorld(), GetActorLocation(), "Vlad's mom is coming!");
	}
	else
	{
		// Keep moving towards next target
		FVector ForceDirection = NextPathPoint - GetActorLocation();
		ForceDirection.Normalize();

		ForceDirection *= MovementForce;

		MeshComponent->AddForce(ForceDirection, NAME_None, bUseVelocityChange);

		DrawDebugDirectionalArrow(GetWorld(), GetActorLocation(), GetActorLocation() + ForceDirection, 16, FColor::Orange, false, 0.f, 0, 1.f);
	}

	DrawDebugSphere(GetWorld(), NextPathPoint, 12, 12, FColor::Orange, false, 0.f, 1.f);
}

