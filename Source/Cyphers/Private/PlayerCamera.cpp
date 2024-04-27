// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCamera.h"
#include <Camera/CameraComponent.h>
#include <Kismet/KismetMathLibrary.h>
#include <Camera/CameraShakeBase.h>

// Sets default values
APlayerCamera::APlayerCamera()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	compRoot = CreateDefaultSubobject<USceneComponent>(TEXT("compRoot"));

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	CameraComponent->SetupAttachment(compRoot);
	static ConstructorHelpers::FClassFinder<UCameraShakeBase> tempCameraShake(TEXT("/Script/Engine.Blueprint'/Game/Blueprints/BP_CameraShake.BP_CameraShake_C'"));
	if (tempCameraShake.Succeeded()) {
		cameraShake = tempCameraShake.Class;
	}
	powerAttackCamMove = CreateDefaultSubobject<USceneComponent>(TEXT("camMove"));
	powerAttackCamMove->SetupAttachment(compRoot);
}

void APlayerCamera::BeginPlay()
{
	Super::BeginPlay();
	CameraMoveDelegate.BindUObject(this, &APlayerCamera::CameraMoveEvent);
	camOriginPosMove = CameraComponent->GetRelativeLocation();
	prevCamPos = CameraComponent->GetRelativeLocation();
}

void APlayerCamera::SetAsMainCamera()
{
	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	PlayerController->SetViewTarget(this);
}

void APlayerCamera::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if(bSkillCamMove == true)SkillCamMove();
	if (bCameraShake == true)CameraShakeRandom();
}

void APlayerCamera::CameraMoveEvent()
{
	cameraOriginPos = GetActorLocation();
	bCameraShake = true;
}

void APlayerCamera::CameraShakeRandom()
{
	if (camCurrTime < cameraShakeTime) {
		camCurrTime += GetWorld()->GetDeltaSeconds();
		float y = UKismetMathLibrary::RandomFloatInRange(-12, 12);
		float z = UKismetMathLibrary::RandomFloatInRange(-12, 12);
		SetActorLocation(cameraOriginPos + FVector(0, y, z));
	}
	else {
		bCameraShake = false;
		SetActorLocation(cameraOriginPos);
		camCurrTime = 0;
	}
}

void APlayerCamera::SkillCamMove()
{
	if (bSkillReady) {
		prevCamPos = FMath::Lerp(prevCamPos, powerAttackCamMove->GetRelativeLocation(), GetWorld()->GetDeltaSeconds() * SkillCamSpeed);
		CameraComponent->SetRelativeLocation(prevCamPos);
	}
	else {
		prevCamPos = FMath::Lerp(prevCamPos, camOriginPosMove, GetWorld()->GetDeltaSeconds() * SkillCamSpeed);
		CameraComponent->SetRelativeLocation(prevCamPos);
		float gap = FMath::Abs((prevCamPos - camOriginPosMove).Length());
		if (gap < 0.01) {
			CameraComponent->SetRelativeLocation(camOriginPosMove);
			bSkillReady = false;
			bSkillCamMove = false;
		}
	}
}
