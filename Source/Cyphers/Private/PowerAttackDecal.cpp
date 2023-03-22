// Fill out your copyright notice in the Description page of Project Settings.


#include "PowerAttackDecal.h"
#include <Components/StaticMeshComponent.h>
#include <Components/SceneComponent.h>
#include <Engine/StaticMesh.h>
#include <Kismet/KismetMaterialLibrary.h>
#include <Materials/MaterialParameterCollection.h>

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

	static ConstructorHelpers::FObjectFinder<UMaterialParameterCollection> tempMPC(TEXT("/Script/Engine.MaterialParameterCollection'/Game/Materials/MPC/MPC_DecalOpacity.MPC_DecalOpacity'"));
	if (tempMPC.Succeeded()) {
		decalOpacity = tempMPC.Object;
	}
}

// Called when the game starts or when spawned
void APowerAttackDecal::BeginPlay()
{
	Super::BeginPlay();
	UKismetMaterialLibrary::SetScalarParameterValue(GetWorld(), decalOpacity, TEXT("DecalOpacity"), defaultOpacity);
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
		currOpacityTime += DeltaTime;
		alpha = defaultOpacity - currOpacityTime / OpacityTermTime;
		if (alpha > 0) {
			UKismetMaterialLibrary::SetScalarParameterValue(GetWorld(), decalOpacity, TEXT("DecalOpacity"), alpha);
		}
		else {
			UKismetMaterialLibrary::SetScalarParameterValue(GetWorld(), decalOpacity, TEXT("DecalOpacity"), 0);
			Destroy();
		}

		/* 스킬 시전 후 점점 작아지게
		currSize -= biggerSpeed * DeltaTime;
		if (currSize > 0.75) {
			compDecal->SetRelativeScale3D(FVector(currSize));
		}
		else {
			Destroy();
		}*/
	}
	CurrentRotate += RotationSpeed * DeltaTime;
	compDecal->SetRelativeRotation(FRotator(0, CurrentRotate,0));
	
}