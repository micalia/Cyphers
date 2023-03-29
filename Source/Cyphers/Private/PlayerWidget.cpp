// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerWidget.h"
#include "Components/ProgressBar.h"
#include <UMG/Public/Components/CanvasPanel.h>

void UPlayerWidget::NativeConstruct()
{
	Super::NativeConstruct();

	PlayerHpBar = Cast<UProgressBar>(GetWidgetFromName(TEXT("PlayerHpBar")));
	BothMouseCoolTimeBar = Cast<UProgressBar>(GetWidgetFromName(TEXT("BothMouseCoolTimeBar")));
	KeyECoolTimeBar = Cast<UProgressBar>(GetWidgetFromName(TEXT("KeyECoolTimeBar")));
	KeyFCoolTimeBar = Cast<UProgressBar>(GetWidgetFromName(TEXT("KeyFCoolTimeBar")));
	BossUI = Cast<UCanvasPanel>(GetWidgetFromName(TEXT("BossUI")));
	GolemHP = Cast<UProgressBar>(GetWidgetFromName(TEXT("GolemHP")));
}

void UPlayerWidget::UpdateCurrHP(float curr, float max)
{
	if (PlayerHpBar != nullptr) {
		//hp 를 0 ~ 1 사이로 표현
		float hp = curr / max;
		//Progress bar UI 갱신
		PlayerHpBar->SetPercent(hp);
	}
}

void UPlayerWidget::UpdateBothMouseCoolTime(float curr, float max)
{
	if (BothMouseCoolTimeBar != nullptr) {
		//hp 를 0 ~ 1 사이로 표현
		float cool = curr / max;
		//Progress bar UI 갱신
		BothMouseCoolTimeBar->SetPercent(cool);
	}
}

void UPlayerWidget::UpdateKeyECoolTime(float curr, float max)
{
	if (KeyECoolTimeBar != nullptr) {
		//hp 를 0 ~ 1 사이로 표현
		float cool = curr / max;
		//Progress bar UI 갱신
		KeyECoolTimeBar->SetPercent(cool);
	}
}


void UPlayerWidget::UpdateKeyFCoolTime(float curr, float max)
{
	if (KeyFCoolTimeBar != nullptr) {
		//hp 를 0 ~ 1 사이로 표현
		float cool = curr / max;
		//Progress bar UI 갱신
		KeyFCoolTimeBar->SetPercent(cool);
	}
}

void UPlayerWidget::UpdateSpaceBarCoolTime(float curr, float max)
{
	if (SpaceBarCoolTimerBar != nullptr) {
		//hp 를 0 ~ 1 사이로 표현
		float cool = curr / max;
		//Progress bar UI 갱신
		SpaceBarCoolTimerBar->SetPercent(cool);
	}
}

void UPlayerWidget::UpdateBossCurrHP(float curr, float max)
{
	if (GolemHP != nullptr) {
		//hp 를 0 ~ 1 사이로 표현
		float hp = curr / max;
		//Progress bar UI 갱신
		GolemHP->SetPercent(hp);
	}
}
