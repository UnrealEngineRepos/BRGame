// Fill out your copyright notice in the Description page of Project Settings.


#include "SCollectableItemActor.h"

// Sets default values
ASCollectableItemActor::ASCollectableItemActor()
{
	CollectableItemInterval = 0.f;
	TicksNum = 0;
	bIsCollectableItemActive = false;
}

// Called when the game starts or when spawned
void ASCollectableItemActor::BeginPlay()
{
	Super::BeginPlay();
	
}

void ASCollectableItemActor::ActivateCollectableItem()
{
	OnActivated();

	if (CollectableItemInterval > 0.f)
	{
		GetWorldTimerManager().SetTimer(TimerHandle_CollectableItemTick, this, &ASCollectableItemActor::OnTickCollectableItem, CollectableItemInterval, true);
	}
	else
	{
		OnTickCollectableItem();
	}
}

void ASCollectableItemActor::OnTickCollectableItem()
{
	TicksProcessed += 1;

	OnCollectableItemTicked();

	if (TicksProcessed >= TicksNum)
	{
		OnExpired();

		bIsCollectableItemActive = false;

		// Deletes timer
		GetWorldTimerManager().ClearTimer(TimerHandle_CollectableItemTick);
	}
}

