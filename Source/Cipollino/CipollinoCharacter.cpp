// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon.h"
#include "Enemy.h"
#include "CipollinoCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/Controller.h"
#include "Camera/CameraComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/SkeletalMeshComponent.h"
#include "MainPlayerController.h"
#include "Animation/AnimInstance.h"
#include "CipollinoAnimInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"


// Sets default values
ACipollinoCharacter::ACipollinoCharacter()
{
    // set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;
    // create camera boom (pulls towards the player if there's a collision)
    CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
    CameraBoom->SetupAttachment(GetRootComponent());
    CameraBoom->TargetArmLength = 600.f; // camera follows at this distance
    CameraBoom->bUsePawnControlRotation = true; // rotate arm based on controller
    GetCapsuleComponent()->SetCapsuleSize(48.f, 105.f); // set size for collision capsule
    FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
    FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
    /** attach the camera to the end the boom let the boom adjust to match
     * the controller orientation
     */
    FollowCamera->bUsePawnControlRotation = false;
    // set our rates for input
    BaseTurnUpRate = BaseLookUpRate = 65.f;
    // don't rotate when the controller rotates
    // let that turn rates for input
    bUseControllerRotationYaw = false;
    bUseControllerRotationPitch = false;
    bUseControllerRotationRoll = false;
    // configure character movement
    GetCharacterMovement()->bOrientRotationToMovement = true; // character moves in the direction of input... (движение в сторону куда смотрит камера)
    GetCharacterMovement()->RotationRate = FRotator{.0f, 540.f, .0f}; // ...поварачивание по оси рысканья с этой скоростью вращения
    GetCharacterMovement()->JumpZVelocity = 650.f; // высота прыжка
    GetCharacterMovement()->AirControl = .2f; // управление в прыжке
    MaxHealth = 100.f;
    Health = 65.f;
    MaxStamina = 150.f;
    Stamina = 120.f;
    Coins = 0;
    RunningSpeed = 650.f;
    SprintingSpeed = 950.f;
    bShiftKeyDown = false;
    LeftMouseButton = false;
    // Initialize Enums
    MovementStatus = EMovementStatus::EMS_Normal;
    StaminaStatus = EStaminaStatus::ESS_Normal;
    StaminaDrainRate = 25.f;
    MinSprintStamina = 50.f;
    InterpSpeed = 15.f;
    InterpToEnemy = false;
    HasCombatTarget = false;
    EnemyDamageCauser = nullptr;
    MovingForwardBackward = false;
    MovingRightLeft = false;
}

void ACipollinoCharacter::ShowPickupLocations()
{
    for(const auto& Location: PickupLocations)
    UKismetSystemLibrary::DrawDebugSphere(this,
                                          Location,
                                          25.f,
                                          8,
                                          FLinearColor::Green,
                                          10.f,
                                          .5f);
}

const auto ACipollinoCharacter::GetLookAtRotationYaw(FVector Target) const
{
    const auto LookAtRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), Target);
    return FRotator{.0f, LookAtRotation.Yaw, .0f};
}

void ACipollinoCharacter::ShiftKeyDown() noexcept
{
    bShiftKeyDown = true;
}

void ACipollinoCharacter::ShiftKeyUp() noexcept
{
    bShiftKeyDown = false;
}

void ACipollinoCharacter::SetMovementStatus(EMovementStatus Status) noexcept
{
    MovementStatus = Status;
    if(EMovementStatus::EMS_Sprinting == MovementStatus)
        GetCharacterMovement()->MaxWalkSpeed = SprintingSpeed;
    else
        GetCharacterMovement()->MaxWalkSpeed = RunningSpeed;
}

// Called when the game starts or when spawned
void ACipollinoCharacter::BeginPlay()
{
    Super::BeginPlay();
    MainPlayerController = Cast<AMainPlayerController>(GetController());
}

void ACipollinoCharacter::IncrementCoins(int32 Amount) noexcept
{
    if(Coins + Amount <= 99999)
        Coins += Amount;
}

void ACipollinoCharacter::DecrementHealth(float Amount) noexcept
{
    Health -= Amount;
    if(Health <= .0f)
    {
        Health = .0f;
        Die();
    }
}

