// Fill out your copyright notice in the Description page of Project Settings.


#include "BlockStone.h"
#include <Components/BoxComponent.h>
#include <Components/StaticMeshComponent.h>

// Sets default values
ABlockStone::ABlockStone()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	compBox = CreateDefaultSubobject<UBoxComponent>(TEXT("BOX"));
	compBox->SetBoxExtent(FVector(252, 370, 460));
	compBox->SetCollisionProfileName(TEXT("BlockAll"));
	SetRootComponent(compBox);
	compSM = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ROCK"));
	compSM->SetupAttachment(compBox);
	compSM->SetRelativeRotation(FRotator(-84, -78, 78));
	compSM->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> tempRockMesh(TEXT("/Script/Engine.StaticMesh'/Game/Resources/Models/Env/SoulCave/Environment/Meshes/Rocks/SM_Cave_Rock_Ceiling.SM_Cave_Rock_Ceiling'"));
	if (tempRockMesh.Succeeded()) {
		compSM->SetStaticMesh(tempRockMesh.Object);
	}
	compSM->SetRelativeScale3D(FVector(1.1, 1.3, 1.7));
}

// Called when the game starts or when spawned
void ABlockStone::BeginPlay()
{
	Super::BeginPlay();
	
	disappearZheight = GetActorLocation().Z - 2000;
}

// Called every frame
void ABlockStone::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bNextStage) {
		FVector p0 = GetActorLocation();
		FVector vt = FVector::DownVector * moveSpeed * GetWorld()->DeltaTimeSeconds;
		FVector p = p0 + vt;

		if (p.Z < disappearZheight)
		{
			Destroy();
		}
		else
		{
			SetActorLocation(p);
		}
	}
}

