// Fill out your copyright notice in the Description page of Project Settings.


#include "StoneObj.h"
#include <Components/SphereComponent.h>
#include <Components/StaticMeshComponent.h>
#include <Engine/StaticMesh.h>
#include <GeometryCollectionEngine/Public/GeometryCollection/GeometryCollectionComponent.h>
#include <Sound/SoundBase.h>
#include <Kismet/KismetMaterialLibrary.h>
#include <Kismet/GameplayStatics.h>
#include "Cypher_Kaya.h"
#include "Golem.h"

// Sets default values
AStoneObj::AStoneObj()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	compCollision = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
	compCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	compCollision->SetSphereRadius(163);
	compCollision->SetNotifyRigidBodyCollision(true);
	compCollision->SetRelativeScale3D(FVector(1.2));

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

	compCheckPlayerCollision = CreateDefaultSubobject<USphereComponent>(TEXT("CheckPlayerCollision"));
	compCheckPlayerCollision->SetupAttachment(RootComponent);
	compCheckPlayerCollision->SetCollisionProfileName(TEXT("StoneCheckPlayer"));
	compCheckPlayerCollision->SetSphereRadius(300);
}

// Called when the game starts or when spawned
void AStoneObj::BeginPlay()
{
	Super::BeginPlay();
	compCollision->OnComponentHit.AddDynamic(this, &AStoneObj::CrashWithPlayer);
	compCheckPlayerCollision->OnComponentBeginOverlap.AddDynamic(this, &AStoneObj::OnCheckPlayerOverlap);
	enemy = Cast<AGolem>(UGameplayStatics::GetActorOfClass(GetWorld(), AGolem::StaticClass()));

	compGcStone->SetVisibility(false);
}

// Called every frame
void AStoneObj::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bIsCrash == true)
	{
		if (currTime > destroyTime) {
			Destroy();
		}
		else {
			currTime += DeltaTime;
			return;
			if (transparentStartTime < currTime) {
				if (bTransparent == true) {
					compGcStone->SetCollisionEnabled(ECollisionEnabled::NoCollision);
					transparentCurrTime += DeltaTime;
					alpha = 1 - transparentCurrTime / transparentTerm;
					if (alpha > 0) {
						UKismetMaterialLibrary::SetScalarParameterValue(GetWorld(), enemy->stoneOpacity, TEXT("StoneOpacity"), alpha);
					}
					else {
						UKismetMaterialLibrary::SetScalarParameterValue(GetWorld(), enemy->stoneOpacity, TEXT("StoneOpacity"), 0);
					}
				}
			}
		}
	}
}

void AStoneObj::CrashWithPlayer(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	ACypher_Kaya* player = Cast<ACypher_Kaya>(OtherActor);
	if (player) {
		compCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		player->bCameraShake = true;
		bIsCrash = true;
		bTransparent = true;
		compGcStone->SetVisibility(true);
		compGcStone->SetSimulatePhysics(true);
		//compGcStone->AddImpulse(addImpulsePower);

		compGcStone->ApplyExternalStrain(0, Hit.ImpactPoint, 0,0,100,100);
		compRock->SetVisibility(false);
		//enemy->bCameraShake = true;

		/*UGameplayStatics::SpawnEmitterAtLocation(GetWorld(),
							crashEffect,
							HitComponent->GetComponentLocation(),
							GetActorRotation());*/

		player->ReceiveDamage(2);
		UGameplayStatics::PlaySound2D(GetWorld(), stoneSound);
		//Destroy();
	}
	/*else {
		compCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		bIsCrash = true;
		bTransparent = true;
		compGcStone->SetVisibility(true);
		compGcStone->SetSimulatePhysics(true);
		compRock->SetVisibility(false);

		UGameplayStatics::PlaySound2D(GetWorld(), stoneSound);
	}*/
}

void AStoneObj::OnCheckPlayerOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ACypher_Kaya* player = Cast<ACypher_Kaya>(OtherActor);
	if (player) {
		compGcStone->SetSimulatePhysics(true);
	}
}

