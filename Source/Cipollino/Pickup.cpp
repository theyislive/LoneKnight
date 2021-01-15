// Fill out your copyright notice in the Description page of Project Settings.


#include "Pickup.h"
#include "Sound/SoundCue.h"
#include "CipollinoCharacter.h"
#include "Kismet/GameplayStatics.h"


APickup::APickup() noexcept
{
    CoinCount = 1;
}

void APickup::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent,
                             AActor* OtherActor,
                             UPrimitiveComponent* OtherComp,
                             int32 OtherBodyIndex,
                             bool bFromSweep,
                             const FHitResult& SweepResult)
{
    Super::OnOverlapBegin(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
    if(OtherActor)
    {
        const auto Cipollino = Cast<ACipollinoCharacter>(OtherActor);
        if(Cipollino)
        {
            Cipollino->IncrementCoins(CoinCount);
            Cipollino->PickupLocations.Add(GetActorLocation());
            if(OverlapParticles)
                UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), OverlapParticles, GetActorLocation(), FRotator{0.f}, true);
            if(OverlapSound)
                UGameplayStatics::PlaySound2D(this, OverlapSound);
            Destroy();
        }
    }
}

void APickup::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent,
                           AActor* OtherActor,
                           UPrimitiveComponent* OtherComp,
                           int32 OtherBodyIndex)
{
    Super::OnOverlapEnd(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex);
}
