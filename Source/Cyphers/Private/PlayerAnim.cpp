#include "PlayerAnim.h"
#include "Cypher_Kaya.h"
#include <GameFramework/CharacterMovementComponent.h>
#include "Cypher_Kaya_Attack.h"
#include "PlayerCamera.h"
#include "Enemy_Sentinel.h"
#include <Kismet/GameplayStatics.h>
#include "../CyphersGameModeBase.h"
#include "PlayerWidget.h"
#include <UMG/Public/Components/ProgressBar.h>
#include "PowerAttackDecal.h"
#include <Particles/ParticleSystemComponent.h>
#include <../Plugins/FX/Niagara/Source/Niagara/Public/NiagaraComponent.h>

UPlayerAnim::UPlayerAnim()
{
	static ConstructorHelpers::FObjectFinder<UAnimMontage> tempBasicAttackMontage(TEXT("/Script/Engine.AnimMontage'/Game/Resources/Animations/Mongtage/AM_Cypher_Kaya/AM_KayaBasicAttack.AM_KayaBasicAttack'"));
	if (tempBasicAttackMontage.Succeeded())
	{
		basicAttackAnimMontage = tempBasicAttackMontage.Object;
	}

	static ConstructorHelpers::FObjectFinder<UAnimMontage> tempDashAttackMontage(TEXT("/Script/Engine.AnimMontage'/Game/Resources/Animations/Mongtage/AM_Cypher_Kaya/AM_KayaDashAttack.AM_KayaDashAttack'"));
	if (tempDashAttackMontage.Succeeded())
	{
		dashAttackAnimMontage = tempDashAttackMontage.Object;
	}

	static ConstructorHelpers::FObjectFinder<UAnimMontage> tempGripAttackMontage(TEXT("/Script/Engine.AnimMontage'/Game/Resources/Animations/Mongtage/AM_Cypher_Kaya/AM_Kaya_Grip.AM_Kaya_Grip'"));
	if (tempGripAttackMontage.Succeeded())
	{
		gripAttackAnimMontage = tempGripAttackMontage.Object;
	}
	static ConstructorHelpers::FObjectFinder<UAnimMontage> tempPowerAttackMontage(TEXT("/Script/Engine.AnimMontage'/Game/Resources/Animations/Mongtage/AM_Cypher_Kaya/AM_KayaPowerAttack.AM_KayaPowerAttack'"));
	if (tempPowerAttackMontage.Succeeded())
	{
		powerAttackAnimMontage = tempPowerAttackMontage.Object;
	}
	static ConstructorHelpers::FObjectFinder<UAnimMontage> tempDamageMontage(TEXT("/Script/Engine.AnimMontage'/Game/Resources/Animations/Mongtage/AM_Cypher_Kaya/AM_KayaDamage.AM_KayaDamage'"));
	if (tempDamageMontage.Succeeded())
	{
		damageMontage = tempDamageMontage.Object;
	}
}

void UPlayerAnim::NativeBeginPlay()
{
	Super::NativeBeginPlay();

	APawn* pawn = TryGetPawnOwner();

	me = Cast<ACypher_Kaya>(pawn);

}

void UPlayerAnim::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (me != nullptr)
	{
		velocity = me->GetVelocity();
		forward = me->GetActorForwardVector();
		right = me->GetActorRightVector();

		dirV = FVector::DotProduct(velocity, forward);
		dirH = FVector::DotProduct(velocity, right);

		bAir = me->GetCharacterMovement()->IsFalling();
	}
}

void UPlayerAnim::AttachCamera()
{
	me->AttachCameraActor();
	me->beforeActCameraPos = me->Camera->GetActorLocation();
	//공격이 끝난 시점 카메라 상대좌표를 가져온다
	UE_LOG(LogTemp, Warning, TEXT("CameraActorComponent->GetComponentLocation() : %s / Camera->GetActorLocation() : %s"), *me->CameraActorComponent->GetComponentLocation().ToString(), *me->Camera->GetActorLocation().ToString())
		//카메라 액터가 ChildActorComponent까지 가기 위해 움직여야 하는 거리
		FVector moveCameraRange = me->CameraActorComponent->GetComponentLocation() - me->Camera->GetActorLocation();
	//카메라 액터의 목적지 월드 위치
	me->afterActCameraPos = me->Camera->GetActorLocation() + moveCameraRange;
	//카메라 액터의 회전값을 캐릭터 ChildActorComponent회전값이랑 일치
	me->beforeActCameraRot = me->Camera->GetActorRotation();
	me->afterActCameraRot = me->CameraActorComponent->GetComponentRotation();
	UE_LOG(LogTemp, Warning, TEXT("me->afterActCameraPos : %s"), *me->afterActCameraPos.ToString())
		me->compKayaAttack->bBackCameraOringinPos = true;
	//me->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
}

void UPlayerAnim::BasicAttackMontageSection(int32 NewSection)
{
	ABCHECK(Montage_IsPlaying(basicAttackAnimMontage))
		Montage_JumpToSection(GetAttackMontageSectionName(NewSection), basicAttackAnimMontage);
}

void UPlayerAnim::AnimNotify_AttackHitCheck()
{
	OnAttackHitCheck.Broadcast();
}

void UPlayerAnim::AnimNotify_NextAttackCheck()
{
	OnNextAttackCheck.Broadcast();
}

