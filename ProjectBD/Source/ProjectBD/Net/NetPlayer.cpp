﻿#include "NetPlayer.h"
#include "ProjectBD.h"
#include "Components/InputComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "ConstructorHelpers.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "TimerManager.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h" // Important, BP 유틸리티 함수
#include "Kismet/KismetSystemLibrary.h"
#include "Particles/ParticleSystem.h"
#include "Sound/SoundBase.h"
#include "Components/DecalComponent.h"
#include "Local/FireCameraShake.h"
#include "Local/BulletDamageType.h"
#include "Animation/AnimMontage.h"
#include "Animation/AnimInstance.h"
#include "Items/MasterItem.h"
#include "Local/LocalPC.h"
#include "Local/BDGameInstance.h"
#include "UnrealNetwork.h" // Network"

// Sets default values
ANetPlayer::ANetPlayer()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(RootComponent);

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm);

	Weapon = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Weapon"));
	Weapon->SetupAttachment(GetMesh(), TEXT("RHandWeapon"));

	// 권고 사항은 하드코딩 하지 말기, 로딩 할때 메시로딩 해서 느림, 생성자용
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_Mesh(TEXT("SkeletalMesh'/Game/Male_Grunt/Mesh/male_grunt.male_grunt'"));
	if (SK_Mesh.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(SK_Mesh.Object);
	}

	/*static ConstructorHelpers::FObjectFinder<UStaticMesh> SM_Wepon(TEXT("StaticMesh'/Game/Weapons/M4A1/SM_M4A1.SM_M4A1'"));
	if (SM_Wepon.Succeeded())
	{
		Weapon->SetStaticMesh(SM_Wepon.Object);
	}*/

	//메시의 방향과 위치 조정
	GetMesh()->SetRelativeLocation(FVector(0,
		0,
		-GetCapsuleComponent()->GetScaledCapsuleHalfHeight())
	);

	GetMesh()->SetRelativeRotation(FRotator(0, -90, 0));

	//메시에 애니메이션 블루프린트 연결, _C 추가, 생성자용
	static ConstructorHelpers::FClassFinder<UAnimInstance> ABP_MyPlayer(TEXT("AnimBlueprint'/Game/Blueprints/Local/Animations/ABP_MyPlayer.ABP_MyPlayer_C'"));
	if (ABP_MyPlayer.Succeeded())
	{
		GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);
		GetMesh()->SetAnimInstanceClass(ABP_MyPlayer.Class);
	}

	SpringArm->TargetArmLength = 150.0f;
	SpringArm->SetRelativeLocation(FVector(0, 46, 93));
	SpringArm->bUsePawnControlRotation = true;

	GetCharacterMovement()->NavAgentProps.bCanCrouch = true;

	GetCharacterMovement()->CrouchedHalfHeight = GetCapsuleComponent()->GetScaledCapsuleHalfHeight();

	GetCharacterMovement()->MaxWalkSpeed = JogSpeed;
	GetCharacterMovement()->MaxWalkSpeedCrouched = CrouchSpeed;

	//Camera 위치 저장
	NormalSpringArmPosition = SpringArm->GetRelativeTransform().GetLocation();
	CrouchSpringArmPosition = FVector(NormalSpringArmPosition.X,
		NormalSpringArmPosition.Y,
		NormalSpringArmPosition.Z - 40);

	//이펙트 로딩
	static ConstructorHelpers::FObjectFinder<UParticleSystem> P_FireEffect(
		TEXT("ParticleSystem'/Game/Effects/P_AssaultRifle_MF.P_AssaultRifle_MF'"));
	if (P_FireEffect.Succeeded())
	{
		FireEffect = P_FireEffect.Object;
	}

	static ConstructorHelpers::FObjectFinder<UParticleSystem> P_HitEffect(
		TEXT("ParticleSystem'/Game/Effects/P_AssaultRifle_IH.P_AssaultRifle_IH'"));
	if (P_HitEffect.Succeeded())
	{
		HitEffect = P_HitEffect.Object;
	}

	static ConstructorHelpers::FObjectFinder<UParticleSystem> P_BloodEffect(
		TEXT("ParticleSystem'/Game/Effects/P_body_bullet_impact.P_body_bullet_impact'"));
	if (P_BloodEffect.Succeeded())
	{
		BloodEffect = P_BloodEffect.Object;
	}

	static ConstructorHelpers::FObjectFinder<USoundBase> S_FireSound(
		TEXT("SoundCue'/Game/Sound/Weapons/SMG_Thompson/Cue_Thompson_Shot.Cue_Thompson_Shot'"));
	if (S_FireSound.Succeeded())
	{
		FireSound = S_FireSound.Object;
	}

	//Load Decal
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> M_BulletDecal(
		TEXT("MaterialInstanceConstant'/Game/Effects/Decal/M_BulletDecal_Inst.M_BulletDecal_Inst'"));
	if (M_BulletDecal.Succeeded())
	{
		BulletDecal = M_BulletDecal.Object;
	}

	Tags.Add(TEXT("Player"));

	//Anim Montage
	static ConstructorHelpers::FObjectFinder<UAnimMontage> DeadAnimation_Motage(
		TEXT("AnimMontage'/Game/Male_Grunt/Animations/Death_1_Montage.Death_1_Montage'"));
	if (DeadAnimation_Motage.Succeeded())
	{
		DeadAnimation = DeadAnimation_Motage.Object;
	}

	static ConstructorHelpers::FObjectFinder<UAnimMontage> ReloadAnimation_Motage(
		TEXT("AnimMontage'/Game/Male_Grunt/Animations/Reload_Rifle_Hip_Montage.Reload_Rifle_Hip_Montage'"));
	if (ReloadAnimation_Motage.Succeeded())
	{
		ReloadAnimation = ReloadAnimation_Motage.Object;
	}
}

