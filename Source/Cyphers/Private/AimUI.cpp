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
	//// ���� UI�� ��ġ�� ����
	//FVector2D AimUILocation = AimImage->GetPo();

	//// ���콺 �̵� �Ÿ��� ���� UI�� �̵���Ŵ
	//FVector2D MouseDelta = GetInputMouseDelta();
	//FVector2D AimUIMove = FVector2D(0.f, MouseDelta.Y * MoveDistance);
	//AimUILocation += AimUIMove;

	//// UI �̵� �Ÿ��� ���� ������ �Ѿ�� ��ġ�� ȭ�� ���� ������Ŵ
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

	//// UI ��ġ�� ������Ʈ
	//AimImage->SetPositionInViewport(AimUILocation);

}
