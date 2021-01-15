// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FloatingPlatform.generated.h"

UCLASS()
class CIPOLLINO_API AFloatingPlatform : public AActor
{
    GENERATED_BODY()
    
protected:
    // Called when the game starts or when spawned
    void BeginPlay() override;

public:
    // Sets default values for this actor's properties
    AFloatingPlatform();

    FTimerHandle InterpolationTimer;

    UPROPERTY(EditAnywhere, meta = (MakeEditWidget = "true"))
    bool Interpring;

    /** Mesh for the platform */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Platform")
    class UStaticMeshComponent* Mesh;

    UPROPERTY(EditAnywhere)
    FVector StartPoint;

    UPROPERTY(EditAnywhere, meta = (MakeEditWidget = "true"))
    FVector EndPoint;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Platform")
    float InterpolationSpeed;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Platform")
    float InterpolationTime;

    float Distance;

    // Called every frame
    void Tick(float DeltaTime) override;

    void ToggleInterpring();

    void SwapVectors(FVector& VecOne, FVector& VecTwo) const;
};