// Called when the game starts or when spawned
void ANetPlayer::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void ANetPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ANetPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction(TEXT("Crouch"), IE_Pressed, this, &ANetPlayer::Crouching);
	PlayerInputComponent->BindAction(TEXT("Ironsight"), IE_Pressed, this, &ANetPlayer::DoIronsight);

	PlayerInputComponent->BindAction(TEXT("Sprint"), IE_Pressed, this, &ANetPlayer::StartSprint);
	PlayerInputComponent->BindAction(TEXT("Sprint"), IE_Released, this, &ANetPlayer::StopSprint);

	PlayerInputComponent->BindAction(TEXT("Fire"), IE_Pressed, this, &ANetPlayer::StartFire);
	PlayerInputComponent->BindAction(TEXT("Fire"), IE_Released, this, &ANetPlayer::StopFire);

	PlayerInputComponent->BindAction(TEXT("LeanLeft"), IE_Pressed, this, &ANetPlayer::StartLeanLeft);
	PlayerInputComponent->BindAction(TEXT("LeanLeft"), IE_Released, this, &ANetPlayer::StopLeanLeft);

	PlayerInputComponent->BindAction(TEXT("LeanRight"), IE_Pressed, this, &ANetPlayer::StartLeanRight);
	PlayerInputComponent->BindAction(TEXT("LeanRight"), IE_Released, this, &ANetPlayer::StopLeanRight);

	PlayerInputComponent->BindAxis(TEXT("Forward"), this, &ANetPlayer::Forward);
	PlayerInputComponent->BindAxis(TEXT("Right"), this, &ANetPlayer::Right);
	PlayerInputComponent->BindAxis(TEXT("Pitch"), this, &ANetPlayer::Pitch);
	PlayerInputComponent->BindAxis(TEXT("Yaw"), this, &ANetPlayer::Yaw);

	PlayerInputComponent->BindAction(TEXT("Reload"), IE_Pressed, this, &ANetPlayer::Reload);

	PlayerInputComponent->BindAction(TEXT("Pickup"), IE_Pressed, this, &ANetPlayer::PickupItem);

	PlayerInputComponent->BindAction(TEXT("Inventory"), IE_Pressed, this, &ANetPlayer::ShowInventory);
}

void ANetPlayer::Forward(float Value)
{
	if (Value != 0.0f)
	{
		AddMovementInput(GetActorForwardVector(), Value);
	}
}

void ANetPlayer::Right(float Value)
{
	if (Value != 0.0f && !bIsSprint)
	{
		AddMovementInput(GetActorRightVector(), Value);
	}
}

//절대 회전을 변경함
void ANetPlayer::Pitch(float Value)
{
	if (Value != 0.0f)
	{
		AddControllerPitchInput(Value);
	}
}

//절대 회전을 변경함
void ANetPlayer::Yaw(float Value)
{
	if (Value != 0.0f)
	{
		AddControllerYawInput(Value);
	}
}

void ANetPlayer::Crouching()
{
	if (bIsSprint)
	{
		return;
	}

	CanCrouch() ? Crouch() : UnCrouch();
}

