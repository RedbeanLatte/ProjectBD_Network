// Fill out your copyright notice in the Description page of Project Settings.

#include "TitleWidgetBase.h"
#include "Components/Button.h"
#include "Components/EditableTextBox.h"
#include "Kismet/GameplayStatics.h"
#include "Local/BDGameInstance.h"

void UTitleWidgetBase::NativeConstruct()
{
	UserID = Cast<UEditableTextBox>(GetWidgetFromName(TEXT("UserID")));
	Password = Cast<UEditableTextBox>(GetWidgetFromName(TEXT("Password")));
	ServerIP = Cast<UEditableTextBox>(GetWidgetFromName(TEXT("ServerIP")));

	LoginButton = Cast<UButton>(GetWidgetFromName(TEXT("LoginButton")));
	ConnectButton = Cast<UButton>(GetWidgetFromName(TEXT("ConnectButton")));
	MakeRoomButton = Cast<UButton>(GetWidgetFromName(TEXT("MakeRoomButton")));

	if (LoginButton)
	{
		LoginButton->OnClicked.AddDynamic(this, &UTitleWidgetBase::Login);
	}

	if (ConnectButton)
	{
		ConnectButton->OnClicked.AddDynamic(this, &UTitleWidgetBase::Connect);
	}

	if (MakeRoomButton)
	{
		MakeRoomButton->OnClicked.AddDynamic(this, &UTitleWidgetBase::MakeRoom);
	}
}

void UTitleWidgetBase::Login()
{
}

void UTitleWidgetBase::Connect()
{
	if (ServerIP && !ServerIP->GetText().IsEmpty())
	{
		FString Temp = ServerIP->GetText().ToString();
		UGameplayStatics::OpenLevel(GetWorld(), FName(*Temp));
	}
}

void UTitleWidgetBase::MakeRoom()
{
	SetUserID();
	UGameplayStatics::OpenLevel(GetWorld(), TEXT("Lobby"), true, TEXT("listen"));
}

void UTitleWidgetBase::SetUserID()
{
	UBDGameInstance* GI = Cast<UBDGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	if (GI && UserID)
	{
		if (UserID->GetText().IsEmpty())
		{
			GI->UserID = FString::Printf(TEXT("Noname%d"), FMath::RandRange(0, 1000));
		}
		else
		{
			GI->UserID = UserID->GetText().ToString();
		}
	}
}
