// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "CipollinoGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class CIPOLLINO_API ACipollinoGameModeBase : public AGameModeBase
{
    GENERATED_BODY()
    
public:
    ACipollinoGameModeBase();

    void BeginPlay() override;
    void Tick(float DeltaTime) override;

private:
    FVector2D object;

    //FVector2D TranslateToObject(FVector2D&& object, FVector2D&& byAmount);
    //FVector2D TranslateToObject(FVector2D object, FVector2D byAmount);
};
