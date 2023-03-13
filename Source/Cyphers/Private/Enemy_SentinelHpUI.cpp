// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy_SentinelHpUI.h"
#include <UMG/Public/Components/ProgressBar.h>

void UEnemy_SentinelHpUI::NativeConstruct()
{
	Super::NativeConstruct();

	currHpBar = Cast<UProgressBar>(GetWidgetFromName(TEXT("currHpBar")));
}

void UEnemy_SentinelHpUI::UpdateCurrHP(float curr, float max)
{
	if (currHpBar != nullptr) {
		//hp �� 0 ~ 1 ���̷� ǥ��
		float hp = curr / max;
	
		//Progress bar UI ����
		currHpBar->SetPercent(hp);
	}
}