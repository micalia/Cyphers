// Fill out your copyright notice in the Description page of Project Settings.


#include "TestPlayer.h"
#include "PlayerMoveInput.h"

ATestPlayer::ATestPlayer()
{
	PrimaryActorTick.bCanEverTick = true;

	ConstructorHelpers::FObjectFinder<USkeletalMesh> tempMesh(TEXT("/Script/Engine.SkeletalMesh'/Game/Resources/Models/Unity_Kaya/reCharacter.reCharacter'"));
	if (tempMesh.Succeeded()) {
		GetMesh()->SetSkeletalMesh(tempMesh.Object);
	}

	GetMesh()->SetRelativeLocation(FVector(0, 0, -86));
	GetMesh()->SetRelativeRotation(FRotator(0, -90, 0));

	compPlayerMove = CreateDefaultSubobject<UPlayerMoveInput>(TEXT("compPlayerMove"));
}

void ATestPlayer::BeginPlay()
{
	Super::BeginPlay();
	
	
}

void ATestPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ATestPlayer::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	compPlayerMove->SetupInputBinding(PlayerInputComponent);	
}