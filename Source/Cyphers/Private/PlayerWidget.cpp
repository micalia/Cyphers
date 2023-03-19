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
	BossUI = Cast<UCanvasPanel>(GetWidgetFromName(TEXT("BossUI")));
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
