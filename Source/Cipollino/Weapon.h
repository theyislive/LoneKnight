// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item.h"
#include "Weapon.generated.h"


UENUM(BlueprintType)
enum class EWeaponState: uint8
{
    EWS_Pickup   UMETA(DisplayName = "Pickup"),
    EWS_Equipped UMETA(DisplayName = "Equipped"),
    EWS_Max      UMETA(DisplayName = "DefaultMax")
};

/**
 * 
 */
UCLASS()
class CIPOLLINO_API AWeapon : public AItem
{
    GENERATED_BODY()

public:
    AWeapon() noexcept;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Item")
    EWeaponState WeaponState;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item | Partcles")
    bool WeaponParticles;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item | Sounds")
    class USoundCue* OnEquipSound;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item | Sounds")
    USoundCue* SwingSound;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "SkeletalMesh")
    class USkeletalMeshComponent* SkeletalMesh;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item | Combat")
    class UBoxComponent* CombatCollision;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item | Combat")
    float Damage;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
    TSubclassOf<UDamageType> DamageTypeClass;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    class AController* WeaponInstigator;

    FORCEINLINE void SetWeaponInstigator(AController* Inst) noexcept { WeaponInstigator = Inst; }

    void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent,
                                AActor* OtherActor,
                                UPrimitiveComponent* OtherComp,
                                int32 OtherBodyIndex,
                                bool bFromSweep,
                                const FHitResult& SweepResult) override;
    void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent,
                              AActor* OtherActor,
                              UPrimitiveComponent* OtherComp,
                              int32 OtherBodyIndex) override;

    void Equip(class ACipollinoCharacter* Character) noexcept;

    FORCEINLINE void SetWeaponState(EWeaponState State) noexcept { WeaponState = State; }
    FORCEINLINE EWeaponState GetWeaponState() noexcept { return WeaponState; }

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

protected:
    // Called when the game starts or when spawned
    void BeginPlay() override;
};
