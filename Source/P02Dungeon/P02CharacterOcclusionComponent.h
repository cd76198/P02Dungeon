#pragma once
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "P02CharacterOcclusionComponent.generated.h"

class UMeshComponent;
class UMaterialInterface;
class UMaterialInstanceDynamic;

USTRUCT()
struct FP02OcclusionSurface
{
    GENERATED_BODY()
    UPROPERTY() TObjectPtr<UMeshComponent> Mesh;
    UPROPERTY() TArray<TObjectPtr<UMaterialInterface>> Originals;
    UPROPERTY() TArray<TObjectPtr<UMaterialInstanceDynamic>> Instances;
    float Alpha = 0;
};

/** Render-only, opt-in wall fading for the local pawn; collision stays intact. */
UCLASS(ClassGroup=(P02), meta=(BlueprintSpawnableComponent))
class P02DUNGEON_API UP02CharacterOcclusionComponent : public UActorComponent
{
    GENERATED_BODY()
public:
    UP02CharacterOcclusionComponent();
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Occlusion", meta=(ClampMin="0.1", ClampMax="1"))
    float StartThreshold = 0.7f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Occlusion", meta=(ClampMin="0", ClampMax="0.95"))
    float FadeAmount = 0.8f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Occlusion")
    TObjectPtr<UMaterialInterface> FadeMaterial;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Occlusion")
    float OcclusionRatio = 0;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Occlusion")
    bool bOcclusionActive = false;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Occlusion")
    int32 FadedSurfaceCount = 0;
private:
    void Restore(FP02OcclusionSurface& Surface);
    void RestoreAll();
    UPROPERTY() TArray<FP02OcclusionSurface> Surfaces;
    TSet<TWeakObjectPtr<UMeshComponent>> Wanted;
    float ScanCountdown = 0;
};
