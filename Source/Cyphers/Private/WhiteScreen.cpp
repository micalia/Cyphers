// Fill out your copyright notice in the Description page of Project Settings.


#include "WhiteScreen.h"
#include <UMG/Public/Components/Image.h>

void UWhiteScreen::NativeConstruct()
{
	WhiteScreen = Cast<UImage>(GetWidgetFromName(TEXT("WhiteScreen")));
}
