// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CpCollisionActor.generated.h"

UCLASS()
class CIPOLLINO_API ACpCollisionActor : public AActor
{
    GENERATED_BODY()
    
public:	
    // Sets default values for this actor's properties
    ACpCollisionActor();

    // Called every frame
    void Tick(float DeltaTime) override;

    UPROPERTY(EditDefaultsOnly, Category = "CollisionActor")
    class USphereComponent* collisionSphere;

    /*UPROPERTY(EditDefaultsOnly, Category = "CollisionActor")
    class UPhysicalMaterial* physicalMaterial;*/

    UPROPERTY(EditAnywhere, Category = "CollisionActor")
    TEnumAsByte<ECollisionResponse> responseToCollisionChannel;

protected:
    // Called when the game starts or when spawned
    void BeginPlay() override;
};
