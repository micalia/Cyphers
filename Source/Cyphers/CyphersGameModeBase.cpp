// Copyright Epic Games, Inc. All Rights Reserved.


#include "CyphersGameModeBase.h"
#include "AimUI.h"

ACyphersGameModeBase::ACyphersGameModeBase()
{
	PrimaryActorTick.bCanEverTick = true;

	// Crosshair Widget Class�� �����մϴ�.
	static ConstructorHelpers::FClassFinder<UAimUI> tempAimUIclass(TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/Blueprints/Widget/WB_AimUI.WB_AimUI_C'"));
	if (tempAimUIclass.Succeeded())
	{
		AimUIClass = tempAimUIclass.Class;
	}
}

void ACyphersGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	// Crosshair Widget ����
	if (AimUIClass != nullptr)
	{
		aimUIWidget = CreateWidget<UAimUI>(GetWorld(), AimUIClass);

		if (aimUIWidget != nullptr)
		{
			aimUIWidget->AddToViewport();
		}
	}
}