float ACipollinoCharacter::TakeDamage(float DamageAmount,
                                      const FDamageEvent& DamageEvent,
                                      AController* EventInstigator,
                                      AActor* DamageCauser)
{
    EnemyDamageCauser = DamageCauser;
    DecrementHealth(DamageAmount);
    return DamageAmount;
}

void ACipollinoCharacter::Die() noexcept
{
    if(EnemyDamageCauser)
    {
        const auto Enemy = Cast<AEnemy>(EnemyDamageCauser);
        if(Enemy)
            Enemy->HasValidTarget = false;
    }
    const auto AnimInstance = GetMesh()->GetAnimInstance();
    SetMovementStatus(EMovementStatus::EMS_Death);
    if(AnimInstance && CombatMontage)
    {
        AnimInstance->Montage_Play(CombatMontage, 1.f);
        AnimInstance->Montage_JumpToSection(FName{"Death"}, CombatMontage);
    }
}

void ACipollinoCharacter::Jump()
{
    if(EMovementStatus::EMS_Death != MovementStatus)
        Super::Jump();
}

void ACipollinoCharacter::DeathEnd()
{
    GetMesh()->bPauseAnims = true;
    GetMesh()->bNoSkeletonUpdate = true;
    if(InputComponent)
        InputComponent->ClearActionBindings();
}

// Called every frame
void ACipollinoCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    if(EMovementStatus::EMS_Death == MovementStatus)
        return;
    const auto DeltaStamina = StaminaDrainRate * DeltaTime; // sec
    const auto AnimInstance = Cast<UCipollinoAnimInstance>(GetMesh()->GetAnimInstance());
    if(AnimInstance)
        if(!AnimInstance->bIsInAir /*&& (MovingForwardBackward || MovingRightLeft)*/)
            switch(StaminaStatus)
            {
            case EStaminaStatus::ESS_Normal:
                if(bShiftKeyDown)
                {
                    if(Stamina - StaminaDrainRate <= MinSprintStamina) // < MinSprintStamina
                        SetStaminaStatus(EStaminaStatus::ESS_BelowMinimum);
                    //Stamina -= DeltaStamina;
                    if(MovingForwardBackward || MovingRightLeft)
                    {
                        Stamina -= DeltaStamina;
                        SetMovementStatus(EMovementStatus::EMS_Sprinting);
                    }
                    else
                    {
                        if(Stamina + DeltaStamina >= MaxStamina)
                            Stamina = MaxStamina;
                        else
                            Stamina += DeltaStamina;
                        SetMovementStatus(EMovementStatus::EMS_Normal);
                        SetStaminaStatus(EStaminaStatus::ESS_Normal);
                    }
                }
                else // Shift key up
                {
                    if(Stamina + DeltaStamina >= MaxStamina)
                        Stamina = MaxStamina;
                    else
                        Stamina += DeltaStamina;
                    SetMovementStatus(EMovementStatus::EMS_Normal);
                }
                break;
            case EStaminaStatus::ESS_BelowMinimum:
                if(bShiftKeyDown)
                {
                    /*if(Stamina - StaminaDrainRate > 0.f)
                    {
                        Stamina -= DeltaStamina;
                        SetMovementStatus(EMovementStatus::EMS_Sprinting);
                    }
                    else
                    {
                        Stamina = 0.f;
                        SetStaminaStatus(EStaminaStatus::ESS_Exhausted);
                        SetMovementStatus(EMovementStatus::EMS_Normal);
                    }*/
                    if(Stamina - DeltaStamina <= 0.f)
                    {
                        Stamina = 0.f;
                        SetStaminaStatus(EStaminaStatus::ESS_Exhausted);
                        SetMovementStatus(EMovementStatus::EMS_Normal);
                    }
                    else
                    {
                        //Stamina -= DeltaStamina;
                        if(MovingForwardBackward || MovingRightLeft)
                        {
                            Stamina -= DeltaStamina;
                            SetMovementStatus(EMovementStatus::EMS_Sprinting);
                        }
                        else
                        {
                            if(Stamina + DeltaStamina >= MinSprintStamina)
                                SetStaminaStatus(EStaminaStatus::ESS_Normal);
                            Stamina += DeltaStamina;
                            SetMovementStatus(EMovementStatus::EMS_Normal);
                        }
                            //SetMovementStatus(EMovementStatus::EMS_Normal);
                        //SetMovementStatus(EMovementStatus::EMS_Sprinting);
                    }
                }
                else // Shift key up
                {
                    if(Stamina + DeltaStamina >= MinSprintStamina)
                        SetStaminaStatus(EStaminaStatus::ESS_Normal);
                    Stamina += DeltaStamina;
                    SetMovementStatus(EMovementStatus::EMS_Normal);
                }
                break;
            case EStaminaStatus::ESS_Exhausted:
                SetMovementStatus(EMovementStatus::EMS_Normal);
                if(bShiftKeyDown)
                    Stamina = 0.f;
                else // Shift key up
                {
                    Stamina += DeltaStamina;
                    SetStaminaStatus(EStaminaStatus::ESS_ExhaustedRecovering);
                }
                break;
            case EStaminaStatus::ESS_ExhaustedRecovering:
                Stamina += DeltaStamina;
                SetMovementStatus(EMovementStatus::EMS_Normal);
                if(Stamina + DeltaStamina >= MinSprintStamina)
                    SetStaminaStatus(EStaminaStatus::ESS_Normal);
                break;
            default:
                ;
            }
            else // Is not in air
            {
                if(Stamina + DeltaStamina >= MaxStamina)
                    Stamina = MaxStamina;
                else
                    Stamina += DeltaStamina;
                //UE_LOG(LogTemp, Warning, TEXT("Sandbox cleanup took %5.3f seconds for platforms %s"), CleanSandboxTime, *PlatformNames);
                //SetMovementStatus(EMovementStatus::EMS_Normal);
            }
    if(InterpToEnemy && CombatTarget)
    {
        const auto LookAtYaw = GetLookAtRotationYaw(CombatTarget->GetActorLocation());
        const auto InterpRotation = FMath::RInterpTo(GetActorRotation(), LookAtYaw, DeltaTime, InterpSpeed);
        SetActorRotation(InterpRotation);
    }
    if(HasCombatTarget && CombatTarget)
    {
        CombatTargetLocation = CombatTarget->GetActorLocation();
        if(MainPlayerController)
            MainPlayerController->EnemyLocation = CombatTargetLocation;
    }
}

