// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Local/MyPlayer.h"
#include "NetPlayer.generated.h"

UCLASS()
class PROJECTBD_API ANetPlayer : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ANetPlayer();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class USpringArmComponent* SpringArm;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UCameraComponent* Camera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UStaticMeshComponent* Weapon;

	// Ironsight 동작용
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "State", Replicated)
	bool bIsIronsight = false;

	// Sprint 동작용
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "State", Replicated)
	bool bIsSprint = false;

	void Forward(float Value);
	void Right(float Value);
	void Pitch(float Value);
	void Yaw(float Value);

	void Crouching();
	void DoIronsight();

	UFUNCTION(Server, Reliable, WithValidation)
	void C2S_DoIronsight();
	bool C2S_DoIronsight_Validate();
	void C2S_DoIronsight_Implementation();

	void StartSprint();

	UFUNCTION(Server, Reliable, WithValidation)
	void C2S_StartSprint();
	bool C2S_StartSprint_Validate();
	void C2S_StartSprint_Implementation();

	void StopSprint();

	UFUNCTION(Server, Reliable, WithValidation)
	void C2S_StopSprint();
	bool C2S_StopSprint_Validate();
	void C2S_StopSprint_Implementation();

	FRotator GetAimOffset() const;

	UPROPERTY(EditAnyWhere, BlueprintReadOnly, Category = "State")
	float SprintSpeed = 800.0f;

	UPROPERTY(EditAnyWhere, BlueprintReadOnly, Category = "State")
	float JogSpeed = 450.0f;

	UPROPERTY(EditAnyWhere, BlueprintReadOnly, Category = "State")
	float IronsightSpeed = 150.0f;

	UPROPERTY(EditAnyWhere, BlueprintReadOnly, Category = "State")
	float CrouchSpeed = 150.0f;

	UPROPERTY(EditAnyWhere, BlueprintReadOnly, Category = "Camera")
	FVector NormalSpringArmPosition;

	UPROPERTY(EditAnyWhere, BlueprintReadOnly, Category = "Camera")
	FVector CrouchSpringArmPosition;

	UPROPERTY(EditAnyWhere, BlueprintReadOnly, Category = "State", Replicated)
	bool bIsFire = false;

	void StartFire();
	void StopFire();
	void OnShoot();

	UFUNCTION(Server, Reliable, WithValidation)
	void C2S_Fire(FVector TraceStart, FVector TraceEnd);
	bool C2S_Fire_Validate(FVector TraceStart, FVector TraceEnd);
	void C2S_Fire_Implementation(FVector TraceStart, FVector TraceEnd);

	FTimerHandle ShootTimer;

	UPROPERTY(EditAnyWhere, BlueprintReadOnly, Category = "Effect")
	class UParticleSystem* FireEffect;

	UPROPERTY(EditAnyWhere, BlueprintReadOnly, Category = "Effect")
	class UParticleSystem* HitEffect;

	UPROPERTY(EditAnyWhere, BlueprintReadOnly, Category = "Effect")
	class UParticleSystem* BloodEffect;

	UPROPERTY(EditAnyWhere, BlueprintReadOnly, Category = "Effect")
	class USoundBase* FireSound;

	UPROPERTY(EditAnyWhere, BlueprintReadOnly, Category = "Effect")
	class UMaterialInterface* BulletDecal;

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
	class AController* EventInstigator, AActor* DamageCauser) override;

	UPROPERTY(EditAnyWhere, BlueprintReadOnly, Category = "Health", Replicated)
	float CurrentHP = 100.0f;

	UPROPERTY(EditAnyWhere, BlueprintReadOnly, Category = "Health")
	float MaxHP = 100.0f;

	UPROPERTY(EditAnyWhere, BlueprintReadOnly, Category = "Animation")
	class UAnimMontage* DeadAnimation;

	UPROPERTY(EditAnyWhere, BlueprintReadOnly, Category = "Animation")
	class UAnimMontage* ReloadAnimation;

	UFUNCTION(NetMulticast, Reliable)
	void S2A_DeadProcess();
	void S2A_DeadProcess_Implementation();

	// TODO: 네트워크 동기화 기능 구현
	void StartLeanLeft();
	void StopLeanLeft();
	void StartLeanRight();
	void StopLeanRight();

	UPROPERTY(EditAnyWhere, BlueprintReadOnly, Category = "Animation")
	EPlayerLeanType CurrentLeanType = EPlayerLeanType::Normal;

	UPROPERTY(EditAnyWhere, BlueprintReadOnly, Category = "Animation", Replicated)
	bool bLeftLean = false;

	UPROPERTY(EditAnyWhere, BlueprintReadOnly, Category = "Animation", Replicated)
	bool bRightLean = false;

	UPROPERTY(EditAnyWhere, BlueprintReadOnly, Category = "Animation", Replicated)
	bool bIsReload = false;

	void Reload();

	TArray<class AMasterItem*> GetItemList;

	void AddItemList(class AMasterItem* Item);
	void RemoveItemList(class AMasterItem* Item);

	int GetClosestItem();

	void PickupItem();

	void ShowInventory();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(NetMulticast, Unreliable)
	void S2A_MuzzleFlashAndSound();
	void S2A_MuzzleFlashAndSound_Implementation();

	UFUNCTION(NetMulticast, Unreliable)
	void S2A_DecalAndHitEffect(const FHitResult& OutHit);
	void S2A_DecalAndHitEffect_Implementation(const FHitResult& OutHit);
};
