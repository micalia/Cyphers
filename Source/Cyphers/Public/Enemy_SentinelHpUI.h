// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Enemy_SentinelHpUI.generated.h"

/**
 * 
 */
UCLASS()
class CYPHERS_API UEnemy_SentinelHpUI : public UUserWidget
{
	GENERATED_BODY()
public:
	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
		class UProgressBar* currHpBar = nullptr;

};
