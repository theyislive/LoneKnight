// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Collider.generated.h"

UCLASS()
class CIPOLLINO_API ACollider : public APawn
{
    GENERATED_BODY()

protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

public:
    // Sets default values for this pawn's properties
    ACollider();

    // Called every frame
    void Tick(float DeltaTime) override;

    // Called to bind functionality to input
    void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

    UPROPERTY(VisibleAnywhere, Category = "Mesh")
    UStaticMeshComponent* MeshComponent;

    UPROPERTY(VisibleAnywhere, Category = "Mesh")
    class USphereComponent* SphereComponent;

    UPROPERTY(VisibleAnywhere, Category = "Mesh")
    class UCameraComponent* Camera;

    UPROPERTY(VisibleAnywhere, Category = "Mesh")
    class USpringArmComponent* SpringArm;

    UPROPERTY(VisibleAnywhere, Category = "Mesh")
    class UColliderMovementComponent* OurMovementComponent;

    UPawnMovementComponent* GetMovementComponent() const override;

    FORCEINLINE UStaticMeshComponent* GetMeshComponent() { return MeshComponent; }
    FORCEINLINE void SetMeshComponent(UStaticMeshComponent* Mesh) { MeshComponent = Mesh; }
    FORCEINLINE USphereComponent* GetSphereComponent() { return SphereComponent; }
    FORCEINLINE void SetSphereComponent(USphereComponent* Sphere) { SphereComponent = Sphere; }
    FORCEINLINE UCameraComponent* GetCameraComponent() { return Camera; }
    FORCEINLINE void SetCameraComponent(UCameraComponent* CameraComponent) { Camera = CameraComponent; }
    FORCEINLINE USpringArmComponent* GetSpringArmComponent() { return SpringArm; }
    FORCEINLINE void SetSpringArmComponent(USpringArmComponent* SpringArmComponent) { SpringArm = SpringArmComponent; }

private:
    void MoveForward(float value);
    void MoveRight(float value);
    void CameraPitch(float AxisValue);
    void CameraYaw(float AxisValue);

    FVector2D CameraInput;
};
