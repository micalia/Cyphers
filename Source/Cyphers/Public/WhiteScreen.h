// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "WhiteScreen.generated.h"

/**
 * 
 */
UCLASS()
class CYPHERS_API UWhiteScreen : public UUserWidget
{
	GENERATED_BODY()
public:
	void NativeConstruct();
	UPROPERTY(meta = (BindWidget))
	class UImage* WhiteScreen = nullptr;
};
