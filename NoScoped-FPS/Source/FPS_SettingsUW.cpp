// Fill out your copyright notice in the Description page of Project Settings.


#include "FPS_SettingsUW.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "FPS_CharacterController.h"
#include "GameFramework/GameUserSettings.h"
#include "Components/Slider.h"
#include "Components/Button.h"
#include "GameFramework/InputSettings.h"
#include "Framework/Commands/InputChord.h"
#include "GameFramework/PlayerInput.h"

void UFPS_SettingsUW::DisplaySettings()
{
	AddToViewport();
}

void UFPS_SettingsUW::SetResolution(int64 width, int64 hight)
{
	srand(time(nullptr));

	m_UserSettings = GetGameUserSettings();

	if (m_UserSettings != nullptr)
	{
		if (width && hight)
		{
			m_UserSettings->RequestResolutionChange(width, hight, EWindowMode::Type::Windowed, false);
			UE_LOG(LogTemp, Warning, TEXT("Screen Resolution has changed"));
		}

		int resGenerator = rand() % 5 + 1;

		if (resGenerator == 1)
		{
			m_UserSettings->RequestResolutionChange(1280, 720, EWindowMode::Type::Windowed, false);
			UE_LOG(LogTemp, Warning, TEXT("Screen Resolution is 1280x720"));
		}

		if (resGenerator == 2)
		{
			m_UserSettings->RequestResolutionChange(1920, 1080, EWindowMode::Type::Windowed, false);
			UE_LOG(LogTemp, Warning, TEXT("Screen Resolution is 1920x1080"));
		}

		if (resGenerator == 3)
		{
			m_UserSettings->RequestResolutionChange(2560, 1440, EWindowMode::Type::Windowed, false);
			UE_LOG(LogTemp, Warning, TEXT("Screen Resolution is 2560x1440"));
		}


		if (resGenerator == 4)
		{
			m_UserSettings->RequestResolutionChange(3840, 2160, EWindowMode::Type::Windowed, false);
			UE_LOG(LogTemp, Warning, TEXT("Screen Resolution is 3840x2160"));
		}


		if (resGenerator == 5)
		{
			m_UserSettings->RequestResolutionChange(7680, 4320, EWindowMode::Type::Windowed, false);
			UE_LOG(LogTemp, Warning, TEXT("Screen Resolution is 7680x4320"));
		}

		UE_LOG(LogTemp, Warning, TEXT("Screen Resolution has changed again"));

		m_UserSettings->ScalabilityQuality.AntiAliasingQuality = 3;
		m_UserSettings->ScalabilityQuality.ResolutionQuality = 3;

		m_UserSettings->ApplyNonResolutionSettings();
		m_UserSettings->SaveSettings();
	}


	
}

void UFPS_SettingsUW::SetKeyBind(FName name, FString newKey)
{
	TArray<FInputActionKeyMapping> keyMapping;
	m_Settings = const_cast<UInputSettings*>(GetDefault<UInputSettings>());
	if (m_Settings)
	{
		m_Settings->GetActionMappingByName(name, keyMapping);
		for (auto& key : keyMapping)
		{ 
			if (name != key.ActionName)
			{
				UE_LOG(LogTemp, Warning, TEXT("Key search is ambiguous, try to check your input under project settings"));
			}

			if (key.Key.GetMenuCategory() == FKey(*newKey).GetMenuCategory())
			{
				UE_LOG(LogTemp, Error, TEXT("Key Removed: %s"), *key.Key.GetFName().ToString());
				UE_LOG(LogTemp, Warning, TEXT("Key Removed: %s"), *FKey(*newKey).GetFName().ToString());
				m_Settings->RemoveActionMapping(FInputActionKeyMapping(name, key.Key));
				m_Settings->AddActionMapping(FInputActionKeyMapping(name, FKey(*newKey)));
				m_Settings->ForceRebuildKeymaps();
				m_Settings->SaveKeyMappings();
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("Key that was updated doesn't bellong to a the category Keyboard."));
			}
		}
	}
	else
	UE_LOG(LogTemp, Warning, TEXT("No input settings found"));
}

//void UFPS_SettingsUW::SetKeyBind(FName name, FString oldKey, FString newKey)
//{
//	TArray<FInputActionKeyMapping> keyMapping;
//
//	TArray<FInputActionKeyMapping> keyMappingInsance;
//
//	//TMap<FKey&, FKey&> keyDirectory;
//
//	m_PlayerInput = Cast<AFPS_CharacterController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
//
//	if (m_PlayerInput)
//	{
//		keyMapping = m_PlayerInput->PlayerInput->GetKeysForAction(name);
//
//		UE_LOG(LogTemp, Warning, TEXT("Action Key initialized"));
//	}
//	else
//		UE_LOG(LogTemp, Warning, TEXT("Action Key uninitialized"));
//
//	//const FInputActionKeyMapping jumpKey(FName("Jump"), key.Key, 1); // Binding a key with an action
//	m_Settings = const_cast<UInputSettings*>(GetDefault<UInputSettings>());
//	if (m_Settings)
//	{
//		for (auto& key : keyMapping)
//		{
//			if (oldKey == key.Key.GetDisplayName().ToString())
//			{
//				UE_LOG(LogTemp, Warning, TEXT("Old key exist in mapping"));
//				m_Settings->RemoveActionMapping(key);
//			}
//			else
//				UE_LOG(LogTemp, Warning, TEXT("Old key doesn't exist in mapping"));
//		}
//
//		for (auto& key: keyMapping)
//		{
//			if (key.Key == FKey(*oldKey))
//			{
//				m_Settings->RemoveActionMapping(FInputActionKeyMapping(name, FKey(*oldKey)));
//				m_Settings->ForceRebuildKeymaps();
//				m_Settings->SaveKeyMappings();
//				UE_LOG(LogTemp, Warning, TEXT("Action Key exist and is removed"));
//			}
//			FText someText = key.Key.GetDisplayName();
//			FString someString = someText.ToString();
//			UE_LOG(LogTemp, Error, TEXT("All Key Names: %s"), *someString);
//		}
//		
//		m_Settings->RemoveActionMapping(FInputActionKeyMapping(name, FKey(*oldKey)));
//		m_Settings->AddActionMapping(FInputActionKeyMapping(name, FKey(*newKey)));
//		m_Settings->ForceRebuildKeymaps();
//
//		UE_LOG(LogTemp, Warning, TEXT("Action Key iterated"));
//	}
//	else
//		UE_LOG(LogTemp, Warning, TEXT("Action Key not iterated"));
//	
//	m_Settings->SaveKeyMappings();
//}

void UFPS_SettingsUW::SetFrameRate(float newFrames)
{
	GEngine->bSmoothFrameRate = true;
	GEngine->SmoothedFrameRateRange = FFloatRange(30.0f, newFrames);
	UE_LOG(LogTemp, Warning, TEXT("Frames: %f"), newFrames);
}

void UFPS_SettingsUW::ChangeSliders(USlider* sliderToChange, FName sliderName,float amount)
{
	if (sliderToChange->GetFName() == sliderName)
	{
		sliderToChange->Value = amount / sliderToChange->MaxValue;
	}
}

UGameUserSettings* UFPS_SettingsUW::GetGameUserSettings()
{
	if (GEngine != nullptr)
	{
		return GEngine->GameUserSettings;
	}
	return nullptr;
}
