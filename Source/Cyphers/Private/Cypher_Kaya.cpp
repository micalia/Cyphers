// Fill out your copyright notice in the Description page of Project Settings.


#include "Cypher_Kaya.h"
#include <Engine/StaticMesh.h>
#include <GameFramework/SpringArmComponent.h>
#include <Camera/CameraComponent.h>
#include "PlayerMoveInput.h"
#include "Cypher_Kaya_Attack.h"
#include "PlayerAnim.h"
#include <Components/StaticMeshComponent.h>
#include "PlayerCamera.h"
#include <Components/ChildActorComponent.h>
#include <Components/SceneComponent.h>

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
	compSword->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
	ConstructorHelpers::FObjectFinder<UStaticMesh> tempSwordMesh(TEXT("/Script/Engine.StaticMesh'/Game/Resources/Models/NightSkySword/maya2sketchfab.maya2sketchfab'"));
	if (tempSwordMesh.Succeeded()) {
		compSword->SetStaticMesh(tempSwordMesh.Object);
	}

	/*compArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("compArm"));
	compArm->SetupAttachment(RootComponent);
	compArm->SetRelativeLocation(FVector(0,0, 50));*/

	/*compCam = CreateDefaultSubobject<UCameraComponent>(TEXT("compCam"));
	compCam->SetupAttachment(compArm);
	compCam->SetRelativeLocation(FVector(-110,0, 78));*/

	ConstructorHelpers::FClassFinder<UPlayerAnim> tempAnim(TEXT("/Script/Engine.AnimBlueprint'/Game/Blueprints/Animation/ABP_Cypher_Kaya.ABP_Cypher_Kaya_C'"));
	if (tempAnim.Succeeded())
	{
		GetMesh()->SetAnimInstanceClass(tempAnim.Class);
	}
//Component Attach
	compPlayerMove = CreateDefaultSubobject<UPlayerMoveInput>(TEXT("compPlayerMove"));
	compKayaAttack = CreateDefaultSubobject<UCypher_Kaya_Attack>(TEXT("compKayaAttack"));


	GetCapsuleComponent()->SetCollisionProfileName(TEXT("Creature"));

	camTarget = CreateDefaultSubobject<USceneComponent>(TEXT("CameraTarget"));
	camTarget->SetupAttachment(RootComponent);

	ConstructorHelpers::FClassFinder<APlayerCamera> tempPlayerCamera(TEXT("/Script/Engine.Blueprint'/Game/Blueprints/BP_PlayerCamera.BP_PlayerCamera_C'"));
	if (tempPlayerCamera.Succeeded())
	{
		cameraFactory = tempPlayerCamera.Class;
	}
	
	CameraActorComponent = CreateDefaultSubobject<UChildActorComponent>(TEXT("CameraActorComponent"));
	CameraActorComponent->SetupAttachment(camTarget);

	// Set the child actor class to the camera actor class we defined earlier
	CameraActorComponent->SetChildActorClass(cameraFactory);

	// Move the camera actor to the character's head
	
	CameraActorComponent->SetRelativeLocation(CameraOffset);
}

void ACypher_Kaya::BeginPlay()
{
	Super::BeginPlay();
	Camera = Cast<APlayerCamera>(CameraActorComponent->GetChildActor());
	if (Camera)
	{
		Camera->SetAsMainCamera();
	}
}

void ACypher_Kaya::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ACypher_Kaya::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	compPlayerMove->SetupInputBinding(PlayerInputComponent);
	compKayaAttack->SetupInputBinding(PlayerInputComponent);
}

void ACypher_Kaya::DetachCameraActor()
{
	//FVector CameraWorldPos = Camera->GetActorLocation();
	Camera->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	//CameraActorComponent->DetachFromParent(true);
	//Camera->SetActorLocation(CameraWorldPos);
}

void ACypher_Kaya::AttachCameraActor()
{
	//CameraActorComponent->AttachToComponent(CameraActorComponent, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	
	Camera->AttachToComponent(CameraActorComponent, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	//Camera->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform, TEXT("CameraTarget"));
	//Camera->SetActorRelativeLocation(CameraOffset);
}
