// Fill out your copyright notice in the Description page of Project Settings.

#include "Character/DFCharacterBase.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "PaperFlipbookComponent.h"
#include "Kismet/KismetSystemLibrary.h"

void ADFCharacterBase::BeginPlay()
{
	Super::BeginPlay();
	UpdateAttributes();
	UpdateState();
	OnTakeAnyDamage.AddDynamic(this, &ADFCharacterBase::TakeDamage);
	UPaperFlipbookComponent* sprite = GetSprite();

	if (sprite)
	{
		sprite->OnFinishedPlaying.AddDynamic(this, &ADFCharacterBase::OnFlipbookPlayed);
	}
}

void ADFCharacterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAxis("Forward", this, &ADFCharacterBase::Move);
	PlayerInputComponent->BindAction("Jump", EInputEvent::IE_Pressed, this, &ADFCharacterBase::DoJump);
}

void ADFCharacterBase::DoJump()
{
	if(IsAlive())
		Jump();
}

void ADFCharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	ProcessAttack();
}

void ADFCharacterBase::UpdateAttributes()
{
	if (AttributeTable)
	{
		FDFCharacterData* data = AttributeTable->FindRow<FDFCharacterData>(AttributeKey, "");

		if (data)
		{
			CurrentHealth = data->Attributes.MaxHealth;

			if (GetCharacterMovement())
			{
				GetCharacterMovement()->MaxWalkSpeed = data->Attributes.RunSpeed;
				GetCharacterMovement()->JumpZVelocity = data->Attributes.JumpSpeed;
			}

			AttackData = data->AttackData;
		}
	}
}

void ADFCharacterBase::TakeDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
{
	if (IsAlive())
	{
		CurrentHealth -= Damage;

		if (CurrentHealth <= 0)
		{
			CurrentState = EDFCharacterState::Dead;
			PlayFlipBook(DeathFlipbook);
			CurrentHealth = 0;
			GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			SetLifeSpan(2);
		}
		else
		{
			CurrentState = EDFCharacterState::TakeHit;
			PlayFlipBook(HitFlipbook);
		}
	}
}

void ADFCharacterBase::Move(float Direction)
{
	if (GetCharacterMovement() && IsAlive() && CurrentState != EDFCharacterState::Attack && CurrentState != EDFCharacterState::TakeHit)
	{
		GetCharacterMovement()->AddInputVector(FVector(Direction, 0, 0));

		if (Controller && Controller->IsLocalPlayerController())
		{
			APlayerController* const PC = CastChecked<APlayerController>(Controller);

			if(Direction >= 1)
				PC->SetControlRotation(FRotator(0, 0, 0));
			else if (Direction <= -1)
				PC->SetControlRotation(FRotator(0, 180, 0));
		}

		UpdateState();
	}
}

EDFCharacterState ADFCharacterBase::GetState()
{
	if (GetCharacterMovement()->IsFalling())
		return GetCharacterMovement()->Velocity.Z > 0 ? EDFCharacterState::Jump : EDFCharacterState::Faling;
	else
		return FMath::Abs(GetCharacterMovement()->Velocity.X) > 0 ? EDFCharacterState::Run: EDFCharacterState::Idle;

	return EDFCharacterState::None;
}

void ADFCharacterBase::UpdateFlipbook()
{
	UPaperFlipbookComponent* sprite = GetSprite();
	UPaperFlipbook* flipbook = nullptr;

	if (sprite)
	{

		switch (CurrentState)
		{
			case EDFCharacterState::None:
				flipbook= nullptr;
				break;

			case EDFCharacterState::Idle:
				flipbook = IdleFlipbook;
				break;

			case EDFCharacterState::Run:
				flipbook = RunFlipbook;
				break;

			case EDFCharacterState::Jump:
				flipbook = JumpFlipbook;
				break;

			case EDFCharacterState::Faling:
				flipbook = FallFlipbook;
				break;

			case EDFCharacterState::Dead:
				flipbook = DeathFlipbook;
				break;

			default:
				flipbook = nullptr;
				break;
		}
	}

	PlayFlipBook(flipbook, true);
}

void ADFCharacterBase::UpdateState()
{
	EDFCharacterState state = GetState();

	if (CurrentState != state)
	{
		CurrentState = state;
		UpdateFlipbook();
	}
}

void ADFCharacterBase::Attack(EDFAttackType Type)
{
	if (IsAlive() && (CurrentState == EDFCharacterState::Idle || CurrentState == EDFCharacterState::Run))
	{
		for (FDFAttackData data : AttackData)
		{
			if (data.AttackType == Type)
			{
				CurrentState = EDFCharacterState::Attack;
				CurrentAttackData = data;
				PlayFlipBook(data.AttackFlipbook);
				break;
			}
		}
	}
}

void ADFCharacterBase::OnFlipbookPlayed()
{
	CurrentState = EDFCharacterState::None;

	if(IsAlive())
		UpdateState();
}

void ADFCharacterBase::ProcessAttack()
{
	UPaperFlipbookComponent* sprite = GetSprite();

	if (sprite && CurrentState == EDFCharacterState::Attack)
	{
		if (sprite->GetPlaybackPositionInFrames() == CurrentAttackData.ImpactFrame)
		{
			PerformAttack(CurrentAttackData.Range, CurrentAttackData.Damage);
		}
	}
}

void ADFCharacterBase::PlayFlipBook(UPaperFlipbook* Flipbook, bool Loop)
{
	UPaperFlipbookComponent* sprite = GetSprite();

	if (sprite)
	{
		sprite->SetFlipbook(Flipbook);
		sprite->SetLooping(Loop);
		sprite->PlayFromStart();
	}
}

void ADFCharacterBase::Destroyed()
{
	Super::Destroyed();
	UPaperFlipbookComponent* sprite = GetSprite();

	if (sprite)
	{
		sprite->OnFinishedPlaying.RemoveDynamic(this, &ADFCharacterBase::OnFlipbookPlayed);
	}
}