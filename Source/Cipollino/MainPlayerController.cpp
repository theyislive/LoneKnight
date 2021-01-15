// Fill out your copyright notice in the Description page of Project Settings.


#include "MainPlayerController.h"
#include "Blueprint/UserWidget.h"


void AMainPlayerController::BeginPlay()
{
    Super::BeginPlay();
    //if(HudOverlay = CreateWidget<UUserWidget>(this, HudOverlayAsset); HudOverlayAsset)
    if(HudOverlayAsset)
    {
        HudOverlay = CreateWidget<UUserWidget>(this, HudOverlayAsset);
        if(HudOverlay)
        {
            HudOverlay->AddToViewport();
            HudOverlay->SetVisibility(ESlateVisibility::Visible);
        }
    }
    if(WidgetEnemyHealthBar)
    {
        EnemyHealthBar = CreateWidget<UUserWidget>(this, WidgetEnemyHealthBar);
        if(EnemyHealthBar)
        {
            EnemyHealthBar->AddToViewport();
            EnemyHealthBar->SetVisibility(ESlateVisibility::Hidden);
            EnemyHealthBar->SetAlignmentInViewport({.0f, .0f} /* Aligment */);
        }
    }
}

void AMainPlayerController::DisplayEnemyHealthBar()
{
    if(EnemyHealthBar)
    {
        EnemyHealthBarVisible = true;
        EnemyHealthBar->SetVisibility(ESlateVisibility::Visible);
    }
}

void AMainPlayerController::RemoveEnemyHealthBar()
{
    if(EnemyHealthBar)
    {
        EnemyHealthBarVisible = false;
        EnemyHealthBar->SetVisibility(ESlateVisibility::Hidden);
    }
}

void AMainPlayerController::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    if(EnemyHealthBar)
    {
        FVector2D PositionInViewport;
        ProjectWorldLocationToScreen(EnemyLocation, PositionInViewport); // convert 3d position to 2d position
        PositionInViewport.Y -= 85.f;
        EnemyHealthBar->SetPositionInViewport(PositionInViewport);
        EnemyHealthBar->SetDesiredSizeInViewport({300.f, 25.f} /* size in viewport */);
    }
}
