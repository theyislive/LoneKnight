// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Enemy.generated.h"


UENUM(BlueprintType)
enum class EEnemyMovementStatus: uint8
{
    EMS_Idle         UMETA(DisplayName = "Idle"),
    EMS_MoveToTarget UMETA(DisplayName = "MoveToTarget"),
    EMS_Attacking    UMETA(DisplayName = "Attacking"),
    EMS_Dead         UMETA(DisplayName = "Dead"),
    EMS_Max          UMETA(DisplayName = "DefaultMax")
};


UCLASS()
class CIPOLLINO_API AEnemy : public ACharacter
{
    GENERATED_BODY()

protected:
    // Called when the game starts or when spawned
    void BeginPlay() override;

public:
    // Sets default values for this character's properties
    AEnemy() noexcept;

    bool HasValidTarget;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
    EEnemyMovementStatus EnemyMovementStatus;

    FORCEINLINE void SetEnemyMovementStatus(EEnemyMovementStatus Status) noexcept { EnemyMovementStatus = Status; }

    FORCEINLINE auto GetEnemyMovementStatus() noexcept { return EnemyMovementStatus; }

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
    class USphereComponent* AgroSphere;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
    USphereComponent* CombatSphere;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
    class AAIController* AIController;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
    float Health;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
    float MaxHealth;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
    float Damage;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
    class UParticleSystem* HitParticles;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
    class USoundCue* HitSound;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
    USoundCue* SwingSound;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
    class UBoxComponent* CombatCollision;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Combat")
    class UAnimMontage* CombatMontage;

    float InterpSpeed;

    bool InterpToTarget;

    FORCEINLINE void SetInterpToTarget(bool Interp) noexcept { InterpToTarget = Interp; }

    const auto GetLookAtRotationYaw(FVector Target) const;

    // Called every frame
    void Tick(float DeltaTime) override;

    // Called to bind functionality to input
    void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

    UFUNCTION()
    virtual void AgroSphereOnOverlapBegin(UPrimitiveComponent* OverlappedComponent,
                                          AActor* OtherActor,
                                          UPrimitiveComponent* OtherComp,
                                          int32 OtherBodyIndex,
                                          bool bFromSweep,
                                          const FHitResult& SweepResult);
    UFUNCTION()
    virtual void AgroSphereOnOverlapEnd(UPrimitiveComponent* OverlappedComponent,
                                        AActor* OtherActor,
                                        UPrimitiveComponent* OtherComp,
                                        int32 OtherBodyIndex);

    UFUNCTION()
    virtual void CombatSphereOnOverlapBegin(UPrimitiveComponent* OverlappedComponent,
                                            AActor* OtherActor,
                                            UPrimitiveComponent* OtherComp,
                                            int32 OtherBodyIndex,
                                            bool bFromSweep,
                                            const FHitResult& SweepResult);
    UFUNCTION()
    virtual void CombatSphereOnOverlapEnd(UPrimitiveComponent* OverlappedComponent,
                                          AActor* OtherActor,
                                          UPrimitiveComponent* OtherComp,
                                          int32 OtherBodyIndex);

    UFUNCTION(BlueprintCallable)
    void MoveToTarget(class ACipollinoCharacter* Target);

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AI")
    bool OverlappingCombatSphere;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AI")
    ACipollinoCharacter* CombatTarget;

    FTimerHandle AttackTimer;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
    float AttackMinTime;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
    float AttackMaxTime;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
    TSubclassOf<UDamageType> DamageTypeClass;

    FTimerHandle DeathTimer;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
    float DeathDelay;

    UFUNCTION()
    void CombatOnOverlapBegin(UPrimitiveComponent* OverlappedComponent,
                              AActor* OtherActor,
                              UPrimitiveComponent* OtherComp,
                              int32 OtherBodyIndex,
                              bool bFromSweep,
                              const FHitResult& SweepResult);
    UFUNCTION()
    void CombatOnOverlapEnd(UPrimitiveComponent* OverlappedComponent,
                            AActor* OtherActor,
                            UPrimitiveComponent* OtherComp,
                            int32 OtherBodyIndex);

    UFUNCTION(BlueprintCallable)
    void ActivateCollision();

    UFUNCTION(BlueprintCallable)
    void DeactivateCollision();

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
    bool Attacking;

    void AttackStart() noexcept;

    UFUNCTION(BlueprintCallable)
    void AttackEnd() noexcept;

    void DecrementHealth(float Amount) noexcept;

    void Die() noexcept;

    UFUNCTION(BlueprintCallable)
    void DeathEnd();

    bool Alive() noexcept;

    void Disappear() noexcept;

    float TakeDamage(float DamageAmount,
                     const struct FDamageEvent& DamageEvent,
                     class AController* EventInstigator,
                     AActor* DamageCauser) override;
};
