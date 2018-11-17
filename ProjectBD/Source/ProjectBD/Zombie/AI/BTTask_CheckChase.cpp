// Fill out your copyright notice in the Description page of Project Settings.

#include "BTTask_CheckChase.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "Zombie/ZombieAIController.h"
#include "Zombie/Zombie.h"
#include "Perception/PawnSensingComponent.h"

// ���� �½�ũ�� Ư�� Ŭ������ ���ӵǸ� �ȵȴ�, �����常 �����ϵ��� ���� �Ұ� 
EBTNodeResult::Type UBTTask_CheckChase::ExecuteTask(UBehaviorTreeComponent & OwnerComp, uint8 * NodeMemory)
{
	AZombieAIController* AIC = Cast<AZombieAIController>(OwnerComp.GetAIOwner());
	if (AIC)
	{
		AZombie* Zombie = Cast<AZombie>(AIC->GetPawn());
		if (Zombie)
		{
			AActor* Player = Cast<AActor>(AIC->BBComponent->GetValueAsObject(FName(TEXT("Target"))));
			if (Player)
			{
				float Distance = FVector::Distance(Zombie->GetActorLocation(), Player->GetActorLocation());
				if (Zombie->PawnSensing->SightRadius < Distance)
				{
					Zombie->CurrentState = EZombieState::Normal;
					AIC->SetCurrentState(Zombie->CurrentState);
				}
				else if (Zombie->AttackRange >= Distance)
				{
					Zombie->CurrentState = EZombieState::Battle;
					AIC->SetCurrentState(Zombie->CurrentState);
				}
			}
		}
	}

	return EBTNodeResult::Succeeded;
}
