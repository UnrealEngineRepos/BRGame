// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SCharacter.generated.h"

class UCameraComponent;
class USpringArmComponent;
class ASWeapon;
class USHealthComponent;

UCLASS()
class BRGAME_API ASCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ASCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Axis function to move forward
	void MoveForward(float Speed);

	// Axis function to move right
	void MoveRight(float Speed);

	// Adds binding to crouch
	void StartCrouching();

	// Adds binding to uncrouch
	void StopCrouching();

	// Adds binding to jump
	void Jump1();

	// Adds binding to stop jumping
	void StopJumping1();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USpringArmComponent* SpringArmComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UCameraComponent* CameraComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USHealthComponent* HealthComponent;

	bool bWantsToZoom;

	// FOV set when you start playing
	float DefaultFOV;
	
	// FOV set When you press RMB
	UPROPERTY(EditDefaultsOnly, Category = "Player")
	float AltFireFOV;

	UPROPERTY(EditDefaultsOnly, Category = "Player", meta = (ClampMin = 0.1, ClampMax = 100))
	float AltFireInterpSpeed;

	// Toggles zoom
	void BeginZoom();

	void EndZoom();

	ASWeapon* CurrentWeapon;

	// The class the character spawns with
	UPROPERTY(EditDefaultsOnly, Category = "Player")
	TSubclassOf<ASWeapon> StarterWeaponClass;

	UPROPERTY(VisibleDefaultsOnly, Category = "Player")
	FName WeaponAttachSocketName;

	// Starts firing the current weapon
	void StartFire();

	void StopFire();

	UFUNCTION()
	void OnHealthChanged(USHealthComponent* OwningHealthComponent, float Health, float HealthDelta, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

	// Pawn died previously
	UPROPERTY(BlueprintReadOnly, Category = "Player")
	bool bDied;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// Returns camera component's location instead of pawn's eyes location
	virtual FVector GetPawnViewLocation() const override;
};
