// Fill out your copyright notice in the Description page of Project Settings.


#include "AimUI.h"
#include <UMG/Public/Components/Image.h>

void UAimUI::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	CurrentRotate += RotationSpeed * InDeltaTime;	
	AimImage->SetRenderTransformAngle(CurrentRotate);
}