void ANetPlayer::DoIronsight()
{
	C2S_DoIronsight();
}

bool ANetPlayer::C2S_DoIronsight_Validate()
{
	return true;
}

void ANetPlayer::C2S_DoIronsight_Implementation()
{
	if (bIsSprint)
	{
		return;
	}

	bIsIronsight = bIsIronsight ? false : true;

	if (bIsIronsight && !bIsCrouched)
	{
		GetCharacterMovement()->MaxWalkSpeed = IronsightSpeed;
	}
	else if (!bIsIronsight && !bIsCrouched)
	{
		GetCharacterMovement()->MaxWalkSpeed = JogSpeed;
	}
}

void ANetPlayer::StartSprint()
{
	C2S_StartSprint();
}

bool ANetPlayer::C2S_StartSprint_Validate()
{
	return true;
}

void ANetPlayer::C2S_StartSprint_Implementation()
{
	if (bIsCrouched || bIsIronsight)
	{
		return;
	}
	GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
	bIsSprint = true;
}

void ANetPlayer::StopSprint()
{
	C2S_StopSprint();
}

bool ANetPlayer::C2S_StopSprint_Validate()
{
	return true;
}

void ANetPlayer::C2S_StopSprint_Implementation()
{
	if (bIsSprint)
	{
		GetCharacterMovement()->MaxWalkSpeed = JogSpeed;
	}

	bIsSprint = false;
}

FRotator ANetPlayer::GetAimOffset() const
{
	const FVector AimDirWS = GetBaseAimRotation().Vector();
	const FVector AimDirLS = ActorToWorld().InverseTransformVectorNoScale(AimDirWS);
	const FRotator AimRotLS = AimDirLS.Rotation();
	return AimRotLS;
}

void ANetPlayer::StartFire()
{
	if (bIsSprint)
	{
		return;
	}

	bIsFire = true;
	OnShoot();
}

void ANetPlayer::StopFire()
{
	bIsFire = false;
}

void ANetPlayer::OnShoot()
{
	if (!bIsFire)
	{
		return;
	}

	//총알 계산
	//1. 카메라 위치, 회전
	FVector CameraLocation;
	FRotator CameraRotation;
	//AI or Player
	//GetController()->GetPlayerViewPoint(CameraLocation, CameraRotation);

	//Local PlayerController 가져오기
	UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetPlayerViewPoint(CameraLocation, CameraRotation);

	//2. 크로스 헤어 위치(화면 가운데 좌표)
	int SizeX;
	int SizeY;
	UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetViewportSize(SizeX, SizeY);

	//3. 화면좌표 -> World 좌표 변환, Project(3D->2D), DeProject(2D->3D)
	//화면 가운데 위치를 3차원 월드 좌표로 변환
	FVector WorldLocation;
	FVector WorldDirection;

	SizeX += FMath::RandRange(-5, 5);
	SizeY += FMath::RandRange(0, 5);

	UGameplayStatics::GetPlayerController(GetWorld(), 0)->DeprojectScreenPositionToWorld(
		SizeX / 2,
		SizeY / 2,
		WorldLocation,
		WorldDirection
	);

	//4. 선을 만들어서 충돌체크(Camera부터 화면 가운데점을 지나는 선을 늘여서 충돌 체크)
	FVector TraceStart = CameraLocation;
	FVector TraceEnd = CameraLocation + (WorldDirection * 8000000.0f);

	C2S_Fire(TraceStart, TraceEnd);

	//카메라 흔들기
	UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0)->PlayCameraShake(
	UFireCameraShake::StaticClass());

	//총구반동
	FRotator NewRotation = GetControlRotation();
	NewRotation.Pitch += 1.0f;
	NewRotation.Yaw += FMath::RandRange(-0.5f, 0.5f);
	UGameplayStatics::GetPlayerController(GetWorld(), 0)->SetControlRotation(NewRotation);

	//연사 구현
	if (bIsFire)
	{
		GetWorldTimerManager().SetTimer(ShootTimer, this, &ANetPlayer::OnShoot, 0.2f);
	}
}

bool ANetPlayer::C2S_Fire_Validate(FVector TraceStart, FVector TraceEnd)
{
	return true;
}

