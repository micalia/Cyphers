// Fill out your copyright notice in the Description page of Project Settings.


#include "Cypher_Kaya.h"
#include <Engine/StaticMesh.h>
#include <GameFramework/SpringArmComponent.h>
#include <Camera/CameraComponent.h>
#include "PlayerMoveInput.h"

ACypher_Kaya::ACypher_Kaya() {
	PrimaryActorTick.bCanEverTick = true;

	ConstructorHelpers::FObjectFinder<USkeletalMesh> tempMesh(TEXT("/Script/Engine.SkeletalMesh'/Game/Resources/Models/Unity_Kaya/reCharacter.reCharacter'"));
	if (tempMesh.Succeeded()) {
		GetMesh()->SetSkeletalMesh(tempMesh.Object);
	}

	GetMesh()->SetRelativeLocation(FVector(0, 0, -86));
	GetMesh()->SetRelativeRotation(FRotator(0, -90, 0));

	compSword = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("compSword"));
	compSword->SetupAttachment(GetMesh(), TEXT("Sword"));
	compSword->SetRelativeScale3D(FVector(1.183, 1.133, 1));
	
	ConstructorHelpers::FObjectFinder<UStaticMesh> tempSwordMesh(TEXT("/Script/Engine.StaticMesh'/Game/Resources/Models/NightSkySword/maya2sketchfab.maya2sketchfab'"));
	if (tempSwordMesh.Succeeded()) {
		compSword->SetStaticMesh(tempSwordMesh.Object);
	}

	compArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("compArm"));
	compArm->SetupAttachment(RootComponent);
	compArm->SetRelativeLocation(FVector(0,0, 50));

	compCam = CreateDefaultSubobject<UCameraComponent>(TEXT("compCam"));
	compCam->SetupAttachment(compArm);
	compCam->SetRelativeLocation(FVector(-110,0, 78));

//Component Attach
	compPlayerMove = CreateDefaultSubobject<UPlayerMoveInput>(TEXT("compPlayerMove"));
}

void ACypher_Kaya::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	compPlayerMove->SetupInputBinding(PlayerInputComponent);
}