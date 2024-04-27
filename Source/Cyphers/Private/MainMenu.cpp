// Fill out your copyright notice in the Description page of Project Settings.


#include "MainMenu.h"
#include <UMG/Public/Components/Button.h>
#include "../CyphersGameModeBase.h"
#include "Cypher_Kaya.h"
#include <../Plugins/FX/Niagara/Source/Niagara/Public/NiagaraComponent.h>
#include <Kismet/GameplayStatics.h>

void UMainMenu::NativeConstruct()
{
	Super::NativeConstruct();

	start_btn = Cast<UButton>(GetWidgetFromName(TEXT("start_btn")));
	exit_btn = Cast<UButton>(GetWidgetFromName(TEXT("exit_btn")));

	start_btn->OnClicked.AddDynamic(this, &UMainMenu::GameStart);
	exit_btn->OnClicked.AddDynamic(this, &UMainMenu::GameExit);
}

void UMainMenu::GameStart()
{
	SetVisibility(ESlateVisibility::Hidden);

	ACyphersGameModeBase* gameMode = Cast<ACyphersGameModeBase>(GetWorld()->GetAuthGameMode());
	gameMode->HideMouseCursor();
	gameMode->ShowUI();

	ACypher_Kaya* kaya = Cast<ACypher_Kaya>(GetWorld()->GetFirstPlayerController()->GetCharacter());
	kaya->bCameraPosFix = false;
	kaya->bPlayerAppear = true; 
	kaya->disolveNiagara->SetActive(true);
	kaya->compSword->SetVisibility(true);
	UGameplayStatics::PlaySound2D(GetWorld(), kaya->playerAppearSound);

	APlayerController* controller = GetWorld()->GetFirstPlayerController();
	kaya->EnableInput(controller);
}

void UMainMenu::GameExit()
{
	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	if (PlayerController)
	{
		PlayerController->ConsoleCommand("quit");
	}
}

