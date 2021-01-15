// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyAnimInstance.h"
#include "Enemy.h"


void UEnemyAnimInstance::NativeInitializeAnimation()
{
    if(nullptr == Pawn)
    {
        Pawn = TryGetPawnOwner();
        if(Pawn)
            Enemy = Cast<AEnemy>(Pawn);
    }
}

void UEnemyAnimInstance::UpdateAnimationProperties()
{
    if(nullptr == Pawn)
        Pawn = TryGetPawnOwner();
    else
    {
        const auto Speed = Pawn->GetVelocity();
        MovementSpeed = FVector{Speed.X, Speed.Y, .0f}.Size(); // сложение компонентов вектора ?
        if(nullptr == Enemy)
            Enemy = Cast<AEnemy>(Pawn);
    }
}
