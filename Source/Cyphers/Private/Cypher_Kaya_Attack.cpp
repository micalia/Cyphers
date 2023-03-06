// Fill out your copyright notice in the Description page of Project Settings.


#include "Cypher_Kaya_Attack.h"

void UCypher_Kaya_Attack::BeginPlay()
{
	Super::BeginPlay();
}

void UCypher_Kaya_Attack::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UCypher_Kaya_Attack::SetupInputBinding(class UInputComponent* PlayerInputComponent)
{
	Super::SetupInputBinding(PlayerInputComponent);

	PlayerInputComponent->BindAction(TEXT("MouseLeft"), IE_Released, this, &UCypher_Kaya_Attack::InputMouseLeft);
}

void UCypher_Kaya_Attack::InputMouseLeft()
{
	UE_LOG(LogTemp, Warning, TEXT("MouseLeft"))
}
