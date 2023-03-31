// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MainMenu.generated.h"

/**
 * 
 */
UCLASS()
class CYPHERS_API UMainMenu : public UUserWidget
{
	GENERATED_BODY()
public: 
	void NativeConstruct();

	UPROPERTY(meta = (BindWidget))
		class UButton* start_btn = nullptr;
	UPROPERTY(meta = (BindWidget))
		class UButton* exit_btn = nullptr;

	UFUNCTION()
	void GameStart();
};
