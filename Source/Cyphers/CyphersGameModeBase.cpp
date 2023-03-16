// Copyright Epic Games, Inc. All Rights Reserved.


#include "CyphersGameModeBase.h"
#include "AimUI.h"
#include "PlayerWidget.h"

ACyphersGameModeBase::ACyphersGameModeBase()
{
	PrimaryActorTick.bCanEverTick = true;

	// Crosshair Widget Class를 설정합니다.
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

void ACyphersGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	// Crosshair Widget 생성
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
}
