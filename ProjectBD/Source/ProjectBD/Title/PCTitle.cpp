// Fill out your copyright notice in the Description page of Project Settings.

#include "PCTitle.h"
#include "Title/TitleWidgetBase.h"

void APCTitle::BeginPlay()
{
	Super::BeginPlay();

	//UMG ��� ����
	FStringClassReference TitleWidgetClass(TEXT("WidgetBlueprint'/Game/Blueprints/Title/UI/TItleWidget.TitleWidget_C'"));

	//UMG CDO���� ã�Ƽ� ����
	if (UClass* MyWidgetClass = TitleWidgetClass.TryLoadClass<UUserWidget>())
	{
		//UMG Ŭ������ UMG �ν��Ͻ��� ����
		UTitleWidgetBase* TitleWidget = Cast<UTitleWidgetBase>(CreateWidget<UUserWidget>(this, MyWidgetClass));
		
		//������ �ν��Ͻ��� ȭ�鿡 ����
		TitleWidget->AddToViewport();
		SetInputMode(FInputModeUIOnly());
		bShowMouseCursor = true;
	}
}
