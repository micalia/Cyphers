#include "PlayerBaseComponent.h"
#include "Cypher_Kaya.h"

UPlayerBaseComponent::UPlayerBaseComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}


void UPlayerBaseComponent::BeginPlay()
{
	Super::BeginPlay();

	me = Cast<ACypher_Kaya>(GetOwner());
	moveComp = me->GetCharacterMovement();
}

void UPlayerBaseComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

void UPlayerBaseComponent::SetupInputBinding(class UInputComponent* PlayerInputComponent)
{

}

