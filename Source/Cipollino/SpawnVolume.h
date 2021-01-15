// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SpawnVolume.generated.h"

UCLASS()
class CIPOLLINO_API ASpawnVolume : public AActor
{
    GENERATED_BODY()
    
protected:
    // Called when the game starts or when spawned
    void BeginPlay() override;

public:
    // Sets default values for this actor's properties
    ASpawnVolume();

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spawning")
    class UBoxComponent* SpawningBox;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawning")
    TSubclassOf<class ACritter> PawnToSpawn;

    // Called every frame
    void Tick(float DeltaTime) override;

    // Get func
    UFUNCTION(BlueprintPure, Category = "Spawning")
    FVector GetSpawnPoint() const;

    // Mixed func (cpp/blueprint)
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Spawning")
    void SpawnOurPawn(UClass* ToSpawn, const FVector& Location);
};
