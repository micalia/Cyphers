// Fill out your copyright notice in the Description page of Project Settings.

#include "Golem.h"
#include <Engine/SkeletalMesh.h>
#include "StoneObj.h"
#include "GolemAnim.h"
#include <Components/CapsuleComponent.h>
#include "Cypher_Kaya.h"
#include "GolemFSM.h"
#include <Kismet/GameplayStatics.h>
#include <Materials/MaterialParameterCollection.h>

AGolem::AGolem()
{
	PrimaryActorTick.bCanEverTick = true;

	//���� ������ ���� �־�� ��
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> tempMesh(TEXT("/Script/Engine.SkeletalMesh'/Game/Resources/Models/Rock_Giant/ModifyModel/FinalModifyGolem.FinalModifyGolem'"));
	if (tempMesh.Succeeded()) {
		GetMesh()->SetSkeletalMesh(tempMesh.Object);
	}
	GetMesh()->SetRelativeLocationAndRotation(FVector(-28, 0, -191), FRotator(0, -90, 0));

	static ConstructorHelpers::FClassFinder<AStoneObj> tempStoneObj(TEXT("/Script/Engine.Blueprint'/Game/Blueprints/BP_StoneObj.BP_StoneObj_C'"));
	if (tempStoneObj.Succeeded()) {
		stoneFactory = tempStoneObj.Class;
	}

	static ConstructorHelpers::FClassFinder<UGolemAnim> tempAnim(TEXT("/Script/Engine.AnimBlueprint'/Game/Blueprints/Animation/ABP_Golem.ABP_Golem_C'"));
	if (tempAnim.Succeeded())
	{
		GetMesh()->SetAnimInstanceClass(tempAnim.Class);
	}
	
	GetCapsuleComponent()->SetCapsuleHalfHeight(196);
	GetCapsuleComponent()->SetCapsuleRadius(125);

	fsm = CreateDefaultSubobject<UGolemFSM>(TEXT("UGolemFSM"));

	static ConstructorHelpers::FObjectFinder<UMaterialParameterCollection> tempMPC(TEXT("/Script/Engine.MaterialParameterCollection'/Game/Materials/MPC/MPC_StoneOpacity.MPC_StoneOpacity'"));
	if (tempMPC.Succeeded()) {
		stoneOpacity = tempMPC.Object;
	}
}
 
void AGolem::BeginPlay()
{
	Super::BeginPlay();
	mainPlayer = Cast<ACypher_Kaya>(UGameplayStatics::GetActorOfClass(GetWorld(), ACypher_Kaya::StaticClass()));
}

void AGolem::MoveJumpAttack() {
	startPos = GetActorLocation();
	originEndPos = mainPlayer->GetActorLocation();
	originEndPos.Z -= 90;
	FVector dir = FVector(startPos.X, startPos.Y, 0) - FVector(originEndPos.X, originEndPos.Y, 0);
	endPos = originEndPos - FVector(dir.GetSafeNormal().X * endPosSub, dir.GetSafeNormal().Y * endPosSub, 0);
	FVector betweenPos = FMath::Lerp(startPos, endPos, 0.5);
	betweenPos.Z += 1700;

	JumpAttackPath(startPos, betweenPos, endPos);
}

void AGolem::JumpAttackPath(FVector start, FVector between, FVector end) {
	lineLoc.Empty();

	float ratio = 1 / curvePointCount;
	for (int32 i = 0; i < (int32)curvePointCount; i++)
	{
		FVector p = CalculateBezier(ratio * i, start, between, end);
		/*GetWorld()->SpawnActor<AActor>(pathObj, p, FRotator::ZeroRotator);*/
		lineLoc.Add(p);
	}
}

FVector AGolem::CalculateBezier(float ratio, FVector start, FVector between, FVector end)
{
	FVector p01 = FMath::Lerp<FVector, float>(start, between, ratio);
	FVector p12 = FMath::Lerp<FVector, float>(between, end, ratio);
	FVector p = FMath::Lerp<FVector, float>(p01, p12, ratio);

	return p;
}