void UPlayerAnim::AnimNotify_PowerAttackCombo1()
{
	me->compNiagra->Activate(true);
	me->powerAttackColl->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	/*FHitResult HitResult;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(me);
	bool bResult = me->GetWorld()->SweepSingleByChannel(
		HitResult,
		me->GetActorLocation(),
		me->GetActorLocation() + me->GetActorForwardVector() * powerAttackRange,
		FQuat::Identity,
		ECollisionChannel::ECC_GameTraceChannel2,
		FCollisionShape::MakeSphere(powerAttackRange),
		Params
	);

#if ENABLE_DRAW_DEBUG

	FVector TraceVec = me->GetActorForwardVector() * powerAttackRange;
	FVector Center = me->GetActorLocation() + TraceVec * 0.5f;
	float HalfHeight = powerAttackRange * 0.5f + powerAttackRadius;
	FQuat CapsuleRot = FRotationMatrix::MakeFromZ(TraceVec).ToQuat();
	FColor DrawColor = bResult ? FColor::Green : FColor::Red;
	float DebugLifeTime = 5.0f;

	DrawDebugCapsule(
		GetWorld(),
		Center,
		HalfHeight,
		powerAttackRadius,
		CapsuleRot,
		DrawColor,
		false,
		DebugLifeTime
	);
#endif
	AActor* hitActor = HitResult.GetActor();
	AEnemy_Sentinel* sentinel = Cast<AEnemy_Sentinel>(hitActor);
	if (bResult)
	{

		if (sentinel != nullptr)
		{
			sentinel->ReceiveDamage();
		}
	}*/
}

void UPlayerAnim::AnimNotify_PowerAttackCombo2()
{
	me->compNiagra->Activate(true);
	/*me->powerAttackColl->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	me->powerAttackColl->SetCollisionEnabled(ECollisionEnabled::QueryOnly);*/

}

void UPlayerAnim::AnimNotify_PowerAttackCombo3()
{
	me->compNiagra->Activate(true);
	//me->powerAttackColl->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	//me->powerAttackColl->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

}

void UPlayerAnim::AnimNotify_PowerAttackCombo4()
{
	me->compNiagra->Activate(true);
	//me->powerAttackColl->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	//me->powerAttackColl->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

}

void UPlayerAnim::AnimNotify_PowerAttackCombo5()
{
	me->compNiagra->Activate(true);
	//me->powerAttackColl->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	//me->powerAttackColl->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

}

void UPlayerAnim::AnimNotify_PowerAttackCombo6()
{
	me->compNiagra->Activate(true);
}


void UPlayerAnim::AnimNotify_PowerAttackCombo7()
{
	me->compNiagra->Activate(true);
	/*me->powerAttackColl->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	me->powerAttackColl->SetCollisionEnabled(ECollisionEnabled::QueryOnly);	*/
	UE_LOG(LogTemp, Warning, TEXT("combo6!!"))
		UGameplayStatics::PlaySound2D(GetWorld(), me->powerAttackEnd);

}

void UPlayerAnim::AnimNotify_PowerAttackEnd()
{

	me->compKayaAttack->decal->bPowerAttackEnd = true;
	me->compKayaAttack->startCoolKeyE = true;
	me->compKayaAttack->currkeyECool = me->compKayaAttack->keyECool;
	me->CyphersGameMode->playerWidget->KeyECoolTimeBar->SetVisibility(ESlateVisibility::Visible);
	me->powerAttackColl->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	UE_LOG(LogTemp, Warning, TEXT("attackENd NOtify"))
		FVector MeshLocation = me->GetMesh()->GetSocketLocation("Bip001");
	FVector currLocation = me->GetActorLocation();

	me->SetActorLocation(FVector(MeshLocation.X, MeshLocation.Y, currLocation.Z));

	AttachCamera();
}

void UPlayerAnim::AnimNotify_PAEndEffect()
{
	me->PlayPowerAttackGroundSound();
	UParticleSystemComponent* pae = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), me->powerAttackEndEffect, me->footPos->GetComponentLocation(), me->GetActorRotation(), true, EPSCPoolMethod::AutoRelease);
	pae->SetRelativeScale3D(FVector(paeScale));
}

void UPlayerAnim::AnimNotify_DashAttackCheck()
{
	OnDashAttackHitCheck.Broadcast();
}

FName UPlayerAnim::GetAttackMontageSectionName(int32 Section)
{
	ABCHECK(FMath::IsWithinInclusive<int32>(Section, 1, 3), NAME_None);
	return FName(*FString::Printf(TEXT("BasicAttack%d"), Section));
}

void UPlayerAnim::BasicAttackPlayAnim()
{
	Montage_Play(basicAttackAnimMontage, 1.0f);
}

void UPlayerAnim::DashAttackPlayAnim()
{
	Montage_Play(dashAttackAnimMontage, 1.0f);
}

void UPlayerAnim::GripAttackPlayAnim()
{
	Montage_Play(gripAttackAnimMontage, 1.0f);
}

void UPlayerAnim::PowerAttackReadyAnim()
{
	Montage_Play(powerAttackAnimMontage, 1.0f);
}


void UPlayerAnim::PowerAttackPlayAnim()
{
	Montage_JumpToSection(TEXT("PowerAttackPlay"), powerAttackAnimMontage);
}

void UPlayerAnim::AnimNotify_BasicAttack1Sound()
{
	UGameplayStatics::PlaySound2D(GetWorld(), me->swing1);
}

void UPlayerAnim::AnimNotify_BasicAttack2Sound()
{
	UGameplayStatics::PlaySound2D(GetWorld(), me->swing2);
}

void UPlayerAnim::AnimNotify_BasicAttack3Sound()
{
	UGameplayStatics::PlaySound2D(GetWorld(), me->swing3);
}

//void UPlayerAnim::AnimNotify_DashAttackSound()
//{
//
//}

void UPlayerAnim::DamagePlayAnim()
{
	Montage_Play(damageMontage, 1.0f);
}

void UPlayerAnim::DiePlayAnim()
{
	me->PlayAnimMontage(damageMontage, 1, TEXT("Die"));
}
