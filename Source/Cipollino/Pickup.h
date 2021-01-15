// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item.h"
#include "Pickup.generated.h"

/**
 * 
 */
UCLASS()
class CIPOLLINO_API APickup : public AItem
{
    GENERATED_BODY()

public:
    APickup() noexcept;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Coins")
    int32 CoinCount;

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
};
