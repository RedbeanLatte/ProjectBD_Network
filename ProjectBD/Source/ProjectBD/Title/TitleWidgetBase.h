// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TitleWidgetBase.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTBD_API UTitleWidgetBase : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual void NativeConstruct() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UButton* LoginButton;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UButton* ConnectButton;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UButton* MakeRoomButton;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UEditableTextBox* UserID;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UEditableTextBox* Password;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UEditableTextBox* ServerIP;
	
	UFUNCTION()
	void Login();

	UFUNCTION()
	void Connect();

	UFUNCTION()
	void MakeRoom();

	UFUNCTION()
	void SetUserID();
};
