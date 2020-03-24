// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "SFollowingBot.generated.h"

class USHealthComponent;

UCLASS()
class BRGAME_API ASFollowingBot : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ASFollowingBot();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleDefaultsOnly, Category = "Components")
	UStaticMeshComponent* MeshComponent;

	UPROPERTY(VisibleDefaultsOnly, Category = "Components")
	USHealthComponent* HealthComponent;

	UFUNCTION()
	void HandleTakeDamage(USHealthComponent* OwningHealthComponent, float Health, float HealthDelta, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

	FVector GetNextPathPoint();

	// Next point in navigation path
	FVector NextPathPoint;

	UPROPERTY(EditDefaultsOnly, Category = "FollowingBot")
	float MovementForce;

	UPROPERTY(EditDefaultsOnly, Category = "FollowingBot")
	bool bUseVelocityChange;

	UPROPERTY(EditDefaultsOnly, Category = "FollowingBot")
	float RequiredDistanceToTarget;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
