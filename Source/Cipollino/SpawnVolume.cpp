// Fill out your copyright notice in the Description page of Project Settings.


#include "SpawnVolume.h"
#include "Components/BoxComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Engine/World.h"
#include "Critter.h"

// Sets default values
ASpawnVolume::ASpawnVolume()
{
    // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;
    SpawningBox = CreateDefaultSubobject<UBoxComponent>(TEXT("SpawningBox"));
}

// Called when the game starts or when spawned
void ASpawnVolume::BeginPlay()
{
    Super::BeginPlay();
    
}

// Called every frame
void ASpawnVolume::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

}

FVector ASpawnVolume::GetSpawnPoint() const
{
    const auto Extent = SpawningBox->GetScaledBoxExtent(); // Get a size box
    const auto Origin = SpawningBox->GetComponentLocation(); // Get a location box
    return UKismetMathLibrary::RandomPointInBoundingBox(Origin, Extent); // получение рандомной точки в "боксе призыва"
}

void ASpawnVolume::SpawnOurPawn_Implementation(UClass* ToSpawn, const FVector& Location)
{
    if(ToSpawn)
    {
        auto World = GetWorld();
        //if(auto World = GetWorld(); World)
        if(World)
        {
            FActorSpawnParameters SpawnParams;
            auto CritterSpawned = World->SpawnActor<ACritter>(ToSpawn, Location, FRotator(0.f), SpawnParams);
        }
    }
}
