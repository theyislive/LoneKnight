// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Item.generated.h"

UCLASS()
class CIPOLLINO_API AItem : public AActor
{
    GENERATED_BODY()

protected:
    // Called when the game starts or when spawned
    void BeginPlay() override;

public:
    // Sets default values for this actor's properties
    AItem() noexcept;

    /** Base shape collision */
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Item | Collision")
    class USphereComponent* CollisionVolume;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Item | Mesh")
    class UStaticMeshComponent* Mesh;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item | Particles")
    class UParticleSystemComponent* IdleParticlesComponent;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item | Particles")
    class UParticleSystem* OverlapParticles;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item | Sounds")
    class USoundCue* OverlapSound;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item | ItemProperties")
    bool Rotate;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item | ItemProperties")
    float RotationRate;

    // Called every frame
    void Tick(float DeltaTime) override;

    UFUNCTION()
    virtual void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent,
                                AActor* OtherActor,
                                UPrimitiveComponent* OtherComp,
                                int32 OtherBodyIndex,
                                bool bFromSweep,
                                const FHitResult& SweepResult);
    UFUNCTION()
    virtual void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent,
                              AActor* OtherActor,
                              UPrimitiveComponent* OtherComp,
                              int32 OtherBodyIndex);
};
