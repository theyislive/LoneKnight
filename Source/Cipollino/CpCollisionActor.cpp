// Fill out your copyright notice in the Description page of Project Settings.

#include "CpCollisionActor.h"
#include "Components/SphereComponent.h"
#include "PhysicalMaterials/PhysicalMaterial.h"

// Sets default values
ACpCollisionActor::ACpCollisionActor()
{
    // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;
    collisionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("Collision Sphere"));
    //physicalMaterial = CreateDefaultSubobject<UPhysicalMaterial>(TEXT("PhisicalMaterial"));
    collisionSphere->SetSphereRadius(32.f);
    collisionSphere->bHiddenInGame = false;
    collisionSphere->SetVisibility(true);
    collisionSphere->SetSimulatePhysics(true);
    collisionSphere->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    //collisionSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
    collisionSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Block);
    collisionSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Block);
    RootComponent = collisionSphere;
    responseToCollisionChannel = ECollisionResponse::ECR_Overlap;
}

// Called when the game starts or when spawned
void ACpCollisionActor::BeginPlay()
{
    Super::BeginPlay();

    /*if(physicalMaterial)
        collisionSphere->BodyInstance.SetPhysMaterialOverride(physicalMaterial);
    collisionSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, responseToCollisionChannel);*/
}

// Called every frame
void ACpCollisionActor::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

}
