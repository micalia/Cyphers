// Fill out your copyright notice in the Description page of Project Settings.


#include "AimUI.h"
#include <UMG/Public/Components/Image.h>

void UAimUI::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	CurrentRotate += RotationSpeed * InDeltaTime;	
	AimImage->SetRenderTransformAngle(CurrentRotate);
}

void UAimUI::MoveAimUI(float DeltaTime, float MoveDistance)
{
	//// 현재 UI의 위치를 얻어옴
	//FVector2D AimUILocation = AimImage->GetPo();

	//// 마우스 이동 거리에 따라 UI를 이동시킴
	//FVector2D MouseDelta = GetInputMouseDelta();
	//FVector2D AimUIMove = FVector2D(0.f, MouseDelta.Y * MoveDistance);
	//AimUILocation += AimUIMove;

	//// UI 이동 거리가 일정 범위를 넘어서면 위치를 화면 끝에 고정시킴
	//FVector2D ViewportSize = FVector2D(GEngine->GameViewport->Viewport->GetSizeXY());
	//FVector2D AimUISize = AimImage->GetDesiredSize();
	//if (AimUILocation.Y + AimUISize.Y * 0.5f > ViewportSize.Y)
	//{
	//	AimUILocation.Y = ViewportSize.Y - AimUISize.Y * 0.5f;
	//}
	//else if (AimUILocation.Y - AimUISize.Y * 0.5f < 0.f)
	//{
	//	AimUILocation.Y = AimUISize.Y * 0.5f;
	//}

	//// UI 위치를 업데이트
	//AimImage->SetPositionInViewport(AimUILocation);

}