//클라이언트에서 시작과 충돌 끝점을 받아서 계산
void ANetPlayer::C2S_Fire_Implementation(FVector TraceStart, FVector TraceEnd)
{
	S2A_MuzzleFlashAndSound();

	//충돌할 물체 종류
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldStatic));
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldDynamic));
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_PhysicsBody));

	//충돌 제외 리스트
	TArray<AActor*> IgnoreActors;
	IgnoreActors.Add(this);

	FHitResult OutHit;

	bool bResult = UKismetSystemLibrary::LineTraceSingleForObjects(
		GetWorld(),
		TraceStart,
		TraceEnd,
		ObjectTypes,
		true,
		IgnoreActors,
		EDrawDebugTrace::ForDuration,
		OutHit,
		true,
		FLinearColor::Red,
		FLinearColor::Green,
		3.0f
	);

	// 카메라에서 화면 가운데로 충돌 처리
	if (bResult)
	{
		//총구에서 맞은데까지 진짜로 총알이 날아가면 맞는지 확인
		TraceStart = Weapon->GetSocketLocation(TEXT("MuzzleFlash"));
		FVector Dir = OutHit.ImpactPoint - TraceStart;
		TraceEnd = TraceStart + (Dir * 2.0f); //float 오차

		//총구에서 히트 포인트까지 다시 체크(중간에 방해물체가 있는지)
		bResult = UKismetSystemLibrary::LineTraceSingleForObjects(
			GetWorld(),
			TraceStart,
			TraceEnd,
			ObjectTypes,
			true,
			IgnoreActors,
			EDrawDebugTrace::ForDuration,
			OutHit,
			true,
			FLinearColor::Blue,
			FLinearColor::Yellow,
			3.0f
		);

		if (bResult)
		{
			//UE_LOG(LogProjectBD, Warning, TEXT("Hit %s"), *OutHit.GetActor()->GetName());
			//Hit Effect
			S2A_DecalAndHitEffect(OutHit);

			//데미지 처리, 데미지 전달
			UGameplayStatics::ApplyPointDamage(
				OutHit.GetActor(),
				30.0f,
				Dir,
				OutHit,
				GetController(),
				this,
				UBulletDamageType::StaticClass()
			);

			/*UGameplayStatics::ApplyRadialDamage(
				GetWorld(),
				30.0f,
				OutHit.Location,
				300.0f,
				UBulletDamageType::StaticClass(),
				IgnoreActors
			);*/
		}
	}
}

float ANetPlayer::TakeDamage(float DamageAmount, FDamageEvent const & DamageEvent, AController * EventInstigator, AActor * DamageCauser)
{
	Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	if (DamageEvent.IsOfType(FRadialDamageEvent::ClassID))
	{
		CurrentHP -= DamageAmount;

		UE_LOG(LogProjectBD, Warning, TEXT("Radial Damage %f"), CurrentHP);
	}
	else if (DamageEvent.IsOfType(FPointDamageEvent::ClassID))
	{
		FPointDamageEvent* PointDamageEvent = (FPointDamageEvent*)(&DamageEvent);

		//UE_LOG(LogProjectBD, Warning, TEXT("Point Damage %s"), *PointDamageEvent->HitInfo.BoneName.ToString());

		if (PointDamageEvent->HitInfo.BoneName.Compare(TEXT("head")) == 0)
		{
			CurrentHP = 0;
		}
		else
		{
			CurrentHP -= DamageAmount;
		}

		if (CurrentHP <= 0)
		{
			CurrentHP = 0;
			S2A_DeadProcess();
		}

		UE_LOG(LogProjectBD, Warning, TEXT("CurrentHP %f"), CurrentHP);
	}
	else if (DamageEvent.IsOfType(FDamageEvent::ClassID))
	{

	}

	return DamageAmount;
}

void ANetPlayer::S2A_DeadProcess_Implementation()
{
	//GetMesh()->SetSimulatePhysics(true);
	FString SectionName = FString::Printf(TEXT("Death_%d"), FMath::RandRange(1, 2));
	PlayAnimMontage(DeadAnimation, 1.0f, FName(*SectionName));
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	DisableInput(Cast<APlayerController>(GetController()));
}

void ANetPlayer::StartLeanLeft()
{
	CurrentLeanType = EPlayerLeanType::Left;
	bLeftLean = true;
}

void ANetPlayer::StopLeanLeft()
{
	CurrentLeanType = EPlayerLeanType::Normal;
	bLeftLean = false;
}

void ANetPlayer::StartLeanRight()
{
	CurrentLeanType = EPlayerLeanType::Right;
	bRightLean = true;
}