// Called to bind functionality to input
void ACipollinoCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);
    check(PlayerInputComponent);
    PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACipollinoCharacter::Jump);
    PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);
    PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &ACipollinoCharacter::ShiftKeyDown);
    PlayerInputComponent->BindAction("Sprint", IE_Released, this, &ACipollinoCharacter::ShiftKeyUp);
    PlayerInputComponent->BindAction("LeftMouseButton", IE_Pressed, this, &ACipollinoCharacter::LeftMouseButtonDown);
    PlayerInputComponent->BindAction("LeftMouseButton", IE_Released, this, &ACipollinoCharacter::LeftMouseButtonUp);
    PlayerInputComponent->BindAxis("MoveForward", this, &ACipollinoCharacter::MoveForwardBackward);
    PlayerInputComponent->BindAxis("MoveRight", this, &ACipollinoCharacter::MoveRightLeft);
    PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput); // yaw/рысканье
    PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);// pitch/наклон
    PlayerInputComponent->BindAxis("TurnRate", this, &ACipollinoCharacter::TurnAtRate);
    PlayerInputComponent->BindAxis("LookUpAtRate", this, &ACipollinoCharacter::LookUpAtRate);
}

void ACipollinoCharacter::MoveForwardBackward(float Value)
{
    MovingForwardBackward = false;
    if (nullptr != Controller && .0f != Value && !Attacking && EMovementStatus::EMS_Death != MovementStatus)
    {
        MovingForwardBackward = true;
        // find out which way is forward
        const auto Rotation = Controller->GetControlRotation();
        const FRotator YawRotation{.0f, Rotation.Yaw, .0f};
        // получить локальную матрицу поворота используя вектор рысканья. Здесь берется X поворот на "вперед/назад"
        const auto Direction = FRotationMatrix{YawRotation}.GetUnitAxis(EAxis::X);
        AddMovementInput(Direction, Value);
    }
}

