#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "P02OutdoorReviewController.generated.h"

class AP02PlaytestEnemy;
class UP02CharacterOcclusionComponent;
class UMaterialInterface;

/** Optional harness placed only in the new outdoor review map. */
UCLASS()
class P02DUNGEON_API AP02OutdoorReviewController : public AActor
{
    GENERATED_BODY()
public:
    AP02OutdoorReviewController();
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaSeconds) override;

    UPROPERTY(EditAnywhere, Category="Outdoor Review")
    FVector CampCenter = FVector(19600, 1550, 1560);
    UPROPERTY(EditAnywhere, Category="Outdoor Review")
    float CampRadius = 750;
    UPROPERTY(EditInstanceOnly, Category="Outdoor Review")
    TArray<TObjectPtr<AP02PlaytestEnemy>> Enemies;
    UPROPERTY(EditAnywhere, Category="Outdoor Review")
    FVector Entrance = FVector(21500, 0, 1800);
    UPROPERTY(EditAnywhere, Category="Outdoor Review", meta=(ClampMin="1.0", Units="cm"))
    float EntranceHalfWidth = 240.0f;
    UPROPERTY(EditAnywhere, Category="Outdoor Review")
    FName Destination = TEXT("/Game/Portfolio02/Tests/FinalBossCandidates/L_Interior_FinalBossCandidates_TEST");
private:
    UPROPERTY(EditAnywhere, Category="Character Occlusion", meta=(AllowPrivateAccess="true"))
    bool bEnableCharacterOcclusion = false;
    UPROPERTY(EditAnywhere, Category="Character Occlusion", meta=(AllowPrivateAccess="true"))
    TObjectPtr<UMaterialInterface> CharacterOcclusionMaterial;
    UPROPERTY(EditAnywhere, Category="Character Occlusion", meta=(ClampMin="0.1", ClampMax="1", AllowPrivateAccess="true"))
    float CharacterOcclusionThreshold = 0.7f;
    UPROPERTY(EditAnywhere, Category="Character Occlusion", meta=(ClampMin="0", ClampMax="0.95", AllowPrivateAccess="true"))
    float CharacterOcclusionFadeAmount = 0.8f;
    TWeakObjectPtr<UP02CharacterOcclusionComponent> CharacterOcclusion;
    TArray<FVector> Homes;
    FVector Previous = FVector::ZeroVector;
    double Started = -1;
    double Distance = 0;
    bool bHavePrevious = false;
    bool bTravelPending = false;
    bool bCampActive = false;
    FString Route = TEXT("common");
};
