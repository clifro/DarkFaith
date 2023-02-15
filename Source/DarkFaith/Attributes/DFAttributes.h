#pragma once

#include "CoreMinimal.h"
#include "PaperFlipbook.h"
#include "Engine/DataTable.h"
#include "DFAttributes.generated.h"

USTRUCT(BlueprintType)
struct FDFAttributes
{
    GENERATED_BODY()

public:

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        float MaxHealth;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        float RunSpeed;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        float JumpSpeed;
};

UENUM()
enum EDFAttackType
{
    LightAttack UMETA(DisplayName = "Light Attack"),
    HeavyAttack UMETA(DisplayName = "Heavy Attack")
};

USTRUCT(BlueprintType)
struct FDFAttackData
{
    GENERATED_BODY()

public:

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        TEnumAsByte<EDFAttackType> AttackType;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        float Range;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        float Damage;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        UPaperFlipbook* AttackFlipbook;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        int ImpactFrame;
};

USTRUCT(BlueprintType)
struct FDFCharacterData : public FTableRowBase
{
    GENERATED_BODY()

public:

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        FDFAttributes Attributes;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        TArray<FDFAttackData> AttackData;
};