// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCamera.h"
#include <Camera/CameraComponent.h>
#include <Kismet/KismetMathLibrary.h>

// Sets default values
APlayerCamera::APlayerCamera()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	
}

void APlayerCamera::SetAsMainCamera()
{
	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	PlayerController->SetViewTarget(this);
}

void APlayerCamera::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

}
