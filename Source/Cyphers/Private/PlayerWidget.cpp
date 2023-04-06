// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerWidget.h"
#include "Components/ProgressBar.h"
#include <UMG/Public/Components/CanvasPanel.h>
#include <UMG/Public/Components/TextBlock.h>

void UPlayerWidget::NativeConstruct()
{
	Super::NativeConstruct();

	PlayerHpBar = Cast<UProgressBar>(GetWidgetFromName(TEXT("PlayerHpBar")));
	BothMouseCoolTimeBar = Cast<UProgressBar>(GetWidgetFromName(TEXT("BothMouseCoolTimeBar")));
	KeyECoolTimeBar = Cast<UProgressBar>(GetWidgetFromName(TEXT("KeyECoolTimeBar")));
	KeyFCoolTimeBar = Cast<UProgressBar>(GetWidgetFromName(TEXT("KeyFCoolTimeBar")));
	BossUI = Cast<UCanvasPanel>(GetWidgetFromName(TEXT("BossUI")));
	GolemHP = Cast<UProgressBar>(GetWidgetFromName(TEXT("GolemHP")));
	BossHPtxt = Cast<UTextBlock>(GetWidgetFromName(TEXT("BossHPtxt")));
}

void UPlayerWidget::UpdateCurrHP(float curr, float max)
{
	if (PlayerHpBar != nullptr) {
		//hp �� 0 ~ 1 ���̷� ǥ��
		float hp = curr / max;
		//Progress bar UI ����
		PlayerHpBar->SetPercent(hp);
	}
}

void UPlayerWidget::UpdateBothMouseCoolTime(float curr, float max)
{
	if (BothMouseCoolTimeBar != nullptr) {
		//hp �� 0 ~ 1 ���̷� ǥ��
		float cool = curr / max;
		//Progress bar UI ����
		BothMouseCoolTimeBar->SetPercent(cool);
	}
}

void UPlayerWidget::UpdateKeyECoolTime(float curr, float max)
{
	if (KeyECoolTimeBar != nullptr) {
		//hp �� 0 ~ 1 ���̷� ǥ��
		float cool = curr / max;
		//Progress bar UI ����
		KeyECoolTimeBar->SetPercent(cool);
	}
}


void UPlayerWidget::UpdateKeyFCoolTime(float curr, float max)
{
	if (KeyFCoolTimeBar != nullptr) {
		//hp �� 0 ~ 1 ���̷� ǥ��
		float cool = curr / max;
		//Progress bar UI ����
		KeyFCoolTimeBar->SetPercent(cool);
	}
}

void UPlayerWidget::UpdateSpaceBarCoolTime(float curr, float max)
{
	if (SpaceBarCoolTimerBar != nullptr) {
		//hp �� 0 ~ 1 ���̷� ǥ��
		float cool = curr / max;
		//Progress bar UI ����
		SpaceBarCoolTimerBar->SetPercent(cool);
	}
}

void UPlayerWidget::UpdateBossCurrHP(float curr, float max)
{
	if (GolemHP != nullptr) {
		//hp �� 0 ~ 1 ���̷� ǥ��
		float hp = curr / max;
		//Progress bar UI ����
		GolemHP->SetPercent(hp);

		if(curr<0) curr = 0;
		FString strHP = FString::Printf(TEXT("%d% / %d%"), (int32)(curr), (int32)max);
		BossHPtxt->SetText(FText::FromString(strHP));
	}
}
