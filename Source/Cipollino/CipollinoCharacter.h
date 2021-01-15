// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "CipollinoCharacter.generated.h"


UENUM(BlueprintType)
enum class EMovementStatus: uint8
{
    EMS_Normal    UMETA(DisplayName = "Normal"),
    EMS_Sprinting UMETA(DisplayName = "Sprinting"),
    EMS_Death     UMETA(DisplayName = "Death"),
    EMS_Max       UMETA(DisplayName = "DefaultMax")
};

UENUM(BlueprintType)
enum class EStaminaStatus: uint8
{
    ESS_Normal              UMETA(DisplayName = "Normal"),
    ESS_BelowMinimum        UMETA(DisplayName = "BelowMinimum"),
    ESS_Exhausted           UMETA(DisplayName = "Exhausted"),
    ESS_ExhaustedRecovering UMETA(DisplayName = "ExhaustedRecovering"),
    EMS_Max                 UMETA(DisplayName = "DefaultMax")
};

UCLASS()
class CIPOLLINO_API ACipollinoCharacter : public ACharacter
{
    GENERATED_BODY()

protected:
    // Called when the game starts or when spawned
    void BeginPlay() override;

public:
    // Sets default values for this character's properties
    ACipollinoCharacter();

    TArray<FVector> PickupLocations;

    UFUNCTION(BlueprintCallable)
    void ShowPickupLocations();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
    class UParticleSystem* HitParticles;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
    class USoundCue* HitSound;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Enums")
    EMovementStatus MovementStatus;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Enums")
    EStaminaStatus StaminaStatus;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
    float StaminaDrainRate;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
    float MinSprintStamina;

    float InterpSpeed;

    bool InterpToEnemy;

    FORCEINLINE void SetInterpToEnemy(bool Interp) noexcept { InterpToEnemy = Interp; }

    const auto GetLookAtRotationYaw(FVector Target) const;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
    class AEnemy* CombatTarget;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
    bool HasCombatTarget;

    FORCEINLINE void SetHasCombatTarget(bool HasTarget) noexcept { HasCombatTarget = HasTarget; }

    FORCEINLINE void SetCombatTarget(AEnemy* Target) noexcept { CombatTarget = Target; }

    UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Combat")
    FVector CombatTargetLocation;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Controller")
    class AMainPlayerController* MainPlayerController;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Running")
    float RunningSpeed;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Running")
    float SprintingSpeed;

    bool bShiftKeyDown;

    /** Pressed down to enable sprinting */
    void ShiftKeyDown() noexcept;

    /** Released to stop sprinting */
    void ShiftKeyUp() noexcept;

    /** Set movement status and running */
    void SetMovementStatus(EMovementStatus Status) noexcept;

    FORCEINLINE void SetStaminaStatus(EStaminaStatus status) noexcept { StaminaStatus = status; }

    /** Camera boom positioning the camera behind the player */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess = "true"), Category = Camera)
    class USpringArmComponent* CameraBoom;

    /** Camera follow */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess = "true"), Category = Camera)
    class UCameraComponent* FollowCamera;

    /** Base turn rates to scale turning functions for the camera */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
    float BaseTurnUpRate;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
    float BaseLookUpRate;

    /**
     *
     * Player Stats
     *
     */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "PlayerStats")
    float MaxHealth;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlayerStats")
    float Health;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "PlayerStats")
    float MaxStamina;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlayerStats")
    float Stamina;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlayerStats")
    int32 Coins;

    void IncrementCoins(int32 Amount) noexcept;

    void DecrementHealth(float Amount) noexcept;

    AActor* EnemyDamageCauser;

    float TakeDamage(float DamageAmount,
                     const struct FDamageEvent& DamageEvent,
                     class AController* EventInstigator,
                     AActor* DamageCauser) override;

    void Die() noexcept;

    void Jump() override;

    UFUNCTION(BlueprintCallable)
    void DeathEnd();

    // Called every frame
    void Tick(float DeltaTime) override;

    // Called to bind functionality to input
    void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

    // Called for forward/backwards input
    void MoveForwardBackward(float Value);

    bool MovingForwardBackward;

    // Called for side to side input
    void MoveRightLeft(float Value);

    bool MovingRightLeft;

    /** Called via input to turn at a given rate
     * @param Rate This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
     */
    void TurnAtRate(float Rate);

    /** Called via input to look up/down at a given rate
     * @param Rate This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
     */
    void LookUpAtRate(float Rate);

    bool LeftMouseButton;

    void LeftMouseButtonDown();
    void LeftMouseButtonUp();

    FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
    FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Items)
    class AItem* EquippedWeapon;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Items)
    class AItem* ActiveOverlappingItem;

    void SetEquippedWeapon(AItem* WeaponToSet) noexcept;
    FORCEINLINE AItem* GetEquippedWeapon() const noexcept { return EquippedWeapon; }
    FORCEINLINE void SetActiveOverlappingItem(AItem* ItemToSet) noexcept { ActiveOverlappingItem = ItemToSet; }

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Anims")
    bool Attacking;

    void AttackStart() noexcept;

    UFUNCTION(BlueprintCallable)
    void AttackEnd() noexcept;

    UPROPERTY(EditDefaultsOnly /* Edit in Actor is default */, BlueprintReadOnly, Category = "Anims")
    class UAnimMontage* CombatMontage;

    UFUNCTION(BlueprintCallable)
    void PlaySwingSound();
};
