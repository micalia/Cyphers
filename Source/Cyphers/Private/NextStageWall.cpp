// Fill out your copyright notice in the Description page of Project Settings.


#include "NextStageWall.h"
#include <Components/BoxComponent.h>
#include <Components/StaticMeshComponent.h>
#include <Engine/StaticMesh.h>

// Sets default values
ANextStageWall::ANextStageWall()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	compBox = CreateDefaultSubobject<UBoxComponent>(TEXT("compBox"));
	SetRootComponent(compBox);
	compBox->SetBoxExtent(FVector(50));

	compSm = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("compSm"));
	compSm->SetupAttachment(compBox);
	compSm->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> tempMesh(TEXT("/Script/Engine.StaticMesh'/Engine/BasicShapes/Cube.Cube'"));
	if (tempMesh.Succeeded())
	{
		compSm->SetStaticMesh(tempMesh.Object);
	}
}

// Called when the game starts or when spawned
void ANextStageWall::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ANextStageWall::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

