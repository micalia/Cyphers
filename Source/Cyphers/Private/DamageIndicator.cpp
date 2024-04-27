// Fill out your copyright notice in the Description page of Project Settings.


#include "DamageIndicator.h"
#include <UMG/Public/Components/TextBlock.h>

void UDamageIndicator::NativeConstruct()
{
	Super::NativeConstruct();

	DamageTxt = Cast<UTextBlock>(GetWidgetFromName(TEXT("DamageTxt")));
}
