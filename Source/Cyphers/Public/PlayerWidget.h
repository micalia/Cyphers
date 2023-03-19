// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerWidget.generated.h"

/**
 * 
 */
UCLASS()
class CYPHERS_API UPlayerWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	UPROPERTY(meta = (BindWidget))
	class UCanvasPanel* BossUI = nullptr;
	UPROPERTY(meta=(BindWidget))
	class UProgressBar* PlayerHpBar = nullptr;

	UPROPERTY(meta = (BindWidget))
		class UProgressBar* BothMouseCoolTimeBar = nullptr;
	UPROPERTY(meta = (BindWidget))
		class UProgressBar* KeyECoolTimeBar = nullptr;

	void NativeConstruct();
	void UpdateCurrHP(float curr, float max);

	void UpdateBothMouseCoolTime(float curr, float max);
	void UpdateKeyECoolTime(float curr, float max);
};
