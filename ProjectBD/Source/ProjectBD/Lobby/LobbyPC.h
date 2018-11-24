// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "LobbyPC.generated.h"

/**
 *
 */
UCLASS()
class PROJECTBD_API ALobbyPC : public APlayerController
{
	GENERATED_BODY()

public:
	ALobbyPC();

	virtual void BeginPlay() override;

	//소유중인 클라이언트에서 UI 생성
	UFUNCTION(Client, Reliable)
	void S2C_SetupWidget();
	void S2C_SetupWidget_Implementation();

	class UItemNameWidgetBase* ItemNameWidget;
	class UInventoryWidgetBase* InventoryWidget;

	void ShowItemName(FString& ItemName, bool Show = true);

	void ShowInventory();
	void UpdateSlotData();

	class ULobbyWidgetBase* LobbyWidget;
};
