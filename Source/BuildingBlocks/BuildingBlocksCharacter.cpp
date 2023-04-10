// Copyright Epic Games, Inc. All Rights Reserved.

#include "BuildingBlocksCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"


//////////////////////////////////////////////////////////////////////////
// ABuildingBlocksCharacter

ABuildingBlocksCharacter::ABuildingBlocksCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
		
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)
}

void ABuildingBlocksCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	//Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
}

FHitResult ABuildingBlocksCharacter::CheckHit()
{
	const FVector Location;
	FHitResult Hit;

	const FVector CameraLocation = FollowCamera->GetComponentLocation();
	const FVector CameraForwardVector = FollowCamera->GetComponentRotation().Vector();

	const FVector Start = CameraLocation;
	const FVector End = Start + (CameraForwardVector * (GetCameraBoom()->TargetArmLength + 500.f));
	FCollisionQueryParams CollisionQueryParams;
	CollisionQueryParams.bDebugQuery = true;

	if (GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, CollisionQueryParams))
	{

		if (bDrawDebugLines)
		{
			DrawDebugLine(GetWorld(), Start, End, FColor::Green, false, 2.f, ECC_WorldStatic, 1.f);
			DrawDebugBox(GetWorld(), Hit.ImpactPoint, FVector(2.f, 2.f, 2.f), FColor::Green, false, 5.f, ECC_WorldStatic, 1.f);
		}
	}
	else {
		if (bDrawDebugLines)
		{
			DrawDebugLine(GetWorld(), Start, End, FColor::Red, false, 5.f, ECC_WorldStatic, 1.f);
		}
	}

	return Hit;
}

//////////////////////////////////////////////////////////////////////////
// Input

void ABuildingBlocksCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent)) {
		
		//Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		//Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ABuildingBlocksCharacter::Move);

		//Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ABuildingBlocksCharacter::Look);

		//Place Block
		EnhancedInputComponent->BindAction(PlaceBlockAction, ETriggerEvent::Triggered, this, &ABuildingBlocksCharacter::PlaceBlock);

		//Remove Block
		EnhancedInputComponent->BindAction(RemoveBlockAction, ETriggerEvent::Triggered, this, &ABuildingBlocksCharacter::RemoveBlock);

		//Block Mode
		EnhancedInputComponent->BindAction(BlockModeAction, ETriggerEvent::Triggered, this, &ABuildingBlocksCharacter::SetBlockMode);

		//Debug Mode
		EnhancedInputComponent->BindAction(DebugModeAction, ETriggerEvent::Triggered, this, &ABuildingBlocksCharacter::ToggleDebugMode);
	}

}

void ABuildingBlocksCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	
		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void ABuildingBlocksCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void ABuildingBlocksCharacter::PlaceBlock(const FInputActionValue& Value)
{
	if (BlockMode != EBlockMode::EBM_Place)
	{
		return;
	}

	const auto Hit = CheckHit();
	const auto HitActor = Hit.GetActor();
	const auto HitLocation = Hit.Location;
	const auto HitNormal = Hit.Normal;

	const FVector AttachLocation = FVector(HitLocation + HitNormal) - 50.f;
	const FVector AttachLocationSnappedToGrid = AttachLocation.GridSnap(100.f);

	if (bDrawDebugLines)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, FString::Printf(TEXT("Attach Location: Original = %s, With Snap = %s"), *AttachLocation.ToString(), *AttachLocationSnappedToGrid.ToString()));
	}

	const FTransform Transform = FTransform(AttachLocationSnappedToGrid);
	FActorSpawnParameters SpawnParameters;
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::DontSpawnIfColliding;

	GetWorld()->SpawnActor<AActor>(Block, Transform, SpawnParameters);
}

void ABuildingBlocksCharacter::RemoveBlock(const FInputActionValue& Value)
{
	if (BlockMode != EBlockMode::EBM_Remove)
	{
		return;
	}

	const auto Hit = CheckHit();
	const auto HitActor = Hit.GetActor();

	if (!IsValid(HitActor))
	{
		return;
	}

	if (HitActor->IsA(Block))
	{
		GetWorld()->DestroyActor(HitActor);
	}
}

void ABuildingBlocksCharacter::SetBlockMode(const FInputActionValue& Value)
{
	if (BlockMode == EBlockMode::EBM_Place)
	{
		BlockMode = EBlockMode::EBM_Remove;
	}
	else {
		BlockMode = EBlockMode::EBM_Place;
	}
}

void ABuildingBlocksCharacter::ToggleDebugMode(const FInputActionValue& Value)
{
	bDrawDebugLines = !bDrawDebugLines;
}
