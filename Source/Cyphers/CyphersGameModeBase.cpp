// Copyright Epic Games, Inc. All Rights Reserved.


#include "CyphersGameModeBase.h"
#include "AimUI.h"
#include "PlayerWidget.h"
#include "MainMenu.h"
#include "Cypher_Kaya.h"

ACyphersGameModeBase::ACyphersGameModeBase()
{
	PrimaryActorTick.bCanEverTick = true;

	// Crosshair Widget Class를 설정합니다.
	static ConstructorHelpers::FClassFinder<UMainMenu> tempMainMenuWidgetClass(TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/Blueprints/Widget/WB_MainMenu.WB_MainMenu_C'"));
	if (tempMainMenuWidgetClass.Succeeded())
	{
		MainMenuWidgetClass = tempMainMenuWidgetClass.Class;
	}

	static ConstructorHelpers::FClassFinder<UAimUI> tempAimUIclass(TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/Blueprints/Widget/WB_AimUI.WB_AimUI_C'"));
	if (tempAimUIclass.Succeeded())
	{
		AimUIClass = tempAimUIclass.Class;
	}

	static ConstructorHelpers::FClassFinder<UPlayerWidget> tempPlayerWidgetclass(TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/Blueprints/Widget/BP_PlayerWidget.BP_PlayerWidget_C'"));
	if (tempPlayerWidgetclass.Succeeded())
	{
		PlayerWidgetClass = tempPlayerWidgetclass.Class;
	}
	
}

void ACyphersGameModeBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	// Crosshair Widget 생성
	if (MainMenuWidgetClass != nullptr)
	{
		MainMenuWidget = CreateWidget<UMainMenu>(GetWorld(), MainMenuWidgetClass);

		if (MainMenuWidget != nullptr)
		{
			MainMenuWidget->AddToViewport();
		}
	}

	if (AimUIClass != nullptr)
	{
		aimUIWidget = CreateWidget<UAimUI>(GetWorld(), AimUIClass);

		if (aimUIWidget != nullptr)
		{
			aimUIWidget->AddToViewport();
		}
	}

	if (PlayerWidgetClass != nullptr)
	{
		playerWidget = CreateWidget<UPlayerWidget>(GetWorld(), PlayerWidgetClass);

		if (playerWidget != nullptr)
		{
			playerWidget->AddToViewport();
		}
	}

	HideUI();
}

void ACyphersGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	ShowMouseCursor();
	
	ACypher_Kaya* kaya = Cast<ACypher_Kaya>(GetWorld()->GetFirstPlayerController()->GetCharacter());
	kaya->bCameraPosFix = true;

	APlayerController* controller = GetWorld()->GetFirstPlayerController();
	kaya->DisableInput(controller);
	
}

void ACyphersGameModeBase::HideUI()
{
	playerWidget->SetVisibility(ESlateVisibility::Hidden);
	aimUIWidget->SetVisibility(ESlateVisibility::Hidden);
}

void ACyphersGameModeBase::ShowUI() {
	playerWidget->SetVisibility(ESlateVisibility::Visible);
	aimUIWidget->SetVisibility(ESlateVisibility::Visible);
}

void ACyphersGameModeBase::ShowMouseCursor()
{
	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	PlayerController->bShowMouseCursor = true;
	PlayerController->bEnableClickEvents = true;
	PlayerController->bEnableMouseOverEvents = true;
}

void ACyphersGameModeBase::HideMouseCursor()
{
	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	PlayerController->bShowMouseCursor = false;
	PlayerController->bEnableClickEvents = false;
	PlayerController->bEnableMouseOverEvents = false;
}