void ANetPlayer::StopLeanRight()
{
	CurrentLeanType = EPlayerLeanType::Normal;
	bRightLean = false;
}

void ANetPlayer::Reload()
{
	bIsReload = true;
}

void ANetPlayer::AddItemList(AMasterItem * Item)
{
	if (!Item->IsPendingKill())
	{
		GetItemList.Add(Item);
		int Index = GetClosestItem();
		if (Index != -1)
		{
			ALocalPC* PC = Cast<ALocalPC>(GetController());
			if (PC)
			{
				PC->ShowItemName(GetItemList[Index]->ItemData.ItemName, true);
			}
		}
	}
}

void ANetPlayer::RemoveItemList(AMasterItem * Item)
{
	GetItemList.Remove(Item);
	int Index = GetClosestItem();
	if (Index != -1) // 주변에 아이템이 없을 때
	{
		ALocalPC* PC = Cast<ALocalPC>(GetController());
		if (PC)
		{
			PC->ShowItemName(GetItemList[Index]->ItemData.ItemName, true);
		}
	}
	else // 주변에 아이템이 한개라도 있을 때
	{
		ALocalPC* PC = Cast<ALocalPC>(GetController());
		if (PC)
		{
			FString Temp(TEXT(""));
			PC->ShowItemName(Temp, false);
		}
	}
}

int ANetPlayer::GetClosestItem()
{
	float Min = 9999999999.9f;
	int MinIndex = -1;
	int i = 0;
	for (auto Item : GetItemList)
	{
		float Distance = FVector::Distance(GetActorLocation(), Item->GetActorLocation());
		if (Min > Distance)
		{
			Min = Distance;
			MinIndex = i;
		}
		i++;
	}
	return MinIndex;
}

void ANetPlayer::PickupItem()
{
	int ItemIndex = GetClosestItem();
	if (ItemIndex != -1)
	{
		auto Item = GetItemList[ItemIndex];
		GetItemList.Remove(Item);
		//인벤토리에 추가
		UBDGameInstance* GI = Cast<UBDGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
		if (GI)
		{
			GI->PushItem(Item->ItemData);
			ALocalPC* PC = Cast<ALocalPC>(GetController());
			if (PC)
			{
				PC->UpdateSlotData();
			}
		}
		Item->Destroy();
	}
}

void ANetPlayer::ShowInventory()
{
	ALocalPC* PC = Cast<ALocalPC>(GetController());
	if (PC)
	{
		PC->ShowInventory();
	}
}

void ANetPlayer::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	//조건식으로 대역폭 줄임
	//DOREPLIFETIME_CONDITION(ANetPlayer, bIsSprint, COND_SimulatedOnly);

	DOREPLIFETIME(ANetPlayer, bIsSprint);
	DOREPLIFETIME(ANetPlayer, bIsIronsight);
	DOREPLIFETIME(ANetPlayer, bIsFire);
	DOREPLIFETIME(ANetPlayer, CurrentHP);
	DOREPLIFETIME(ANetPlayer, bLeftLean);
	DOREPLIFETIME(ANetPlayer, bRightLean);
	DOREPLIFETIME(ANetPlayer, bIsReload);
}

void ANetPlayer::S2A_MuzzleFlashAndSound_Implementation()
{
	//MuzzleFlash, Fire Sound
	UGameplayStatics::SpawnSoundAttached(FireSound, Weapon);
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), FireEffect, Weapon->GetSocketTransform(TEXT("MuzzleFlash")));
}

void ANetPlayer::S2A_DecalAndHitEffect_Implementation(const FHitResult& OutHit)
{
	if (OutHit.GetActor()->ActorHasTag(TEXT("Player")))
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(),
			BloodEffect,
			OutHit.ImpactPoint,
			OutHit.ImpactNormal.Rotation()
		);
	}
	else
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(),
			HitEffect,
			OutHit.ImpactPoint,
			OutHit.ImpactNormal.Rotation()
		);

		UDecalComponent* Decal = UGameplayStatics::SpawnDecalAtLocation(GetWorld(),
			BulletDecal,
			FVector(5, 5, 5),
			OutHit.ImpactPoint,
			OutHit.ImpactNormal.Rotation(),
			10.0f
		);

		//UE_LOG(LogProjectBD, Warning, TEXT("Decal Fade Size %f"), Decal->FadeScreenSize);
		Decal->SetFadeScreenSize(0.001f); // Screen 차지 비율(percent)
	}
}
