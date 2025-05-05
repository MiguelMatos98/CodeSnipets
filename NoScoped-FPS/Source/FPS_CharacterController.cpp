// Fill out your copyright notice in the Description page of Project Settings.


#include "FPS_CharacterController.h"
#include "FPS_SettingsUW.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/UserWidget.h"

AFPS_CharacterController::AFPS_CharacterController()
{

}

void AFPS_CharacterController::BeginPlay()
{
	Super::BeginPlay();

	if (m_MainSettingsBP)
	{
		m_MainSettingsMenu = CreateWidget<UFPS_SettingsUW>(GetWorld(), m_MainSettingsBP);
	}
	
	if (m_MainSettingsMenu)
	{
		m_MainSettingsMenu->DisplaySettings();
	}


	bShowMouseCursor = true;
}
