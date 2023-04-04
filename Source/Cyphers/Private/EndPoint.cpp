// Fill out your copyright notice in the Description page of Project Settings.


#include "EndPoint.h"
#include <../Plugins/FX/Niagara/Source/Niagara/Public/NiagaraComponent.h>
#include <Components/CapsuleComponent.h>
#include "../CyphersGameModeBase.h"
#include "WhiteScreen.h"

// Sets default values
AEndPoint::AEndPoint()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	compCapsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("compCapsule"));
	SetRootComponent(compCapsule);
	compCapsule->SetCollisionProfileName(TEXT("EndPoint"));
	compCapsule->SetCapsuleRadius(127);
	compCapsule->SetRelativeLocation(FVector(0,0, -130));

	compNiagra = CreateDefaultSubobject<UNiagaraComponent>(TEXT("EndPointEffect"));
	compNiagra->SetupAttachment(compCapsule);

	static ConstructorHelpers::FObjectFinder<UNiagaraSystem> tempNiagraSystem(TEXT("/Script/Niagara.NiagaraSystem'/Game/Resources/Effect/VFX_45/Effect/P_Recall_Effect08.P_Recall_Effect08'"));
	if (tempNiagraSystem.Succeeded()) {
		NiagaraSystemAsset = tempNiagraSystem.Object;
	}

	compNiagra->SetAsset(NiagaraSystemAsset);
}

// Called when the game starts or when spawned
void AEndPoint::BeginPlay()
{
	Super::BeginPlay();
	
	compCapsule->OnComponentBeginOverlap.AddDynamic(this, &AEndPoint::OnOverlap);

}

// Called every frame
void AEndPoint::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AEndPoint::OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{ 
	UE_LOG(LogTemp, Warning, TEXT("Overlap!!!!!!!!!!!"))
	ACyphersGameModeBase* gamemode = Cast<ACyphersGameModeBase>(GetWorld()->GetAuthGameMode());
	gamemode->whiteScreen->SetVisibility(ESlateVisibility::Visible);
	gamemode->OpacityOnCheck = true;
	gamemode->ActiveWhiteScreenOpacity = true;
}

