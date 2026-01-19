// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "FPS_SettingsUW.generated.h"

/**
 * 
 */
UCLASS()
class NOSCOPED_FPS_API UFPS_SettingsUW : public UUserWidget
{
	GENERATED_BODY()
	
private:

	UPROPERTY()
	class UGameUserSettings* m_UserSettings;
	
	UPROPERTY()
	class UInputSettings* m_Settings;

	UPROPERTY()
	class AFPS_CharacterController* m_PlayerInput;
public:

	UFUNCTION(BlueprintCallable)
	void DisplaySettings();

	UFUNCTION(BlueprintCallable)
	void SetResolution(int64 width, int64 hight);

	UFUNCTION(BlueprintCallable)
	void SetKeyBind(FName name, FString newKey);

	UFUNCTION(BlueprintCallable)
	void SetFrameRate(float newFrames);

	UFUNCTION(BlueprintCallable)
	void ChangeSliders(class USlider* sliderToChange, FName sliderName, float amount);

	UFUNCTION()
	UGameUserSettings* GetGameUserSettings();

	
};
