// Fill out your copyright notice in the Description page of Project Settings.

#include "PCTitle.h"
#include "Title/TitleWidgetBase.h"

void APCTitle::BeginPlay()
{
	Super::BeginPlay();

	//UMG 경로 세팅
	FStringClassReference TitleWidgetClass(TEXT("WidgetBlueprint'/Game/Blueprints/Title/UI/TItleWidget.TitleWidget_C'"));

	//UMG CDO에서 찾아서 생성
	if (UClass* MyWidgetClass = TitleWidgetClass.TryLoadClass<UUserWidget>())
	{
		//UMG 클래스를 UMG 인스턴스로 생성
		UTitleWidgetBase* TitleWidget = Cast<UTitleWidgetBase>(CreateWidget<UUserWidget>(this, MyWidgetClass));
		
		//생성된 인스턴스를 화면에 붙임
		TitleWidget->AddToViewport();
		SetInputMode(FInputModeUIOnly());
		bShowMouseCursor = true;
	}
}
