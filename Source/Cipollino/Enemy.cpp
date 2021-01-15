// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Components/SphereComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/BoxComponent.h"
#include "CipollinoCharacter.h"
#include "Sound/SoundCue.h"
#include "AIController.h"
#include "MainPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Animation/AnimInstance.h"
#include "GameFramework/DamageType.h"
#include "TimerManager.h"
#include "Components/CapsuleComponent.h"


// Sets default values
AEnemy::AEnemy() noexcept
{
    // Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;
    AgroSphere = CreateDefaultSubobject<USphereComponent>(TEXT("AgroSphere"));
    AgroSphere->SetupAttachment(GetRootComponent());
    AgroSphere->InitSphereRadius(600.f);
    CombatSphere = CreateDefaultSubobject<USphereComponent>(TEXT("CombatSphere"));
    CombatSphere->SetupAttachment(GetRootComponent());
    CombatSphere->InitSphereRadius(75.f);
    CombatCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("CombatCollision"));
    CombatCollision->SetupAttachment(GetMesh(), FName{"SocketFrontLeftKnee"});
    EnemyMovementStatus = EEnemyMovementStatus::EMS_Idle;
    OverlappingCombatSphere = false;
    Attacking = false;
    Health = 75.f;
    MaxHealth = 100.f;
    Damage = 10.f;
    AttackMinTime = .5;
    AttackMaxTime = 3.5f;
    InterpSpeed = 2.5f; // a value change
    InterpToTarget = false;
    DeathDelay = 3.f;
    HasValidTarget = false;
}

// Called when the game starts or when spawned
void AEnemy::BeginPlay()
{
    Super::BeginPlay();
    AIController = Cast<AAIController>(GetController());
    AgroSphere->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::AgroSphereOnOverlapBegin);
    AgroSphere->OnComponentEndOverlap.AddDynamic(this, &AEnemy::AgroSphereOnOverlapEnd);
    CombatSphere->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::CombatSphereOnOverlapBegin);
    CombatSphere->OnComponentEndOverlap.AddDynamic(this, &AEnemy::CombatSphereOnOverlapEnd);
    CombatCollision->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::CombatOnOverlapBegin);
    CombatCollision->OnComponentEndOverlap.AddDynamic(this, &AEnemy::CombatOnOverlapEnd);
    CombatCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    CombatCollision->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
    CombatCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
    CombatCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
}

const auto AEnemy::GetLookAtRotationYaw(FVector Target) const
{
    const auto LookAtRotate = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), Target);
    return FRotator{.0f, LookAtRotate.Yaw, .0f};
}

// Called every frame
void AEnemy::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    if(/*InterpToTarget &&*/ OverlappingCombatSphere)
    {
        const auto LookAtYaw = GetLookAtRotationYaw(CombatTarget->GetActorLocation());
        const auto Interp = FMath::RInterpTo(GetActorRotation(), LookAtYaw, DeltaTime, InterpSpeed);
        SetActorRotation(Interp);
    }
}

// Called to bind functionality to input
void AEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void AEnemy::AgroSphereOnOverlapBegin(UPrimitiveComponent* OverlappedComponent,
                                      AActor* OtherActor,
                                      UPrimitiveComponent* OtherComp,
                                      int32 OtherBodyIndex,
                                      bool bFromSweep,
                                      const FHitResult& SweepResult)
{
    if(OtherActor && Alive())
    {
        const auto Cipollino = Cast<ACipollinoCharacter>(OtherActor);
        if(Cipollino)
        {
            CombatTarget = Cipollino;
            MoveToTarget(Cipollino);
        }
    }
}

void AEnemy::AgroSphereOnOverlapEnd(UPrimitiveComponent* OverlappedComponent,
                                    AActor* OtherActor,
                                    UPrimitiveComponent* OtherComp,
                                    int32 OtherBodyIndex)
{
    if(OtherActor)
    {
        const auto Cipollino = Cast<ACipollinoCharacter>(OtherActor);
        if(Cipollino)
        {
            HasValidTarget = false;
            CombatTarget = nullptr;
            Cipollino->SetHasCombatTarget(false); //!!!
            if(Cipollino->MainPlayerController)
                Cipollino->MainPlayerController->RemoveEnemyHealthBar();
            SetEnemyMovementStatus(EEnemyMovementStatus::EMS_Idle);
            if(AIController)
                AIController->StopMovement();
        }
    }
}

void AEnemy::CombatSphereOnOverlapBegin(UPrimitiveComponent* OverlappedComponent,
                                        AActor* OtherActor,
                                        UPrimitiveComponent* OtherComp,
                                        int32 OtherBodyIndex,
                                        bool bFromSweep,
                                        const FHitResult& SweepResult)
{
    if(OtherActor && Alive())
    {
        const auto Cipollino = Cast<ACipollinoCharacter>(OtherActor);
        if(Cipollino)
        {
            HasValidTarget = true;
            Cipollino->SetCombatTarget(this);
            Cipollino->SetHasCombatTarget(true);
            if(Cipollino->MainPlayerController)
                Cipollino->MainPlayerController->DisplayEnemyHealthBar();
            OverlappingCombatSphere = true;
            AttackStart();
        }
    }
}

