// Fill out your copyright notice in the Description page of Project Settings.


#include "Golem.h"
#include <Engine/SkeletalMesh.h>
#include "StoneObj.h"

AGolem::AGolem()
{
	PrimaryActorTick.bCanEverTick = true;
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> tempMesh(TEXT("/Script/Engine.SkeletalMesh'/Game/Resources/Models/Rock_Giant/Meshes/SK_Rock_Giant.SK_Rock_Giant'"));
	if (tempMesh.Succeeded()) {
		GetMesh()->SetSkeletalMesh(tempMesh.Object);
	}
	GetMesh()->SetRelativeLocationAndRotation(FVector(0, 0, -84), FRotator(0, -90, 0));

	static ConstructorHelpers::FClassFinder<AStoneObj> tempStoneObj(TEXT("/Script/Engine.Blueprint'/Game/Blueprints/BP_StoneObj.BP_StoneObj_C'"));
	if (tempStoneObj.Succeeded()) {
		stoneFactory = tempStoneObj.Class;
	}
}
