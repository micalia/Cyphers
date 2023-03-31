// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "CyphersGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class CYPHERS_API ACyphersGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
public:
	ACyphersGameModeBase();
	virtual void PostInitializeComponents() override;
	virtual void BeginPlay() override;


	TSubclassOf<class UMainMenu> MainMenuWidgetClass;
	UPROPERTY()
		UMainMenu* MainMenuWidget;

	TSubclassOf<class UAimUI> AimUIClass;
	UPROPERTY()
		UAimUI* aimUIWidget;

	TSubclassOf<class UPlayerWidget> PlayerWidgetClass;
	UPROPERTY()
		UPlayerWidget* playerWidget;

		void HideUI();
		void ShowUI();

		void ShowMouseCursor();
		void HideMouseCursor();
};
