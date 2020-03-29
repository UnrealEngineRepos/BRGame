// Fill out your copyright notice in the Description page of Project Settings.


#include "SHealthComponent.h"

// Sets default values for this component's properties
USHealthComponent::USHealthComponent()
{
	DefaultHealth = 100;
}


// Called when the game starts
void USHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	// Hooks the component to the owning actor
	AActor* MyOwner = GetOwner();
	
	if (MyOwner != nullptr)
	{
		// Subscribes us to the component
		MyOwner->OnTakeAnyDamage.AddDynamic(this, &USHealthComponent::HandleTakeAnyDamage);
	}

	Health = DefaultHealth;
}

void USHealthComponent::HandleTakeAnyDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
	if (Damage <= 0.f || bIsDead)
	{
		UE_LOG(LogTemp, Log, TEXT("Damage <= 0.f || bIsDead"));
		return;
	}

	/*if (DamageCauser != DamagedActor && IsFriendly(DamagedActor, DamageCauser))*/
	/*if (DamageCauser != DamagedActor)
	{
		UE_LOG(LogTemp, Log, TEXT("DamageCauser != DamagedActor"));
		return;
	}*/

	// Updates health clamped
	Health = FMath::Clamp(Health - Damage, 0.f, DefaultHealth);

	UE_LOG(LogTemp, Log, TEXT("HP Changed: %s"), *FString::SanitizeFloat(Health));

	bIsDead = Health <= 0.0f;

	OnHealthChanged.Broadcast(this, Health, Damage, DamageType, InstigatedBy, DamageCauser);

	if (bIsDead)
	{
		/*ASGameMode* GameMode = Cast<ASGameMode>(GetWorld()->GetAuthGameMode());
		if (GameMode)
		{
			GameMode->OnActorKilled.Broadcast(GetOwner(), DamageCauser, InstigatedBy);
		}*/
	}
}

void USHealthComponent::Heal(float HealAmount)
{
	if (HealAmount <= 0.f || Health <= 0.f)
	{
		return;
	}

	Health = FMath::Clamp(Health + HealAmount, 0.f, DefaultHealth);

	UE_LOG(LogTemp, Log, TEXT("HP Changed: %s (+%s)"), *FString::SanitizeFloat(Health), *FString::SanitizeFloat(HealAmount));

	OnHealthChanged.Broadcast(this, Health, -HealAmount, nullptr, nullptr, nullptr);
}
