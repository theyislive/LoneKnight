// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MainPlayerController.generated.h"


/**
 * 
 */
UCLASS()
class CIPOLLINO_API AMainPlayerController : public APlayerController
{
    GENERATED_BODY()

public:
    /** Reference to the UMG asset in the editor */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
    TSubclassOf<class UUserWidget> HudOverlayAsset;

    /** Variable to hold the widget after creating it */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
    UUserWidget* HudOverlay;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
    TSubclassOf<UUserWidget> WidgetEnemyHealthBar;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
    UUserWidget* EnemyHealthBar;

    bool EnemyHealthBarVisible;

    void DisplayEnemyHealthBar();
    void RemoveEnemyHealthBar();

    FVector EnemyLocation;

protected:
    void BeginPlay() override;
    void Tick(float DeltaTime) override;
};