void ACipollinoCharacter::MoveRightLeft(float Value)
{
    MovingRightLeft = false;
    if (nullptr != Controller && .0f != Value && !Attacking && EMovementStatus::EMS_Death != MovementStatus)
    {
        MovingRightLeft = true;
        // find out which way is forward
        const auto Rotation = Controller->GetControlRotation();
        const FRotator YawRotation{.0f, Rotation.Yaw, .0f};
        // получить локальную матрицу поворота используя вектор рысканья. Здесь берется Y поворот на "налево/направо"
        const auto Direction = FRotationMatrix{YawRotation}.GetUnitAxis(EAxis::Y);
        AddMovementInput(Direction, Value);
    }
}

void ACipollinoCharacter::TurnAtRate(float Rate)
{
    // добавление рыскания для контроллера на градусов в секунду * кол-во времени (дельты) кадра чтобы был плавный взгляд вверх
    AddControllerYawInput(Rate * BaseTurnUpRate * GetWorld()->GetDeltaSeconds());
}

void ACipollinoCharacter::LookUpAtRate(float Rate)
{
    // добавление наклон для контроллера на градусов в секунду * кол-во времени (дельты) кадра чтобы был плавный взгляд вверх
    AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void ACipollinoCharacter::LeftMouseButtonDown()
{
    LeftMouseButton = true;
    if(EMovementStatus::EMS_Death == MovementStatus)
        return;
    if(ActiveOverlappingItem)
    {
        const auto Weapon = Cast<AWeapon>(ActiveOverlappingItem);
        if(Weapon)
        {
            Weapon->Equip(this);
            SetActiveOverlappingItem(nullptr);
        }
    }
    else if(EquippedWeapon)
        AttackStart();
}

void ACipollinoCharacter::LeftMouseButtonUp()
{
    LeftMouseButton = false;
}

void ACipollinoCharacter::SetEquippedWeapon(AItem* WeaponToSet) noexcept
{
    if(EquippedWeapon)
        EquippedWeapon->Destroy();
    EquippedWeapon = WeaponToSet;
}

void ACipollinoCharacter::AttackStart() noexcept
{
    if(!Attacking && EMovementStatus::EMS_Death != MovementStatus)
    {
        Attacking = true;
        SetInterpToEnemy(true);
        auto AnimInstance = GetMesh()->GetAnimInstance();
        if(AnimInstance && CombatMontage)
        {
            const auto Section = FMath::RandRange(0, 1);
            switch(Section)
            {
            case 0:
                AnimInstance->Montage_Play(CombatMontage, 2.1f);
                AnimInstance->Montage_JumpToSection(FName("Attack0"), CombatMontage);
                break;

            case 1:
                AnimInstance->Montage_Play(CombatMontage, 1.8f);
                AnimInstance->Montage_JumpToSection(FName("Attack1"), CombatMontage);
                break;

            default:
                ;
            }
        }
        /*
        if(EquippedWeapon)
        {
            const auto ReallyEquippedWeapon = Cast<AWeapon>(EquippedWeapon);
            if(ReallyEquippedWeapon)
                if(ReallyEquippedWeapon->SwingSound)
                    UGameplayStatics::PlaySound2D(this, ReallyEquippedWeapon->SwingSound);
        }
        */
    }
}

void ACipollinoCharacter::AttackEnd() noexcept
{
    Attacking = false;
    SetInterpToEnemy(false);
    if(LeftMouseButton)
        AttackStart();
}

void ACipollinoCharacter::PlaySwingSound()
{
    const auto ReallyEquippedWeapon = Cast<AWeapon>(EquippedWeapon);
    if(ReallyEquippedWeapon)
        if(ReallyEquippedWeapon->SwingSound)
            UGameplayStatics::PlaySound2D(this, ReallyEquippedWeapon->SwingSound);
}
