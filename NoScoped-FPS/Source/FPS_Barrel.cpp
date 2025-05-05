// Fill out your copyright notice in the Description page of Project Settings.


#include "FPS_Barrel.h"
#include "Components/SphereComponent.h"
#include "DrawDebugHelpers.h"
#include "Components/StaticMeshComponent.h"
#include "Components/AudioComponent.h"
#include "Sound/SoundCue.h"
#include "Particles/ParticleSystemComponent.h"
#include "Particles/ParticleSystem.h"
#include "EngineUtils.h"
#include "NoScoped_FPS\Pelumi\FPS_BulletHitInterface.h"
#include "NoScoped_FPS\Pelumi\FPS_Character.h"
#include <Kismet/GameplayStatics.h>
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"

AFPS_Barrel::AFPS_Barrel() 
{
	PrimaryActorTick.bCanEverTick = true;

	m_mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BarrilMeshComponent"));
	SetRootComponent(m_mesh);

	m_sphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("OverlappingSphereComponent"));
	m_sphereComponent->SetupAttachment(GetRootComponent());
	m_sphereComponent->SetCollisionProfileName("Trigger");

	m_LeakEffectComponent = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("LeakEffectComponent"));
	m_LeakEffectComponent->SetupAttachment(GetRootComponent());

	m_readyToExplode = false;
	m_isHit = false;

	m_playerDamage = 100.0f;
	m_enemyDamage = 100.0f;
	m_hitCount = 3;
	m_hitCountDecreaseRate = 1.0f;
}

void AFPS_Barrel::BeginPlay()
{
	Super::BeginPlay();

	m_currentHitCount = m_hitCount;

	m_mesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	m_mesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);

	m_LeakEffectComponent->Deactivate();

	m_mesh->SetRenderCustomDepth(false);
}

void AFPS_Barrel::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	HandleHealthDegree (DeltaTime);
}

void AFPS_Barrel::BulletHit_Implementation(float damage, FHitResult hitResult, AActor* shooter, AController* shooterController,bool isWeakPoint, bool isCritical)
{
	if (m_currentHitCount <= 0) return;

	if (!m_isHit)
	{
		m_isHit = true;
		m_LeakEffectComponent->Activate();
	}

	m_currentHitCount--;

	if (m_hitEffect)
{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), m_hitEffect, hitResult.ImpactPoint, hitResult.ImpactNormal.Rotation());
	}

	if (!m_readyToExplode && m_currentHitCount == 0)
	{
		m_readyToExplode = true;
		Explode();
	}
}

void AFPS_Barrel::OnPositionSwapHighlightStart_Implementation()
{
	m_mesh->SetRenderCustomDepth(true);
}

void AFPS_Barrel::OnPositionSwapHighlightEnd_Implementation()
{
	m_mesh->SetRenderCustomDepth(false);
}

AActor* AFPS_Barrel::GetActor_Implementation()
{
	return  this;
}

void AFPS_Barrel::SwapPosition_Implementation(FVector location, FRotator rotation)
{
	SetActorLocationAndRotation(location, rotation);
}

void AFPS_Barrel::HandleHealthDegree(float DeltaTime)
{
	if (m_isHit && !m_readyToExplode)
	{
		m_currentHitCount -= DeltaTime * m_hitCountDecreaseRate;

		if (m_currentHitCount <= 0 && !m_readyToExplode)
		{
			m_readyToExplode = true;
			m_currentHitCount = 0;
			Explode();
		}
	}
}

void AFPS_Barrel::Explode()
{
	TArray<AActor*> m_overlappedActors;
	m_sphereComponent->GetOverlappingActors(m_overlappedActors, AActor::StaticClass());

	for (AActor* actor : m_overlappedActors)
	{

		// display actor name on screen
		//GEngine->AddOnScreenDebugMessage (-1, 5.0f, FColor::Red, actor->GetName());

		IFPS_BulletHitInterface* bulletHitInterface = Cast<IFPS_BulletHitInterface>(actor);
		if (bulletHitInterface)
		{
			// get ACharacter from OtherActor
			AController* controller = nullptr;
			FHitResult muzzleHit;
			muzzleHit.ImpactPoint = GetActorLocation();
			muzzleHit.ImpactNormal = GetActorForwardVector();
			muzzleHit.Location = GetActorLocation();
			muzzleHit.Actor = actor;

			bulletHitInterface->BulletHit_Implementation(m_enemyDamage, muzzleHit, this, controller,false, true);
		}

		AFPS_Character* character = Cast<AFPS_Character>(actor);
		if (character)
		{
			character->TakeDamage(m_playerDamage, FDamageEvent(), GetInstigatorController(), this);
		}
	}

	if (m_explosionEffect)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), m_explosionEffect, GetActorLocation());
	}

	if (m_explosionSound)
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), m_explosionSound, GetActorLocation());
	}

	Destroy();
}