void AEnemy::CombatSphereOnOverlapEnd(UPrimitiveComponent* OverlappedComponent,
                                      AActor* OtherActor,
                                      UPrimitiveComponent* OtherComp,
                                      int32 OtherBodyIndex)
{
    if(OtherActor)
    {
        const auto Cipollino = Cast<ACipollinoCharacter>(OtherActor);
        if(Cipollino)
        {
            if(this == Cipollino->CombatTarget)
            {
                Cipollino->SetCombatTarget(nullptr);
                //Cipollino->SetHasCombatTarget(false);
            }
            OverlappingCombatSphere = false;
            //GetWorldTimerManager().ClearTimer(AttackTimer);
            /*if(EEnemyMovementStatus::EMS_Attacking != EnemyMovementStatus)
                MoveToTarget(Cipollino);*/
        }
    }
}

void AEnemy::MoveToTarget(ACipollinoCharacter* Target)
{
    SetEnemyMovementStatus(EEnemyMovementStatus::EMS_MoveToTarget);
    if(AIController)
    {
        FAIMoveRequest MoveRequest;
        MoveRequest.SetGoalActor(Target);
        MoveRequest.SetAcceptanceRadius(10.f); // radius accept
        FNavPathSharedPtr NavPath;
        AIController->MoveTo(MoveRequest, &NavPath);
        /*
        * Draw path points
        auto PathPoints = NavPath->GetPathPoints();
        for(const auto& Point: PathPoints)
            UKismetSystemLibrary::DrawDebugSphere(this,
                                                  Point.Location,
                                                  25.f,
                                                  8,
                                                  FLinearColor::Red,
                                                  10.f,
                                                  .5f);*/
    }
}

void AEnemy::CombatOnOverlapBegin(UPrimitiveComponent* OverlappedComponent,
                                  AActor* OtherActor,
                                  UPrimitiveComponent* OtherComp,
                                  int32 OtherBodyIndex,
                                  bool bFromSweep,
                                  const FHitResult& SweepResult)
{
    if(OtherActor)
    {
        const auto Cipollino = Cast<ACipollinoCharacter>(OtherActor);
        if(Cipollino)
        {
            const auto TipSocket = GetMesh()->GetSocketByName("TipSocketFrontLeftKnee");
            if(TipSocket && Cipollino->HitParticles)
                UGameplayStatics::SpawnEmitterAtLocation(GetWorld(),
                                                         Cipollino->HitParticles,
                                                         TipSocket->GetSocketLocation(GetMesh()),
                                                         FRotator{.0f},
                                                         false);
            if(Cipollino->HitSound)
                UGameplayStatics::PlaySound2D(this, Cipollino->HitSound);
            if(DamageTypeClass)
                UGameplayStatics::ApplyDamage(Cipollino, Damage, AIController, this, DamageTypeClass);
        }
    }
}

void AEnemy::CombatOnOverlapEnd(UPrimitiveComponent* OverlappedComponent,
                                AActor* OtherActor,
                                UPrimitiveComponent* OtherComp,
                                int32 OtherBodyIndex)
{

}

void AEnemy::ActivateCollision()
{
    CombatCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    if(SwingSound)
        UGameplayStatics::PlaySound2D(this, SwingSound);
}

void AEnemy::DeactivateCollision()
{
    CombatCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AEnemy::AttackStart() noexcept
{
    if(Alive() && !Attacking && OverlappingCombatSphere && HasValidTarget)
    {
        Attacking = true;
        SetInterpToTarget(true);
        if(AIController)
        {
            AIController->StopMovement();
            SetEnemyMovementStatus(EEnemyMovementStatus::EMS_Attacking);
        }
        auto AnimMontage = GetMesh()->GetAnimInstance();
        if(AnimMontage)
        {
            AnimMontage->Montage_Play(CombatMontage, 1.35f);
            AnimMontage->Montage_JumpToSection(FName{"Attack0"}, CombatMontage);
        }
    }
}

void AEnemy::AttackEnd() noexcept
{
    Attacking = false;
    //SetInterpToTarget(false);
    if(OverlappingCombatSphere)
    {
        const auto AttackTime = FMath::RandRange(AttackMinTime, AttackMaxTime);
        GetWorldTimerManager().SetTimer(AttackTimer, this, &AEnemy::AttackStart, AttackTime);
    }
    else
        SetInterpToTarget(false);
}

void AEnemy::DecrementHealth(float Amount) noexcept
{
    Health -= Amount;
    if(Health <= 0.f)
    {
        Health = .0f;
        Die();
    }
}

void AEnemy::Die() noexcept
{
    const auto AnimInstance = GetMesh()->GetAnimInstance();
    if(AnimInstance && CombatMontage)
    {
        SetEnemyMovementStatus(EEnemyMovementStatus::EMS_Dead);
        AnimInstance->Montage_Play(CombatMontage, 1.f);
        AnimInstance->Montage_JumpToSection(FName{"Death"}, CombatMontage);
        CombatCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        AgroSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        CombatSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    }
}

void AEnemy::DeathEnd()
{
    GetMesh()->bPauseAnims = true;
    GetMesh()->bNoSkeletonUpdate = true;
    GetWorldTimerManager().SetTimer(DeathTimer, this, &AEnemy::Disappear, DeathDelay);
}

bool AEnemy::Alive() noexcept
{
    return GetEnemyMovementStatus() != EEnemyMovementStatus::EMS_Dead;
}

void AEnemy::Disappear() noexcept
{
    Destroy();
}

float AEnemy::TakeDamage(float DamageAmount,
                         const FDamageEvent& DamageEvent,
                         AController* EventInstigator,
                         AActor* DamageCauser)
{
    DecrementHealth(DamageAmount);
    return DamageAmount;
}
