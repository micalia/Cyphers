// Fill out your copyright notice in the Description page of Project Settings.


#include "PowerAttackDecal.h"
#include <Components/StaticMeshComponent.h>
#include <Components/SceneComponent.h>
#include <Engine/StaticMesh.h>

// Sets default values
APowerAttackDecal::APowerAttackDecal()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	rootComp = CreateDefaultSubobject<USceneComponent>(TEXT("RootComp"));
	SetRootComponent(rootComp);
	compDecal = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PowerAttackDecal"));
	compDecal->SetupAttachment(RootComponent);
	compDecal->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	compDecal->SetRelativeScale3D(FVector(0.75));
	
	static ConstructorHelpers::FObjectFinder<UStaticMesh> tempDecalMesh(TEXT("/Script/Engine.StaticMesh'/Engine/BasicShapes/Plane.Plane'"));
	if (tempDecalMesh.Succeeded()) {
		compDecal->SetStaticMesh(tempDecalMesh.Object);
	}
}

// Called when the game starts or when spawned
void APowerAttackDecal::BeginPlay()
{
	Super::BeginPlay();
	
	currScale = GetActorScale3D();
}

// Called every frame
void APowerAttackDecal::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (bPowerAttackEnd == false) {
		if (currSize > MaxScale) {
			currSize = MaxScale;
			return;
		}
		currSize+= biggerSpeed * DeltaTime;
		if (currSize < MaxScale) {
			compDecal->SetRelativeScale3D(FVector(currSize));
		}
	}
	else {
		currSize -= biggerSpeed * DeltaTime;
		if (currSize > 0.75) {
			compDecal->SetRelativeScale3D(FVector(currSize));
		}
		else {
			Destroy();
		}
	}
	CurrentRotate += RotationSpeed * DeltaTime;
	compDecal->SetRelativeRotation(FRotator(0, CurrentRotate,0));
	
}