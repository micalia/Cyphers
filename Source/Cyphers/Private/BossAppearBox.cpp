// Fill out your copyright notice in the Description page of Project Settings.


#include "BossAppearBox.h"
#include <Kismet/GameplayStatics.h>
#include "Golem.h"
#include "Cypher_Kaya.h"
#include <Components/BoxComponent.h>
#include "PlayerCamera.h"
#include "GolemFSM.h"
#include "GolemAnim.h"
#include "Cypher_Kaya_Attack.h"
#include "PlayerAnim.h"
#include "../CyphersGameModeBase.h"
#include "PlayerWidget.h"
#include <UMG/Public/Components/CanvasPanel.h>

ABossAppearBox::ABossAppearBox()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	collisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Box"));
	collisionBox->SetCollisionProfileName(TEXT("BossAppearBox"));
	collisionBox->SetBoxExtent(boxDefaultSize);
	SetRootComponent(collisionBox);
}

// Called when the game starts or when spawned
void ABossAppearBox::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogTemp, Warning, TEXT("box Beginplay!!"))
	collisionBox->OnComponentBeginOverlap.AddDynamic(this, &ABossAppearBox::OnOverlap);
	enemy = Cast<AGolem>(UGameplayStatics::GetActorOfClass(GetWorld(), AGolem::StaticClass()));
	player = Cast<ACypher_Kaya>(UGameplayStatics::GetActorOfClass(GetWorld(), ACypher_Kaya::StaticClass()));


	MoveDest = FTransform(FQuat::MakeFromEuler(FVector(enemy->GetActorRotation().Roll,
		enemy->GetActorRotation().Pitch + 5,
		enemy->GetActorRotation().Yaw + 180)),
		FVector(enemy->GetMesh()->GetSocketLocation(TEXT("Bip001-Spine1")).X,
			enemy->GetMesh()->GetSocketLocation(TEXT("Bip001-Spine1")).Y,
			enemy->GetMesh()->GetSocketLocation(TEXT("Bip001-Spine1")).Z + cameraFocusHeight) + (enemy->GetActorForwardVector() * cameraFocusDistanceToBoss),
		FVector(1));

	gameMode = Cast<ACyphersGameModeBase>(UGameplayStatics::GetGameMode(GetWorld()));
}

// Called every frame
void ABossAppearBox::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (bStart == true) {
		currTime += DeltaTime;
		alpha = currTime / moveTime;
		if (alpha < 1) {
			player->Camera->SetActorLocation(FMath::Lerp(cameraOrigin.GetLocation(), MoveDest.GetLocation(), alpha));
			player->Camera->SetActorRotation(FMath::Lerp(cameraOrigin.GetRotation(), MoveDest.GetRotation(), alpha));
		}
		else {	
			if (bAnimOn == false) {
				enemy->fsm->mState = EGolemState::AppearBoss;
				enemy->fsm->anim->animState = EGolemState::AppearBoss;
				bAnimOn = true;
			}

			if (roarCurrTime < roarTime) {
				roarCurrTime += DeltaTime;
			}
			else { 
				backCurrTime+=DeltaTime;
				backAlpha = backCurrTime / backMoveTime;
				if (backAlpha < 1) {
					player->Camera->SetActorLocation(FMath::Lerp(MoveDest.GetLocation(), player->CameraActorComponent->GetComponentLocation(), backAlpha));
					player->Camera->SetActorRotation(FMath::Lerp(MoveDest.GetRotation(), FQuat(player->CameraActorComponent->GetComponentRotation()), backAlpha));
				}
				else {
					player->Camera->SetActorLocation(FMath::Lerp(MoveDest.GetLocation(), player->CameraActorComponent->GetComponentLocation(), 1.0f));
					player->Camera->SetActorRotation(FMath::Lerp(MoveDest.GetRotation(), FQuat(player->CameraActorComponent->GetComponentRotation()), 1.0f));
					gameMode->ShowUI();
					player->compKayaAttack->bBackCameraOringinPos = false;
				player->AttachCameraActor();
					/*APlayerController* controller = GetWorld()->GetFirstPlayerController();
					player->EnableInput(controller);*/

					gameMode->playerWidget->BossUI->SetRenderOpacity(1);
					enemy->bossAppear = true;
					enemy->fsm->mState = EGolemState::Idle;
					enemy->fsm->anim->animState = EGolemState::Idle;
				
					Destroy();
				}
			}
		}
	}
}

void ABossAppearBox::OnOverlap(UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{
	ACypher_Kaya* kaya = Cast<ACypher_Kaya>(OtherActor);
	if (kaya) {
		UE_LOG(LogTemp, Warning, TEXT("Hit box !!!!!!"))
		/*APlayerController* controller = GetWorld()->GetFirstPlayerController();
		kaya->DisableInput(controller);*/

		gameMode->HideUI();
		kaya->compKayaAttack->bBackCameraOringinPos = true;
		kaya->bCameraPosFix = true;
		bStart = true;
		kaya->DetachCameraActor();
		cameraOrigin = kaya->Camera->GetActorTransform();
	}
}

