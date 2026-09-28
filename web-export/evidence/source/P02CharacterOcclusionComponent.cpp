#include "P02CharacterOcclusionComponent.h"
#include "Camera/PlayerCameraManager.h"
#include "Components/CapsuleComponent.h"
#include "Components/MeshComponent.h"
#include "Engine/World.h"
#include "GameFramework/Character.h"
#include "GameFramework/PlayerController.h"
#include "HAL/IConsoleManager.h"
#include "Materials/MaterialInstanceDynamic.h"

static TAutoConsoleVariable<int32> P02CharacterOcclusionEnabled(
    TEXT("p02.CharacterOcclusion.Enabled"), 1, TEXT("Enable opt-in local character occlusion fading."));
static const FName OcclusionTag(TEXT("P02_AutoOcclusionWall"));

UP02CharacterOcclusionComponent::UP02CharacterOcclusionComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
    PrimaryComponentTick.TickGroup = TG_PostUpdateWork;
}

void UP02CharacterOcclusionComponent::Restore(FP02OcclusionSurface& Surface)
{
    if (!IsValid(Surface.Mesh)) return;
    for (int32 Slot = 0; Slot < Surface.Originals.Num(); ++Slot)
    {
        // Preserve material changes made by another system while fading.
        if (Surface.Instances.IsValidIndex(Slot) && Surface.Mesh->GetMaterial(Slot) == Surface.Instances[Slot])
            Surface.Mesh->SetMaterial(Slot, Surface.Originals[Slot]);
    }
}

void UP02CharacterOcclusionComponent::RestoreAll()
{
    for (auto& Surface : Surfaces) Restore(Surface);
    Surfaces.Reset(); Wanted.Reset(); OcclusionRatio = 0; bOcclusionActive = false; FadedSurfaceCount = 0;
}

void UP02CharacterOcclusionComponent::EndPlay(const EEndPlayReason::Type Reason)
{
    RestoreAll();
    Super::EndPlay(Reason);
}

void UP02CharacterOcclusionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* TickFunction)
{
    Super::TickComponent(DeltaTime, TickType, TickFunction);
    ACharacter* Pawn = Cast<ACharacter>(GetOwner());
    APlayerController* PC = Pawn ? Cast<APlayerController>(Pawn->GetController()) : nullptr;
    if (!Pawn || !PC || !PC->IsLocalController() || !PC->PlayerCameraManager || !FadeMaterial || !P02CharacterOcclusionEnabled.GetValueOnGameThread())
    {
        RestoreAll(); ScanCountdown = 0; return;
    }
    const FVector Camera = PC->PlayerCameraManager->GetCameraLocation();
    const FVector Center = Pawn->GetActorLocation();
    const FVector ScreenRight = PC->PlayerCameraManager->GetCameraRotation().RotateVector(FVector::RightVector);
    const float HalfHeight = Pawn->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
    const float Radius = Pawn->GetCapsuleComponent()->GetScaledCapsuleRadius();
    ScanCountdown -= DeltaTime;
    if (ScanCountdown <= 0)
    {
        ScanCountdown = 0.1f;
        Wanted.Reset();
        TSet<TWeakObjectPtr<UMeshComponent>> Hits;
        int32 Blocked = 0;
        TArray<AActor*> Attached; Pawn->GetAttachedActors(Attached);
        for (int32 Row = 0; Row < 5; ++Row)
        for (int32 Column = 0; Column < 2; ++Column)
        {
            const FVector Target = Center + FVector(0, 0, HalfHeight * (-0.72f + Row * 0.36f))
                + ScreenRight * Radius * (Column == 0 ? -0.55f : 0.55f);
            FCollisionQueryParams Params(SCENE_QUERY_STAT(P02CharacterOcclusion), true, Pawn);
            Params.AddIgnoredActors(Attached);
            bool bSampleBlocked = false;
            // Trace through opted-in blockers to handle several overlapping walls.
            for (int32 Layer = 0; Layer < 4; ++Layer)
            {
                FHitResult Hit;
                if (!GetWorld()->LineTraceSingleByChannel(Hit, Camera, Target, ECC_Camera, Params)) break;
                bSampleBlocked = true;
                UMeshComponent* Mesh = Cast<UMeshComponent>(Hit.GetComponent());
                if (!Mesh || !Mesh->ComponentHasTag(OcclusionTag)) break;
                Hits.Add(Mesh); Params.AddIgnoredComponent(Mesh);
            }
            Blocked += bSampleBlocked ? 1 : 0;
        }
        OcclusionRatio = Blocked / 10.0f;
        const float Start = FMath::Clamp(StartThreshold, 0.1f, 1.0f);
        const float Stop = FMath::Max(0.0f, Start - 0.2f);
        if (!bOcclusionActive && OcclusionRatio >= Start) bOcclusionActive = true;
        else if (bOcclusionActive && OcclusionRatio <= Stop) bOcclusionActive = false;
        if (bOcclusionActive) Wanted = MoveTemp(Hits);
        for (auto Target : Wanted)
        {
            if (!Target.IsValid() || Surfaces.ContainsByPredicate([&](const auto& S) { return S.Mesh == Target.Get(); })) continue;
            FP02OcclusionSurface Surface; Surface.Mesh = Target.Get();
            for (int32 Slot = 0; Slot < Surface.Mesh->GetNumMaterials(); ++Slot)
            {
                Surface.Originals.Add(Surface.Mesh->GetMaterial(Slot));
                UMaterialInstanceDynamic* MID = UMaterialInstanceDynamic::Create(FadeMaterial, this);
                MID->SetScalarParameterValue(TEXT("CharacterFade"), 0);
                Surface.Instances.Add(MID); Surface.Mesh->SetMaterial(Slot, MID);
            }
            Surfaces.Add(MoveTemp(Surface));
        }
    }
    FadedSurfaceCount = 0;
    for (int32 Index = Surfaces.Num() - 1; Index >= 0; --Index)
    {
        auto& Surface = Surfaces[Index];
        if (!IsValid(Surface.Mesh)) { Surfaces.RemoveAtSwap(Index); continue; }
        const float Target = Wanted.Contains(Surface.Mesh.Get()) ? FMath::Clamp(FadeAmount, 0.0f, 0.95f) : 0;
        Surface.Alpha = FMath::FInterpConstantTo(Surface.Alpha, Target, DeltaTime, Target > 0 ? 4.0f : 2.5f);
        if (Surface.Alpha <= 0 && Target == 0) { Restore(Surface); Surfaces.RemoveAtSwap(Index); continue; }
        ++FadedSurfaceCount;
        for (auto MID : Surface.Instances)
        {
            MID->SetScalarParameterValue(TEXT("CharacterFade"), Surface.Alpha);
            MID->SetVectorParameterValue(TEXT("CharacterCenter"), FLinearColor(Center.X, Center.Y, Center.Z, 1));
            MID->SetScalarParameterValue(TEXT("ClearRadius"), FMath::Max(130.0f, HalfHeight * 1.45f));
        }
    }
}
