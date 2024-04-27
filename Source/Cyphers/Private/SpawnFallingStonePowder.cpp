// Fill out your copyright notice in the Description page of Project Settings.

//#include "CascadeParticleSystemComponent.h"
#include "SpawnFallingStonePowder.h"
#include <Particles/ParticleSystemComponent.h>

// Sets default values
ASpawnFallingStonePowder::ASpawnFallingStonePowder()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	rootComp = CreateDefaultSubobject<USceneComponent>(TEXT("rootComp"));
	SetRootComponent(rootComp);
	vfxComp = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("vfxComp"));
	vfxComp->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void ASpawnFallingStonePowder::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASpawnFallingStonePowder::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

