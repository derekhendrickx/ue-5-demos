// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "BuildingBlocksCharacter.generated.h"

UENUM(Meta = (Bitflags, UseEnumValuesAsMaskValuesInEditor = "true"))
enum class EBlockMode : uint8
{
	EBM_Place	UMETA(DisplayName = "Place"),
	EBM_Remove	UMETA(DisplayName = "Remove")
};


UCLASS(config=Game)
class ABuildingBlocksCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;
	
	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputMappingContext* DefaultMappingContext;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* LookAction;

	/** Place Block Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* PlaceBlockAction;

	/** Remove Block Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* RemoveBlockAction;

	/** Block Mode Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* BlockModeAction;

	/** Debug Mode Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* DebugModeAction;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Block, meta = (AllowPrivateAccess = "true", Bitmask, BitmaskEnum = "EBlockMode"))
	EBlockMode BlockMode;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Debug, meta = (AllowPrivateAccess = "true"))
	uint8  DrawDebugLines : 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Block, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<AActor> Block;

public:
	ABuildingBlocksCharacter();

private:
	FHitResult CheckHit();
	

protected:

	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);

	/** Called for placing block input */
	void PlaceBlock(const FInputActionValue& Value);

	/** Called for removing block input */
	void RemoveBlock(const FInputActionValue& Value);

	/** Called for setting block mode input */
	void SetBlockMode(const FInputActionValue& Value);

	/** Called for toggling debug mode input */
	void ToggleDebugMode(const FInputActionValue& Value);
			

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	// To add mapping context
	virtual void BeginPlay();

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	FORCEINLINE EBlockMode GetBlockMode() const { return BlockMode; }

	FORCEINLINE uint8 GetDrawDebugLines() const { return DrawDebugLines; }
};

