#pragma once

#include "CoreMinimal.h"
#include "PaperCharacter.h"
#include "Components/InputComponent.h"
#include "PaperFlipbook.h"
#include "Engine/DataTable.h"
#include "DarkFaith/Attributes/DFAttributes.h"
#include "DFCharacterBase.generated.h"

UENUM()
enum EDFCharacterState
{
	None	UMETA(DisplayName = "None"),
	Idle	UMETA(DisplayName = "Idle"),
	Run		UMETA(DisplayName = "Run"),
	Jump	UMETA(DisplayName = "Jump"),
	Faling	UMETA(DisplayName = "Fall"),
	Dead	UMETA(DisplayName = "Dead"),
	Attack	UMETA(DisplayName = "Attack"),
	TakeHit UMETA(DisplayName = "TakeHit"),
};

UCLASS()
class DARKFAITH_API ADFCharacterBase : public APaperCharacter
{
	GENERATED_BODY()

public:

	FORCEINLINE bool IsAlive() { return CurrentHealth > 0; }

	UPROPERTY()
		TEnumAsByte<EDFCharacterState> CurrentState = EDFCharacterState::None;

	UPROPERTY()
		float CurrentHealth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<FDFAttackData> AttackData;

	UFUNCTION()
		void TakeDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FName AttributeKey;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UDataTable* AttributeTable;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Flipbooks)
		UPaperFlipbook* IdleFlipbook;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Flipbooks)
		UPaperFlipbook* RunFlipbook;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Flipbooks)
		UPaperFlipbook* JumpFlipbook;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Flipbooks)
		UPaperFlipbook* FallFlipbook;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Flipbooks)
		UPaperFlipbook* HitFlipbook;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Flipbooks)
		UPaperFlipbook* DeathFlipbook;

	UFUNCTION()
		void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	UFUNCTION()
		void BeginPlay() override;

	UFUNCTION()
		void Tick(float DeltaTime) override;

	UFUNCTION()
		void Destroyed() override;

	UFUNCTION(BlueprintCallable)
		void Move(float Direction);

	UFUNCTION()
		void DoJump();

	UFUNCTION()
		EDFCharacterState GetState();

	UFUNCTION()
		void UpdateState();

	UFUNCTION()
		void UpdateFlipbook();

	UFUNCTION()
		void UpdateAttributes();

	UFUNCTION(BlueprintCallable)
		void Attack(EDFAttackType Type);

	UFUNCTION()
		void OnFlipbookPlayed();

	UPROPERTY()
		FDFAttackData CurrentAttackData;

	UFUNCTION()
		void ProcessAttack();

	UFUNCTION(BlueprintImplementableEvent)
		void PerformAttack(float Range, float Damage);

	UFUNCTION()
		void PlayFlipBook(UPaperFlipbook* Flipbook, bool Loop = false);
};
