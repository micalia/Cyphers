// Fill out your copyright notice in the Description page of Project Settings.


#include "MyCharacter.h"
#include <Components/BoxComponent.h>
#include <Components/StaticMeshComponent.h>
#include <Kismet/KismetMathLibrary.h>

// Sets default values
AMyCharacter::AMyCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	smComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("smComp"));
	smComp->SetupAttachment(GetRootComponent());

	smCannonComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("smCannonComp"));
	smCannonComp->SetupAttachment(smComp);
	smCannonComp->SetRelativeLocation(FVector(46,0,125));
	smCannonComp->SetRelativeRotation(FRotator(-50,0,0));
	smCannonComp->SetRelativeScale3D(FVector(0.097221, 0.097221,1));

	cannonPos = CreateDefaultSubobject<USceneComponent>(TEXT("cannonPos"));
	cannonPos->SetupAttachment(smCannonComp);
	cannonPos->SetRelativeLocation(FVector(14,0,68));
	cannonPos->SetRelativeRotation(FRotator(50,0,0));
	
	ConstructorHelpers::FObjectFinder<UStaticMesh> tempMesh(TEXT("/Script/Engine.StaticMesh'/Game/StarterContent/Shapes/Shape_NarrowCapsule.Shape_NarrowCapsule'"));
	if (tempMesh.Succeeded()) {
		smComp->SetStaticMesh(tempMesh.Object);
	}
}

// Called when the game starts or when spawned
void AMyCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	StartJump();
}

// Called every frame
void AMyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	float AttackRange = 50;
		FHitResult HitResult;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);
	bool bResult = GetWorld()->SweepSingleByChannel(
		HitResult,
		GetActorLocation(),
		GetActorLocation() + GetActorForwardVector() * AttackRange,
		FQuat::Identity,
		ECollisionChannel::ECC_Visibility,
		FCollisionShape::MakeBox(FVector(50, 30, 20)),
		Params
	);

#if ENABLE_DRAW_DEBUG

	FVector TraceVec = GetActorForwardVector() * AttackRange;
	FVector Center = GetActorLocation() + TraceVec * 0.5f;
	FQuat BoxRot = FRotationMatrix::MakeFromZ(TraceVec).ToQuat();
	FColor DrawColor = bResult ? FColor::Green : FColor::Red;
	float DebugLifeTime = 0.1f;

	DrawDebugBox(
		GetWorld(),
		Center,
		FVector(50, 30, 20),
		BoxRot,
		DrawColor,
		false,
		DebugLifeTime
	);

#endif
}

void AMyCharacter::StartJump()
{
	if (!bIsJumping)
	{
		JumpStartPos = GetActorLocation();
		JumpEndPos = JumpStartPos + JumpDistance * FVector::ForwardVector;
		JumpMidPos = JumpStartPos + FVector(0.0f, 0.0f, JumpHeight);
		JumpStartTime = GetWorld()->GetTimeSeconds();
		bIsJumping = true;
	}
}

void AMyCharacter::MoveAlongPath(float DeltaTime)
{
	float TimeElapsed = GetWorld()->GetTimeSeconds() - JumpStartTime;

	/*if (TimeElapsed >= JumpDuration)
	{
		SetActorLocation(JumpEndPos);
		return;
	}*/

	float Height = UKismetMathLibrary::FInterpTo(0.0f, JumpHeight, TimeElapsed, JumpDuration);
	FVector NewPos = GetActorLocation();
	NewPos.Z = UKismetMathLibrary::FInterpTo(NewPos.Z, JumpMidPos.Z, DeltaTime, JumpDuration / 2.0f);
	NewPos += JumpDistance * FVector::ForwardVector * DeltaTime / (JumpDuration / 2.0f);
	NewPos.Z = UKismetMathLibrary::FInterpTo(NewPos.Z, JumpEndPos.Z, DeltaTime, JumpDuration / 2.0f);
	SetActorLocation(NewPos);
}

//void AMyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
//{
//	Super::SetupPlayerInputComponent(PlayerInputComponent);
//
//	// Bind grenade throw inputs
//	PlayerInputComponent->BindAction("Grenade", IE_Pressed, this, &AMyCharacter::OnGrenadeStart);
//	PlayerInputComponent->BindAction("Grenade", IE_Released, this, &AMyCharacter::OnGrenadeRelease);
//}
//
//void AMyCharacter::ClearGrenadeTrajectory()
//{
//	GEngine->AddOnScreenDebugMessage(0, 3, FColor::Purple, FString::Printf(TEXT("ClearGrenadeTrajectory")), true, FVector2D(2, 2));
//	//UE_LOG(LogTemp, Warning, TEXT("ClearGrenadeTrajectory"))
//	/*for (ADebugSphereActor* Sphere : GrenadeTrajectoryPoints)
//	{
//		if (Sphere != nullptr)
//		{
//			Sphere->Destroy();
//		}
//	}
//	GrenadeTrajectoryPoints.Empty();*/
//}
//
//void AMyCharacter::ShowGrenadeTrajectory()
//{
//	//// Calculate initial velocity vector
//	//FVector StartPos = GetActorLocation() + GetCapsuleComponent()->GetUpVector() * 50.f;
//	//FVector LaunchVelocity = GetCapsuleComponent()->GetForwardVector().RotateAngleAxis(GrenadeThrowAngle, GetCapsuleComponent()->GetRightVector()) * GrenadeVelocity;
//
//	//// Simulate grenade trajectory
//	//TArray<FVector> TrajectoryPoints;
//	//UGameplayStatics::PredictProjectilePath(this, StartPos, LaunchVelocity, GrenadeLifetime, UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldStatic), this, TrajectoryPoints);
//
//	//// Draw debug spheres along the predicted trajectory
//	//for (FVector Point : TrajectoryPoints)
//	//{
//	//	ADebugSphereActor* Sphere = GetWorld()->SpawnActor<ADebugSphereActor>(ADebugSphereActor::StaticClass(), Point, FRotator::ZeroRotator);
//	//	if (Sphere != nullptr)
//	//	{
//	//		Sphere->SetSphereRadius(5.f);
//	//		Sphere->SetSphereColor(FColor::Red);
//	//		GrenadeTrajectoryPoints.Add(Sphere);
//	//	}
//	//}
//}
//
//void AMyCharacter::OnGrenadeStart()
//{
//	GEngine->AddOnScreenDebugMessage(0, 3, FColor::Purple, FString::Printf(TEXT("OnGrenadeStart"), 3), true, FVector2D(2, 2));
//	//// Clear previous trajectory visualization
//	//ClearGrenadeTrajectory();
//
//	//// Save initial grenade throw state
//	//GrenadeStartLocation = GetActorLocation() + GetCapsuleComponent()->GetUpVector() * 50.f;
//	//GrenadeStartRotation = GetActorRotation();
//}
//
//void AMyCharacter::OnGrenadeRelease()
//{
//	GEngine->AddOnScreenDebugMessage(0, 3, FColor::Purple, FString::Printf(TEXT("OnGrenadeRelease")), true, FVector2D(2, 2));
//}

