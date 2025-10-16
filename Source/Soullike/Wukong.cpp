// Fill out your copyright notice in the Description page of Project Settings.


#include "Wukong.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "InputAction.h"
#include "DamageReceiver.h"
#include "Blueprint/UserWidget.h"
#include "Components/SphereComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "PlayerHUD.h"

AWukong::AWukong()
{
	PrimaryActorTick.bCanEverTick = true;
	WukongState = EPlayerState::Peace;
	DamageReceiver = CreateDefaultSubobject<UDamageReceiver>(TEXT("DamageReceiver"));

}

void AWukong::BeginPlay()
{
	Super::BeginPlay();

	// PlayerController�� ���� ����
	if (APlayerController* PC = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(InputMappingContext, 0);
		}
	}

	if (DamageReceiver) {
		DamageReceiver->SetHp(MaxHp);
		DamageReceiver->OnDead.AddDynamic(this, &AWukong::OnDead);
		DamageReceiver->OnHealthPowerChanged.AddDynamic(this, &AWukong::OnHpChanged);
	}
}

void AWukong::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AWukong::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	if (UEnhancedInputComponent* Input = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		Input->BindAction(IA_Move, ETriggerEvent::Triggered, this, &AWukong::OnHitMove);
		Input->BindAction(IA_Look, ETriggerEvent::Triggered, this, &AWukong::OnHitLook);
		Input->BindAction(IA_Jump, ETriggerEvent::Triggered, this, &AWukong::OnHitJump);
		Input->BindAction(IA_Roll, ETriggerEvent::Triggered, this, &AWukong::OnHitRollInput);
		Input->BindAction(IA_Attack, ETriggerEvent::Triggered, this, &AWukong::OnHitAttack);
	}
}

//����� Ű�Է�
void AWukong::OnHitMove(const FInputActionValue& Value)
{
	UE_LOG(LogTemp, Log, TEXT("Move"));
	if (!bCanMove || WukongState != EPlayerState::Peace) {
		return;
	}

	FVector2D MoveAxisVector = Value.Get<FVector2D>();

	//��Ʈ�ѷ� ȸ����. ī�޶� �÷��̾� ������ ������ ĳ���Ͱ� �ȿ����̴� �����ذ��� ���� ��ȸ���� ���
	const FRotator Rotation = Controller->GetControlRotation();
	const FRotator YawRotation(0, Rotation.Yaw, 0);
	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

	//�Ž� -90�� ȸ��������
	AddMovementInput(ForwardDirection, MoveAxisVector.X);
	AddMovementInput(RightDirection, MoveAxisVector.Y);
}

//ī�޶� �̵�
void AWukong::OnHitLook(const FInputActionValue& Value)
{
	//UE_LOG(LogTemp, Log, TEXT("Look: %f, %f"), X, Y);
	// FInputActionValue�� FVector2D�� ��ȯ
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	AddControllerYawInput(LookAxisVector.X);
	AddControllerPitchInput(LookAxisVector.Y);

}

void AWukong::OnHitJump(const FInputActionValue& Value)
{
	UE_LOG(LogTemp, Log, TEXT("Jump"));
	if (!bCanMove) {
		return;
	}
	Jump();
}

void AWukong::OnHitRollInput(const FInputActionValue& Value)
{
	OnHitRoll(Value);
}

bool AWukong::OnHitRoll_Implementation(const FInputActionValue& Value)
{
	UE_LOG(LogTemp, Log, TEXT("Roll"));
	if (WukongState != EPlayerState::Peace || GetCharacterMovement()->IsFalling()) {
		return false;
	}
	switch (AttackState)
	{
	case EAttackState::Hit:
	case EAttackState::SlamPush:
	case EAttackState::SlamDown:
		return false;
	case EAttackState::None:
		NextCombo = NAME_None;
		break;
	}

	SetState(EPlayerState::Roll);
	return true;
}

void AWukong::OnHitAttack(const FInputActionValue& Value)
{
	UE_LOG(LogTemp, Log, TEXT("Attack"));

}



void AWukong::OnHpChanged(float HealthPower, float MaxHealthPower) {
	UE_LOG(LogTemp, Log, TEXT("HpChanged"));
	CurrentHp = HealthPower;
	MaxHp = MaxHealthPower;
	PlayerHUD->SetHp(CurrentHp, MaxHp);
}

//���������ù����� �˷���
void AWukong::OnDead()
{
	UE_LOG(LogTemp, Log, TEXT("Dead"));

	//�״� ����� ���� ����
	ECollisionChannel MonsterAttackChannel = ECC_GameTraceChannel4;
	GetCapsuleComponent()->SetCollisionResponseToChannel(MonsterAttackChannel, ECR_Ignore);

	//���� ����
	SetState(EPlayerState::Dead);
}

void AWukong::SetState(EPlayerState NewState)
{
	UE_LOG(LogTemp, Log, TEXT("SetState"));
	if (WukongState != EPlayerState::Dead) {
		WukongState = NewState;
	}
}

