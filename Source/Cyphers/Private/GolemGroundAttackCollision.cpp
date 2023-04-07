// Fill out your copyright notice in the Description page of Project Settings.

#include "GolemGroundAttackCollision.h"
#include <Components/BoxComponent.h>
#include "Cypher_Kaya.h"

// Sets default values
AGolemGroundAttackCollision::AGolemGroundAttackCollision()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	groundAttackCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("GACollision"));
	groundAttackCollision->SetCollisionProfileName(TEXT("EnemyAtkCollision"));
	groundAttackCollision->SetBoxExtent(FVector(50));
	groundAttackCollision->SetRelativeScale3D(FVector(0.95, 4, 4));
}

// Called when the game starts or when spawned
void AGolemGroundAttackCollision::BeginPlay()
{
	Super::BeginPlay();

	groundAttackCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	groundAttackCollision->OnComponentBeginOverlap.AddDynamic(this, &AGolemGroundAttackCollision::OnCollisionOverlap);
}

// Called every frame
void AGolemGroundAttackCollision::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	currTime += DeltaTime;
	if(currTime>lifeTime) Destroy();

	FVector p0 = GetActorLocation();
	FVector vt = GetActorForwardVector() * MoveSpeed * DeltaTime;
	SetActorLocation(p0+vt);
}

void AGolemGroundAttackCollision::OnCollisionOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ACypher_Kaya* kaya = Cast<ACypher_Kaya>(OtherActor);
	if (kaya != nullptr) {
		kaya->ReceiveDamage(2);
		groundAttackCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}

