// Copyright Epic Games, Inc. All Rights Reserved.


#include "CipollinoGameModeBase.h"

ACipollinoGameModeBase::ACipollinoGameModeBase()
{
    PrimaryActorTick.bCanEverTick = true;
}

void ACipollinoGameModeBase::BeginPlay()
{
    Super::BeginPlay();

    // GEngine version print

    object = FVector2D(0.f, 0.f);
}

void ACipollinoGameModeBase::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    //object = TranslateToObject(object, FVector2D(.1f, .1f));

    /*if (GEngine)
        GEngine->AddOnScreenDebugMessage(-1, .1f, FColor::Red, object.ToString());*/
}

//FVector2D ACipollinoGameModeBase::TranslateToObject(FVector2D&& object, FVector2D&& byAmount)
//{
//    object += byAmount;
//    return std::move(object);
//}
//FVector2D ACipollinoGameModeBase::TranslateToObject(FVector2D objectTranslate, FVector2D byAmount)
//{
//    FVector2D tedObject = objectTranslate + byAmount;
//    return tedObject;
//}
