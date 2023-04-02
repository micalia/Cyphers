// Fill out your copyright notice in the Description page of Project Settings.


#include "MainMenu.h"
#include <UMG/Public/Components/Button.h>
#include "../CyphersGameModeBase.h"
#include "Cypher_Kaya.h"

void UMainMenu::NativeConstruct()
{
	Super::NativeConstruct();

	start_btn = Cast<UButton>(GetWidgetFromName(TEXT("start_btn")));
	exit_btn = Cast<UButton>(GetWidgetFromName(TEXT("exit_btn")));

	start_btn->OnClicked.AddDynamic(this, &UMainMenu::GameStart);

}

void UMainMenu::GameStart()
{
	SetVisibility(ESlateVisibility::Hidden);

	ACyphersGameModeBase* gameMode = Cast<ACyphersGameModeBase>(GetWorld()->GetAuthGameMode());
	gameMode->HideMouseCursor();
	gameMode->ShowUI();

	ACypher_Kaya* kaya = Cast<ACypher_Kaya>(GetWorld()->GetFirstPlayerController()->GetCharacter());
	kaya->bCameraPosFix = false;
	kaya->GetMesh()->SetVisibility(true);
	kaya->compSword->SetVisibility(true);

	APlayerController* controller = GetWorld()->GetFirstPlayerController();
	kaya->EnableInput(controller);
}
