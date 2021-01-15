// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Critter.generated.h"

UCLASS()
class CIPOLLINO_API ACritter : public APawn
{
    GENERATED_BODY()

protected:
    // Called when the game starts or when spawned
    void BeginPlay() override;

public:
    // Sets default values for this pawn's properties
    ACritter();

    // Called every frame
    void Tick(float DeltaTime) override;

    // Called to bind functionality to input
    void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

    UPROPERTY(EditAnywhere)
    USkeletalMeshComponent* meshComponent;

    UPROPERTY(EditAnywhere)
    class UCameraComponent* camera;

    UPROPERTY(EditAnywhere, Category = "PawnMovement")
    float maxSpeed;

private:
    void MoveForward(float value);
    void MoveRight(float value);

    FVector currentVelocity;
};
