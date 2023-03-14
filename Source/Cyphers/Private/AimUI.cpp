// Fill out your copyright notice in the Description page of Project Settings.


#include "AimUI.h"
#include <UMG/Public/Components/CanvasPanel.h>

void UAimUI::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	CurrentRotate += RotationSpeed * InDeltaTime;	
	AimCanvas->SetRenderTransformAngle(CurrentRotate);
}
