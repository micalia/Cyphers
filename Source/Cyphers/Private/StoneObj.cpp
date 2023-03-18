// Fill out your copyright notice in the Description page of Project Settings.


#include "StoneObj.h"
#include <Components/SphereComponent.h>
#include <Components/StaticMeshComponent.h>
#include <Engine/StaticMesh.h>
#include <GeometryCollectionEngine/Public/GeometryCollection/GeometryCollectionComponent.h>
#include <Sound/SoundBase.h>

// Sets default values
AStoneObj::AStoneObj()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	compCollision = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
	compCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	compCollision->SetSphereRadius(163);
	compCollision->SetNotifyRigidBodyCollision(true);

	compRock = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Rock"));

	static ConstructorHelpers::FObjectFinder<UStaticMesh> tempStoneMesh(TEXT("StaticMesh'/Game/StarterContent/Props/SM_Rock.SM_Rock'"));
	if (tempStoneMesh.Succeeded()) {
		compRock->SetStaticMesh(tempStoneMesh.Object);
	}

	SetRootComponent(compCollision);
	compRock->SetupAttachment(compCollision);
	compRock->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	compRock->SetWorldScale3D(FVector(2.1f, 1.4f, 1.0f));
	compRock->SetRelativeLocation(FVector(71, 55, 10));
	compRock->SetRelativeRotation(FRotator(86, -169, 152));

	compGcStone = CreateDefaultSubobject<UGeometryCollectionComponent>(TEXT("GCstone"));
	compGcStone->SetupAttachment(compCollision);
	compGcStone->SetRelativeLocation(FVector(-41.270561, -0.190629, -143.993635));
	compGcStone->SetSimulatePhysics(false);
	compGcStone->SetVisibility(false);

	static ConstructorHelpers::FObjectFinder<UGeometryCollection> tempGcStone(TEXT("/Script/GeometryCollectionEngine.GeometryCollection'/Game/Resources/GC_Result/SM_Rock_GeometryCollection.SM_Rock_GeometryCollection'"));
	if (tempGcStone.Succeeded()) {
		compGcStone->SetRestCollection(tempGcStone.Object);
	}

	static ConstructorHelpers::FObjectFinder<USoundBase> tempStoneSound(TEXT("/Script/Engine.SoundWave'/Game/Resources/Sounds/rock_destroy_sound.rock_destroy_sound'"));
	if (tempStoneSound.Succeeded()) {
		stoneSound = tempStoneSound.Object;
	}
}

// Called when the game starts or when spawned
void AStoneObj::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AStoneObj::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AStoneObj::CrashWithPlayer(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{

}
