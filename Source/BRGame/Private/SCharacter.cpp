// Fill out your copyright notice in the Description page of Project Settings.


#include "SCharacter.h"
#include "SWeapon.h"
#include "BRGame.h"
#include "SHealthComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/PawnMovementComponent.h"
#include "Engine/World.h"
#include "Components/CapsuleComponent.h"

// Sets default values
ASCharacter::ASCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComponent"));
	SpringArmComponent->bUsePawnControlRotation = true;
	SpringArmComponent->SetupAttachment(RootComponent);

	// Enables support for crouching
	GetMovementComponent()->GetNavAgentPropertiesRef().bCanCrouch = true;

	// Enables support for jumping
	GetMovementComponent()->GetNavAgentPropertiesRef().bCanJump = true;

	// Unblocks the weapon channel in capsule component
	GetCapsuleComponent()->SetCollisionResponseToChannel(COLLISION_WEAPON, ECR_Ignore);

	HealthComponent = CreateDefaultSubobject<USHealthComponent>(TEXT("HealthComponent"));

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	CameraComponent->SetupAttachment(SpringArmComponent);

	AltFireFOV = 55.f;
	AltFireInterpSpeed = 30.f;

	WeaponAttachSocketName = "WeaponSocket";
}

// Called when the game starts or when spawned
void ASCharacter::BeginPlay()
{
	Super::BeginPlay();

	DefaultFOV = CameraComponent->FieldOfView;

	// Adds the event
	HealthComponent->OnHealthChanged.AddDynamic(this, &ASCharacter::OnHealthChanged);

	// Spawns a default weapon
	
	FActorSpawnParameters SpawnParameters;

	// Sets spawn collision
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	// Spawns StarterWeaponClass
	CurrentWeapon = GetWorld()->SpawnActor<ASWeapon>(StarterWeaponClass, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParameters);

	// Attaches the current weapon to the parent component (mesh)
	if (CurrentWeapon != nullptr)
	{
		CurrentWeapon->SetOwner(this);
		CurrentWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, WeaponAttachSocketName);
	}
}

void ASCharacter::MoveForward(float Speed)
{
	AddMovementInput(GetActorForwardVector() * Speed);
}

void ASCharacter::MoveRight(float Speed)
{
	AddMovementInput(GetActorRightVector() * Speed);
}

void ASCharacter::StartCrouching()
{
	Crouch();
}

void ASCharacter::StopCrouching()
{
	UnCrouch();
}

void ASCharacter::BeginZoom()
{
	bWantsToZoom = true;
}

void ASCharacter::EndZoom()
{
	bWantsToZoom = false;
}

void ASCharacter::StartFire()
{
	if (CurrentWeapon != nullptr)
	{
		CurrentWeapon->StartFire();
	}
}

void ASCharacter::StopFire()
{
	if (CurrentWeapon != nullptr)
	{
		CurrentWeapon->StopFire();
	}
}

void ASCharacter::OnHealthChanged(USHealthComponent* OwningHealthComp, float Health, float HealthDelta, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
	if (Health <= 0.f && !bDied)
	{
		bDied = true;

		GetMovementComponent()->StopMovementImmediately();
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		DetachFromControllerPendingDestroy();

		// The pawn will be destroyed in 5 seconds;
		SetLifeSpan(5.0f);
	}
}

// Called every frame
void ASCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	float TargetFOV = bWantsToZoom ? AltFireFOV : DefaultFOV;
	float NewFOV = FMath::FInterpTo(CameraComponent->FieldOfView, TargetFOV, DeltaTime, AltFireInterpSpeed);

	CameraComponent->SetFieldOfView(NewFOV);

}

// Called to bind functionality to input
void ASCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Adds the unput binding to move forward
	PlayerInputComponent->BindAxis("MoveForward", this, &ASCharacter::MoveForward);
	
	// Adds the unput binding to move right
	PlayerInputComponent->BindAxis("MoveRight", this, &ASCharacter::MoveRight);

	// Binds axis to look up
	PlayerInputComponent->BindAxis("LookUp", this, &ASCharacter::AddControllerPitchInput);
	
	// Binds axis to look sideways 
	PlayerInputComponent->BindAxis("LookSideways", this, &ASCharacter::AddControllerYawInput);

	// Binds StartCrouching
	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &ASCharacter::StartCrouching);

	// Binds StopCrouching
	PlayerInputComponent->BindAction("Crouch", IE_Released, this, &ASCharacter::StopCrouching);

	// Binds Jump
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);

	// Binds BeginZoom
	PlayerInputComponent->BindAction("AltFire", IE_Pressed, this, &ASCharacter::BeginZoom);
	
	// Binds EndZoom
	PlayerInputComponent->BindAction("AltFire", IE_Released, this, &ASCharacter::EndZoom);

	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &ASCharacter::StartFire);
	PlayerInputComponent->BindAction("Fire", IE_Released, this, &ASCharacter::StopFire);
}

FVector ASCharacter::GetPawnViewLocation() const
{
	if (CameraComponent != nullptr)
	{
		return CameraComponent->GetComponentLocation();
	}

	return Super::GetPawnViewLocation();
}
