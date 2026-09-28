#include "P02OutdoorReviewController.h"
#include "P02FieldTelemetry.h"
#include "P02CharacterOcclusionComponent.h"
#include "P02PlaytestSystem.h"
#include "GameFramework/Pawn.h"
#include "Kismet/GameplayStatics.h"
#include "Components/SceneComponent.h"

AP02OutdoorReviewController::AP02OutdoorReviewController()
{
    PrimaryActorTick.bCanEverTick = true;
    SetRootComponent(CreateDefaultSubobject<USceneComponent>(TEXT("Root")));
}

void AP02OutdoorReviewController::BeginPlay()
{
    Super::BeginPlay();
    for (AP02PlaytestEnemy* Enemy : Enemies)
    {
        Homes.Add(IsValid(Enemy) ? Enemy->GetActorLocation() : FVector::ZeroVector);
        if (IsValid(Enemy))
        {
            Enemy->SetEncounterActive(false);
            Enemy->AddTickPrerequisiteActor(this);
        }
    }
}

void AP02OutdoorReviewController::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
    APawn* Pawn = UGameplayStatics::GetPlayerPawn(this, 0);
    if (!Pawn || bTravelPending) return;
    if (bEnableCharacterOcclusion && CharacterOcclusionMaterial &&
        (!CharacterOcclusion.IsValid() || CharacterOcclusion->GetOwner() != Pawn))
    {
        if (CharacterOcclusion.IsValid()) CharacterOcclusion->DestroyComponent();
        auto* Component = NewObject<UP02CharacterOcclusionComponent>(Pawn);
        Component->FadeMaterial = CharacterOcclusionMaterial;
        Component->StartThreshold = CharacterOcclusionThreshold;
        Component->FadeAmount = CharacterOcclusionFadeAmount;
        Pawn->AddInstanceComponent(Component);
        Component->RegisterComponent();
        CharacterOcclusion = Component;
    }
    const FVector Position = Pawn->GetActorLocation();
    const double Now = GetWorld()->GetTimeSeconds();
    if (bHavePrevious)
    {
        const double Step = FVector::Dist(Position, Previous);
        if (Step > 1 && Started < 0) Started = Now;
        Distance += Step;
    }
    Previous = Position;
    bHavePrevious = true;
    if (Route == TEXT("common") && FMath::Abs(Position.Y) > 600)
    {
        Route = Position.Y > 0 ? TEXT("right") : TEXT("left");
        UE_LOG(LogTemp, Display, TEXT("OUTDOOR route=%s time=%.3f"), *Route, Now);
    }
    const bool bInCamp = FVector::Dist2D(Position, CampCenter) < CampRadius;
    if (bInCamp != bCampActive)
    {
        bCampActive = bInCamp;
        UE_LOG(LogTemp, Display, TEXT("OUTDOOR camp=%d time=%.3f"), bInCamp, Now);
    }
    for (int32 Index = 0; Index < Enemies.Num(); ++Index)
    {
        AP02PlaytestEnemy* Enemy = Enemies[Index];
        if (!IsValid(Enemy)) continue;
        const bool bOutside = FVector::Dist2D(Enemy->GetActorLocation(), CampCenter) > CampRadius;
        Enemy->SetEncounterActive(bInCamp && !bOutside);
        // Review-only leash: reset survivors when the player leaves the camp.
        // No additional gate or encounter-clear condition is introduced.
        if ((!bInCamp || bOutside) && Homes.IsValidIndex(Index))
            Enemy->SetActorLocation(Homes[Index], false, nullptr, ETeleportType::TeleportPhysics);
    }
    const FVector Relative = Position - Entrance;
    if (Started >= 0 && Relative.X > 40 && Relative.X < 360 &&
        FMath::Abs(Relative.Y) <= EntranceHalfWidth && FMath::Abs(Relative.Z - 96) < 160)
    {
        bTravelPending = true;
        UE_LOG(LogTemp, Display, TEXT("OUTDOOR complete route=%s seconds=%.3f distance_uu=%.1f"),
            *Route, Now - Started, Distance);
        if(auto* Log=GetWorld()->GetSubsystem<UP02FieldTelemetry>()) Log->BeforeTravel(Destination.ToString());
        UGameplayStatics::OpenLevel(this, Destination);
    }
}
