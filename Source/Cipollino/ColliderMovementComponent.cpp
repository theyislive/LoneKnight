// Fill out your copyright notice in the Description page of Project Settings.


#include "ColliderMovementComponent.h"

void UColliderMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                               FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
    // check to make sure everything is still valid and that we are allowed to move
    if (!PawnOwner || !UpdatedComponent || ShouldSkipUpdate(DeltaTime))
        return;
    // get and clear the vector from collider
    const auto DesiredMovementThisFrame = ConsumeInputVector().GetClampedToMaxSize(1.f) * DeltaTime * 150.f /* speed */;
    if (!DesiredMovementThisFrame.IsNearlyZero())
    {
        FHitResult Hit; // hit box
        SafeMoveUpdatedComponent(DesiredMovementThisFrame, UpdatedComponent->GetComponentRotation(),
                                 true /* проверка на то что может ли вещъ проходить через другие объекты */,
                                 Hit);
        // if we bump into something, slide along the side of it
        if (Hit.IsValidBlockingHit())
            SlideAlongSurface(DesiredMovementThisFrame, 1.f - Hit.Time, Hit.Normal, Hit); // скольжение вдоль поверхности
    }
}
