// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NoScoped_FPS\Pelumi\FPS_BulletHitInterface.h"
#include "NoScoped_FPS\Pelumi\FPS_PositionSwapInterface.h"
#include "FPS_Barrel.generated.h"

UCLASS()
class NOSCOPED_FPS_API AFPS_Barrel : public AActor, public IFPS_BulletHitInterface, public IFPS_PositionSwapInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AFPS_Barrel();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void BulletHit_Implementation(float damage, FHitResult hitResult, AActor* shooter, AController* shooterController,bool isWeakPoint, bool isCritical) override;

	virtual void OnPositionSwapHighlightStart_Implementation() override;
	virtual void OnPositionSwapHighlightEnd_Implementation() override;
	virtual AActor* GetActor_Implementation() override;
	virtual void SwapPosition_Implementation(FVector location, FRotator rotation) override;

	void HandleHealthDegree(float DeltaTime);

	UFUNCTION()
	void Explode();

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Properties, meta = (AllowPrivateAccess = "true"))
	float m_playerDamage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Properties, meta = (AllowPrivateAccess = "true"))
	float m_enemyDamage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Properties, meta = (AllowPrivateAccess = "true"))
	float m_hitCount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Properties, meta = (AllowPrivateAccess = "true"))
	float m_hitCountDecreaseRate;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Properties, meta = (AllowPrivateAccess = "true"))
	float m_currentHitCount;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Properties, meta = (AllowPrivateAccess = "true"))
	bool m_isHit;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Properties, meta = (AllowPrivateAccess = "true"))
	bool m_readyToExplode;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Properties, meta = (AllowPrivateAccess = "true"))
	class USphereComponent* m_sphereComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Properties, meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* m_mesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Properties, meta = (AllowPrivateAccess = "true"))
	class UParticleSystemComponent* m_LeakEffectComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Properties, meta = (AllowPrivateAccess = "true"))
	class UNiagaraSystem* m_niagaraSystem;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Properties, meta = (AllowPrivateAccess = "true"))
	class UParticleSystem* m_explosionEffect;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Properties, meta = (AllowPrivateAccess = "true"))
	class UParticleSystem* m_hitEffect;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Properties, meta = (AllowPrivateAccess = "true"))
	class USoundBase* m_explosionSound;
};
