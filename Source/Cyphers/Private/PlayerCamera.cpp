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

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	
	static ConstructorHelpers::FClassFinder<UCameraShakeBase> tempCameraShake(TEXT("/Script/Engine.Blueprint'/Game/Blueprints/BP_CameraShake.BP_CameraShake_C'"));
	if (tempCameraShake.Succeeded()) {
		cameraShake = tempCameraShake.Class;
	}

}

void APlayerCamera::BeginPlay()
{
	Super::BeginPlay();
	CameraMoveDelegate.BindUObject(this, &APlayerCamera::CameraMoveEvent);
}

void APlayerCamera::SetAsMainCamera()
{
	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	PlayerController->SetViewTarget(this);
}

void APlayerCamera::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

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