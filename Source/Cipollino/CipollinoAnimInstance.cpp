// Fill out your copyright notice in the Description page of Project Settings.


#include "CipollinoAnimInstance.h"
#include "CipollinoCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

void UCipollinoAnimInstance::NativeInitializeAnimation()
{
    if (nullptr == Pawn)
    {
        Pawn = TryGetPawnOwner();
        if(Pawn)
            Cipollino = Cast<ACipollinoCharacter>(Pawn);
    }
}

void UCipollinoAnimInstance::UpdateAnimationProperties()
{
    if (nullptr == Pawn)
        Pawn = TryGetPawnOwner();
    else
    {
        const auto Speed = Pawn->GetVelocity();
        MovementSpeed = FVector{Speed.X, Speed.Y, .0f}.Size(); // сложение компонентов вектора ?
        bIsInAir = Pawn->GetMovementComponent()->IsFalling(); // падает ли персонаж
        if(nullptr == Cipollino)
            Cipollino = Cast<ACipollinoCharacter>(Pawn);
    }
}
