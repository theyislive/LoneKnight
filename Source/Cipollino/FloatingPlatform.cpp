// Fill out your copyright notice in the Description page of Project Settings.


#include "FloatingPlatform.h"
#include "TimerManager.h"
#include "Components/StaticMeshComponent.h"

// Sets default values
AFloatingPlatform::AFloatingPlatform()
{
    // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;
    Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
    RootComponent = Mesh;
    StartPoint = FVector{0.f};
    EndPoint = FVector{0.f};
    Interpring = false;
    InterpolationSpeed = 4.f;
    InterpolationTime = 1.f; // sec
}

// Called when the game starts or when spawned
void AFloatingPlatform::BeginPlay()
{
    Super::BeginPlay();
    StartPoint = GetActorLocation();
    EndPoint += StartPoint; // ����� �������� � �������� ����� ��������� ����� �.�. �������� ����� ���������� � ��������� ����������� ������� � ������������
    Distance = (EndPoint - StartPoint).Size();
    GetWorldTimerManager().SetTimer(InterpolationTimer, this, &AFloatingPlatform::ToggleInterpring, InterpolationTime);
}

// Called every frame
void AFloatingPlatform::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    if(Interpring)
    {
        const auto CurrentLocation = GetActorLocation();
        // ������������. ������� ����������� �� ������ ������� � �������
        const auto Interpolation = FMath::VInterpTo(CurrentLocation,
                                                         EndPoint,
                                                               DeltaTime /* ����� � �������� �� ���������� ����� */,
                                                               InterpolationSpeed);
        SetActorLocation(Interpolation);
        const auto DistanceTraveled = (GetActorLocation() - StartPoint).Size();
        //if(auto DistanceTraveled = (GetActorLocation() - StartPoint).Size(); Distance - DistanceTraveled <= 1.f)
        if(Distance - DistanceTraveled <= 1.f)
        {
            ToggleInterpring();
            SwapVectors(StartPoint, EndPoint);
            GetWorldTimerManager().SetTimer(InterpolationTimer, this, &AFloatingPlatform::ToggleInterpring, InterpolationTime);
        }
    }
}

void AFloatingPlatform::ToggleInterpring()
{
    Interpring = !Interpring;
}

void AFloatingPlatform::SwapVectors(FVector& VecOne, FVector& VecTwo) const
{
    const auto Temp = VecOne;
    VecOne = VecTwo;
    VecTwo = Temp;
}
