// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SCollectableItemActor.generated.h"

UCLASS()
class BRGAME_API ASCollectableItemActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASCollectableItemActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Time between powerup ticks 
	UPROPERTY(EditDefaultsOnly, Category = "CollectableItems")
	float CollectableItemInterval;

	// Total times the effect is applied
	UPROPERTY(EditDefaultsOnly, Category = "CollectableItems")
	int32 TicksNum;

	// Keeps state of the power-up
	bool bIsCollectableItemActive;

	FTimerHandle TimerHandle_CollectableItemTick;

	// Total number of ticks applied
	int32 TicksProcessed;

	UFUNCTION()
	void OnTickCollectableItem();

public:
	void ActivateCollectableItem();

	UFUNCTION(BlueprintImplementableEvent, Category = "CollectableItems")
	void OnActivated();

	UFUNCTION(BlueprintImplementableEvent, Category = "CollectableItems")
	void OnCollectableItemTicked();

	UFUNCTION(BlueprintImplementableEvent, Category = "CollectableItems")
	void OnExpired();

};
