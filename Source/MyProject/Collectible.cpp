#include "Collectible.h"

#include "MyProjectCharacter.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"

ACollectible::ACollectible()
{
	PrimaryActorTick.bCanEverTick = true;

	//create the collider and mesh components and setup attachment
	ColliderComponent = CreateDefaultSubobject<USphereComponent>(TEXT("Collider Component"));
	SetRootComponent(ColliderComponent);
	ColliderComponent->SetGenerateOverlapEvents(true);
	ColliderComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	ColliderComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	ColliderComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	ColliderComponent->OnComponentBeginOverlap.AddDynamic(this, &ACollectible::OnBeginOverlapComponentEvent);

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh Component"));
	MeshComponent->SetupAttachment(ColliderComponent);
	MeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	MeshComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	MeshComponent->SetGenerateOverlapEvents(false);

}

void ACollectible::OnBeginOverlapComponentEvent(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//check if player collided
	if (auto Ch = Cast<AMyProjectCharacter>(OtherActor))
	{
		Ch->SetCollectibles(this);
		UGameplayStatics::PlaySound2D(GetWorld(), Sound, 0.6f);
		Cast<AMyProjectCharacter>(OtherActor)->CollectibleCollided();
		Destroy();
	}
}


