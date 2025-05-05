// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "FPS_CharacterController.generated.h"

/**
 * 
 */
UCLASS()
class NOSCOPED_FPS_API AFPS_CharacterController : public APlayerController
{
	GENERATED_BODY()

private:

	UPROPERTY(VisibleAnywhere)
	class UFPS_SettingsUW* m_MainSettingsMenu;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UFPS_SettingsUW> m_MainSettingsBP;

public:

	AFPS_CharacterController();

	virtual void BeginPlay() override;
};
