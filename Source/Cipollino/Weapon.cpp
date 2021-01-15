// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon.h"
#include "Sound/SoundCue.h"
#include "CipollinoCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Components/SkeletalMeshComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "Components/BoxComponent.h"
#include "GameFramework/DamageType.h"
#include "Enemy.h"


AWeapon::AWeapon() noexcept
{
    SkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMesh"));
    SkeletalMesh->SetupAttachment(GetRootComponent());
    CombatCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("CombatCollision"));
    CombatCollision->SetupAttachment(GetRootComponent());
    WeaponParticles = false;
    WeaponState = EWeaponState::EWS_Pickup;
    Damage = 25.f;
}

void AWeapon::BeginPlay()
{
    Super::BeginPlay();
    CombatCollision->OnComponentBeginOverlap.AddDynamic(this, &AWeapon::CombatOnOverlapBegin);
    CombatCollision->OnComponentEndOverlap.AddDynamic(this, &AWeapon::CombatOnOverlapEnd);
    CombatCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    CombatCollision->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
    CombatCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
    CombatCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
}

void AWeapon::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent,
                             AActor* OtherActor,
                             UPrimitiveComponent* OtherComp,
                             int32 OtherBodyIndex,
                             bool bFromSweep,
                             const FHitResult& SweepResult)
{
    Super::OnOverlapBegin(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
    if(EWeaponState::EWS_Pickup == WeaponState && OtherActor)
    {
        //if(auto Cipollino = Cast<ACipollinoCharacter>(OtherActor); Cipollino)
        const auto Cipollino = Cast<ACipollinoCharacter>(OtherActor);
        if(Cipollino)
            Cipollino->SetActiveOverlappingItem(this);
    }
}

void AWeapon::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent,
                           AActor* OtherActor,
                           UPrimitiveComponent* OtherComp,
                           int32 OtherBodyIndex)
{
    Super::OnOverlapEnd(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex);
    if(OtherActor)
    {
        //if(auto Cipollino = Cast<ACipollinoCharacter>(OtherActor); Cipollino)
        const auto Cipollino = Cast<ACipollinoCharacter>(OtherActor);
        if(Cipollino)
            Cipollino->SetActiveOverlappingItem(nullptr);
    }
}

void AWeapon::Equip(ACipollinoCharacter* Character) noexcept
{
    if(Character)
    {
        SetWeaponInstigator(Character->GetController());
        // Если предмет в руке оказывается между персонажем и камерой тогда игнорировать коллизии
        SkeletalMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
        SkeletalMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
        // Отключить физику потому что подключаем меш к игроку
        SkeletalMesh->SetSimulatePhysics(false);
        const auto RightHandSocket = Character->GetMesh()->GetSocketByName("RightHandSocket"); // Получаем сокет по его названию в skeleton
        //if(const auto RightHandSocket = Character->GetMesh()->GetSocketByName("RightHandSocket"); RightHandSocket)
        if(RightHandSocket)
        {
            const auto EquippedWeapon = RightHandSocket->AttachActor(this, Character->GetMesh());
            if(EquippedWeapon)
            {
                Rotate = false;
                Character->SetEquippedWeapon(this);
                if(!WeaponParticles)
                    IdleParticlesComponent->Deactivate();
                if(OnEquipSound)
                    UGameplayStatics::PlaySound2D(this, OnEquipSound);
            }
        }
    }
}

void AWeapon::CombatOnOverlapBegin(UPrimitiveComponent* OverlappedComponent,
                                   AActor* OtherActor,
                                   UPrimitiveComponent* OtherComp,
                                   int32 OtherBodyIndex,
                                   bool bFromSweep,
                                   const FHitResult& SweepResult)
{
    if(OtherActor)
    {
        const auto Enemy = Cast<AEnemy>(OtherActor);
        if(Enemy)
        {
            //if(Enemy->HitParticles)
            //{
            //    const auto WeaponSocket = SkeletalMesh->GetSocketByName("WeaponSocket");
            //    /*
            //    *
            //    TCHAR TempName[1024];
            //    FCString::Strcpy(TempName, *FName::SafeString(SweepResult.BoneName.GetDisplayIndex(), SweepResult.BoneName.GetNumber()));
            //    UE_LOG(LogTemp, Warning, TEXT("CombatOnOverlapBegin. SweepResult: %s"), TempName);
            //    */
            //    if(WeaponSocket)
            //        UGameplayStatics::SpawnEmitterAtLocation(GetWorld(),
            //                                                 Enemy->HitParticles,
            //                                                 WeaponSocket->GetSocketLocation(SkeletalMesh) /*GetActorLocation()*/,
            //                                                 FRotator{.0f},
            //                                                 false);
            //    if(Enemy->HitSound)
            //        UGameplayStatics::PlaySound2D(this, Enemy->HitSound);
            //}
            const auto WeaponSocket = SkeletalMesh->GetSocketByName("WeaponSocket");
            if(WeaponSocket)
                if(Enemy->HitParticles)
                    UGameplayStatics::SpawnEmitterAtLocation(GetWorld(),
                                                             Enemy->HitParticles,
                                                             WeaponSocket->GetSocketLocation(SkeletalMesh) /*GetActorLocation()*/,
                                                             FRotator{.0f},
                                                             false);
            if(Enemy->HitSound)
                UGameplayStatics::PlaySound2D(this, Enemy->HitSound);
            if(DamageTypeClass)
                UGameplayStatics::ApplyDamage(Enemy,
                                              Damage,
                                              WeaponInstigator,
                                              this,
                                              DamageTypeClass);
        }
    }
}

void AWeapon::CombatOnOverlapEnd(UPrimitiveComponent* OverlappedComponent,
                                 AActor* OtherActor,
                                 UPrimitiveComponent* OtherComp,
                                 int32 OtherBodyIndex)
{

}

void AWeapon::ActivateCollision()
{
    CombatCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void AWeapon::DeactivateCollision()
{
    CombatCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}
