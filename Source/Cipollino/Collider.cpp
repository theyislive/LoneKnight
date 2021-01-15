// Fill out your copyright notice in the Description page of Project Settings.


#include "Collider.h"
#include "Engine/StaticMesh.h"
#include "ColliderMovementComponent.h"
#include "Camera/CameraComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "Components/SphereComponent.h"
#include "Components/InputComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/SpringArmComponent.h"


// Sets default values
ACollider::ACollider()
{
    // Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;
    RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
    SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
    SphereComponent->SetupAttachment(GetRootComponent());
    SphereComponent->InitSphereRadius(40.f);
    SphereComponent->SetCollisionProfileName(TEXT("Pawn"));

    MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
    MeshComponent->SetupAttachment(GetRootComponent());
    static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshComponentAsset(TEXT("StaticMesh'/Game/StarterContent/Shapes/Shape_Sphere.Shape_Sphere'"));
    if (MeshComponentAsset.Succeeded())
    {
        MeshComponent->SetStaticMesh(MeshComponentAsset.Object);
        MeshComponent->SetRelativeLocation(FVector{0.f, 0.f, -40.f});
        MeshComponent->SetWorldScale3D(FVector{.8f, .8f, .8f});
    }
    SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
    SpringArm->SetupAttachment(GetRootComponent());
    SpringArm->SetRelativeRotation(FRotator{-45.f, 0.f, 0.f}); // исходное положение в сторону пешки
    SpringArm->TargetArmLength = 400.f; // длина руки которая держит камеру на расстоянии
    SpringArm->bEnableCameraLag = true; // задержка движения камеры
    SpringArm->CameraLagSpeed = 3.f;
    Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
    Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
    OurMovementComponent = CreateDefaultSubobject<UColliderMovementComponent>(TEXT("MovementComponent"));
    OurMovementComponent->UpdatedComponent = RootComponent; // компонент который двигаем
    CameraInput = {.0f, .0f};
    AutoPossessPlayer = EAutoReceiveInput::Player0; // bind player. single player only EAutoReceiveInput::Player0
}

// Called when the game starts or when spawned
void ACollider::BeginPlay()
{
    Super::BeginPlay();
    
}

// Called every frame
void ACollider::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    auto NewRotation = GetActorRotation();
    NewRotation.Yaw += CameraInput.X;
    SetActorRotation(NewRotation); // rotate actor
    auto NewSpringArmRotation = SpringArm->GetComponentRotation();
    NewSpringArmRotation.Pitch = FMath::Clamp(NewSpringArmRotation.Pitch += CameraInput.Y, -80.f, -15.f);
    SpringArm->SetWorldRotation(NewSpringArmRotation); // rotate spring arm
}

// Called to bind functionality to input
void ACollider::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);
    PlayerInputComponent->BindAxis(TEXT("MoveForward"), this, &ACollider::MoveForward);
    PlayerInputComponent->BindAxis(TEXT("MoveRight"), this, &ACollider::MoveRight);
    PlayerInputComponent->BindAxis(TEXT("CameraPitch"), this, &ACollider::CameraPitch);
    PlayerInputComponent->BindAxis(TEXT("CameraYaw"), this, &ACollider::CameraYaw);
}

UPawnMovementComponent* ACollider::GetMovementComponent() const
{
    return OurMovementComponent;
}

void ACollider::MoveForward(float value)
{
    const auto forward = GetActorForwardVector();
    if (OurMovementComponent)
    {
        OurMovementComponent->AddInputVector(value * forward);
    }
}

void ACollider::MoveRight(float value)
{
    const auto right = GetActorRightVector();
    if (OurMovementComponent)
    {
        OurMovementComponent->AddInputVector(value * right, true);
    }
}

void ACollider::CameraYaw(float AxisValue)
{
    CameraInput.X = AxisValue;
}

void ACollider::CameraPitch(float AxisValue)
{
    CameraInput.Y = AxisValue;
}
