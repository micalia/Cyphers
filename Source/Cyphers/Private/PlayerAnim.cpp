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
#include "PlayerMoveInput.h"

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
	static ConstructorHelpers::FObjectFinder<UAnimMontage> tempDashFowardMontage(TEXT("/Script/Engine.AnimMontage'/Game/Resources/Animations/Mongtage/AM_Cypher_Kaya/AM_KayaDash.AM_KayaDash'"));
	if (tempDashFowardMontage.Succeeded())
	{
		dashMontage = tempDashFowardMontage.Object;
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
	//������ ���� ���� ī�޶� �����ǥ�� �����´�
		//ī�޶� ���Ͱ� ChildActorComponent���� ���� ���� �������� �ϴ� �Ÿ�
	FVector moveCameraRange = me->CameraActorComponent->GetComponentLocation() - me->Camera->GetActorLocation();
	//ī�޶� ������ ������ ���� ��ġ
	me->afterActCameraPos = me->Camera->GetActorLocation() + moveCameraRange;
	//ī�޶� ������ ȸ������ ĳ���� ChildActorComponentȸ�����̶� ��ġ
	me->beforeActCameraRot = me->Camera->GetActorRotation();
	me->afterActCameraRot = me->CameraActorComponent->GetComponentRotation();
	me->compKayaAttack->bBackCameraOringinPos = true;
}

void UPlayerAnim::AnimNotify_LeftFoot()
{
	UGameplayStatics::PlaySound2D(GetWorld(), me->leftFootSound);
}

void UPlayerAnim::AnimNotify_RightFoot()
{
	UGameplayStatics::PlaySound2D(GetWorld(), me->rightFootSound);
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

void UPlayerAnim::AnimNotify_GripAttackCheck()
{
	me->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	OnGripAttackCheck.Broadcast();
}

void UPlayerAnim::AnimNotify_GripAttackCheck2()
{
	me->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	OnGripAttackCheck2.Broadcast();
}

void UPlayerAnim::AnimNotify_PowerAttackCombo1()
{
	me->compNiagra->Activate(true);
	me->powerAttackColl->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

	UGameplayStatics::PlaySound2D(GetWorld(), me->powerAttackComboSound);
}

void UPlayerAnim::AnimNotify_PowerAttackCombo2()
{
	me->compNiagra->Activate(true);
}

void UPlayerAnim::AnimNotify_PowerAttackCombo3()
{
	me->compNiagra->Activate(true);
}

void UPlayerAnim::AnimNotify_PowerAttackCombo4()
{
	me->compNiagra->Activate(true);
}

void UPlayerAnim::AnimNotify_PowerAttackCombo5()
{
	me->compNiagra->Activate(true);
}

void UPlayerAnim::AnimNotify_PowerAttackCombo6()
{
	me->compNiagra->Activate(true);
}

void UPlayerAnim::AnimNotify_PowerAttackCombo7()
{
	me->compNiagra->Activate(true);
	UGameplayStatics::PlaySound2D(GetWorld(), me->powerAttackEnd);
}

void UPlayerAnim::AnimNotify_PowerAttackEnd()
{
	me->compKayaAttack->decal->bPowerAttackEnd = true;
	me->compKayaAttack->startCoolKeyE = true;
	me->compKayaAttack->currkeyECool = me->compKayaAttack->keyECool;
	if (me->CyphersGameMode) {
		me->CyphersGameMode->playerWidget->KeyECoolTimeBar->SetVisibility(ESlateVisibility::Visible);
	}
	me->powerAttackColl->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	FVector MeshLocation = me->GetMesh()->GetSocketLocation("Bip001");
	FVector currLocation = me->GetActorLocation();

	me->SetActorLocation(FVector(MeshLocation.X, MeshLocation.Y, currLocation.Z));

	AttachCamera();
}

void UPlayerAnim::AnimNotify_PAEndEffect()
{
	me->Camera->bSkillCamMove = false;
	me->PlayPowerAttackGroundSound();
	UParticleSystemComponent* pae = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), me->powerAttackEndEffect, me->footPos->GetComponentLocation(), me->GetActorRotation(), true, EPSCPoolMethod::AutoRelease);
	pae->SetRelativeScale3D(FVector(paeScale * 3));
	me->Camera->CameraMoveDelegate.ExecuteIfBound();
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

void UPlayerAnim::PlayDashAnim()
{
	me->PlayDashSound();

	if (me->compKayaAttack->dashHorizontal == -1) {
		me->PlayAnimMontage(dashMontage, 1, TEXT("DashFL45"));
	}
	else if (me->compKayaAttack->dashHorizontal == 1) {
		me->PlayAnimMontage(dashMontage, 1, TEXT("DashFR45"));
	}
	else {
		me->PlayAnimMontage(dashMontage, 1, TEXT("DashFront"));
	}
}

void UPlayerAnim::AnimNotify_NextDashCheck()
{
	OnNextDashCheck.Broadcast();
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

void UPlayerAnim::AnimNotify_AfterImage()
{
	me->compNiagra->Activate(true);
}

void UPlayerAnim::AnimNotify_GaUpAtkSound()
{
	me->PlayGaUpAtkSound();
}

void UPlayerAnim::AnimNotify_GaDownAtkSound()
{
	me->PlayGaDownAtkSound();
}

void UPlayerAnim::DamagePlayAnim()
{
	Montage_Play(damageMontage, 1.0f);
}

void UPlayerAnim::DiePlayAnim()
{
	me->PlayAnimMontage(damageMontage, 1, TEXT("Die"));
}

void UPlayerAnim::KnockBackFlyAnim()
{
	me->PlayAnimMontage(damageMontage, 1, TEXT("FlyingDamage"));
}

void UPlayerAnim::RiseAnim()
{
	me->PlayAnimMontage(damageMontage, 1, TEXT("Rise"));
}

