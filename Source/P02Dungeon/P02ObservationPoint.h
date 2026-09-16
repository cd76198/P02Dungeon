#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Blueprint/UserWidget.h"
#include "P02ObservationPoint.generated.h"

class UBoxComponent;
class UTexture2D;
class UFont;
class APawn;
class AP02ObservationPoint;

UCLASS()
class P02DUNGEON_API UP02ObservationImageWidget : public UUserWidget
{
    GENERATED_BODY()
public:
    void Configure(AP02ObservationPoint* Point, UTexture2D* Image, bool bPlaceholder);
protected:
    virtual TSharedRef<SWidget> RebuildWidget() override;
    virtual FReply NativeOnKeyDown(const FGeometry& Geometry, const FKeyEvent& Event) override;
    UFUNCTION() void CloseClicked();
private:
    TWeakObjectPtr<AP02ObservationPoint> OwnerPoint;
    UPROPERTY() TObjectPtr<UTexture2D> DisplayImage;
    UPROPERTY() TObjectPtr<UFont> KoreanFont;
    bool bTemporaryImage = true;
};

/** Optional, side-specific observation. Only an explicitly placed actor enables it. */
UCLASS()
class P02DUNGEON_API AP02ObservationPoint : public AActor
{
    GENERATED_BODY()
public:
    AP02ObservationPoint();
    virtual void Tick(float DeltaSeconds) override;
    virtual void EndPlay(const EEndPlayReason::Type Reason) override;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly) TObjectPtr<UBoxComponent> InteractionArea;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Observation") FVector SightTargetOffset = FVector(0, 300, 250);
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Observation") TObjectPtr<UTexture2D> ObservationImage;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Observation") bool bPlaceholderImage = true;
    UFUNCTION(BlueprintPure) bool CanObserve(APawn* Pawn) const;
    UFUNCTION(BlueprintCallable) void OpenObservation();
    UFUNCTION(BlueprintCallable) void CloseObservation();
    UFUNCTION(BlueprintPure) bool IsObservationOpen() const { return IsValid(Viewer); }
private:
    UPROPERTY() TObjectPtr<UP02ObservationImageWidget> Viewer;
    TWeakObjectPtr<APlayerController> ObservingController;
    bool bPreviousCursor = false;
    bool bPreviousPaused = false;
    bool bWasEligible = false;
};
