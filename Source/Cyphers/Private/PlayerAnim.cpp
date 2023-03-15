#include "PlayerAnim.h"
#include "Cypher_Kaya.h"
#include <GameFramework/CharacterMovementComponent.h>
#include "Cypher_Kaya_Attack.h"

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

void UPlayerAnim::AnimNotify_PowerAttackStart()
{
	UE_LOG(LogTemp, Warning, TEXT("attackstart NOtify"))
		FVector MeshLocation = me->GetMesh()->GetSocketLocation("Bip001");
	UE_LOG(LogTemp, Warning, TEXT("MeshLocation : %s"), *MeshLocation.ToString())
	me->DetachCameraActor();
	//me->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void UPlayerAnim::AnimNotify_PowerAttackEnd()
{
	UE_LOG(LogTemp, Warning, TEXT("attackENd NOtify"))
		FVector MeshLocation = me->GetMesh()->GetSocketLocation("Bip001");
		FVector currLocation = me->GetActorLocation();
	DrawDebugSphere(GetWorld(), MeshLocation, 20.0f, 32, FColor::Blue, false, 5.0f);

	me->SetActorLocation(FVector(MeshLocation.X, MeshLocation.Y, currLocation.Z));
	
	DrawDebugSphere(GetWorld(), me->GetActorLocation(), 20.0f, 32, FColor::Red, false, 5.0f);
	UE_LOG(LogTemp, Warning, TEXT("MeshLocation : %s / me->GetActorLocation: %s"), *MeshLocation.ToString(), *me->GetActorLocation().ToString())
	me->AttachCameraActor();
	//me->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
}

void UPlayerAnim::AnimNotify_DashAttackCheck()
{
	OnDashAttackHitCheck.Broadcast();
}

FName UPlayerAnim::GetAttackMontageSectionName(int32 Section)
{
	ABCHECK(FMath::IsWithinInclusive<int32>(Section, 1, 3),NAME_None);
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
