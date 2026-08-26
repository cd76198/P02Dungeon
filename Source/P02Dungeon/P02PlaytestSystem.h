// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Subsystems/WorldSubsystem.h"
#include "Tickable.h"
#include "P02PlaytestSystem.generated.h"

class ACharacter;
class APawn;
class UBoxComponent;
class UCameraComponent;
class UPrimitiveComponent;
class USceneComponent;
class USpringArmComponent;
class UStaticMeshComponent;

/** Fixed quarter-view camera that can follow any player pawn. */
UCLASS(BlueprintType)
class P02DUNGEON_API AP02QuarterViewCameraRig : public AActor
{
	GENERATED_BODY()

public:
	AP02QuarterViewCameraRig();
	virtual void Tick(float DeltaSeconds) override;

	void SetTrackedPawn(APawn* InPawn);
	void ApplyCameraSettings(float InPitch, float InYaw, float InDistance, float InFOV);
	float GetActualCameraDistance() const;
	bool IsCameraCollisionFixApplied() const;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "P02|Camera")
	float CameraPitch = -50.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "P02|Camera")
	float CameraYaw = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "P02|Camera", meta = (ClampMin = "100.0"))
	float CameraDistance = 1700.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "P02|Camera", meta = (ClampMin = "20.0", ClampMax = "120.0"))
	float CameraFOV = 45.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "P02|Camera")
	bool bEnableCameraCollision = false;

private:
	UPROPERTY(VisibleAnywhere, Category = "P02|Camera")
	TObjectPtr<USceneComponent> SceneRoot;

	UPROPERTY(VisibleAnywhere, Category = "P02|Camera")
	TObjectPtr<USpringArmComponent> SpringArm;

	UPROPERTY(VisibleAnywhere, Category = "P02|Camera")
	TObjectPtr<UCameraComponent> Camera;

	TWeakObjectPtr<APawn> TrackedPawn;
};

/** Minimal moving door for encounter-clear tests. */
UCLASS(BlueprintType)
class P02DUNGEON_API AP02PlaytestDoor : public AActor
{
	GENERATED_BODY()

public:
	AP02PlaytestDoor();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

	UFUNCTION(BlueprintCallable, Category = "P02|Door")
	void SetDoorOpen(bool bShouldOpen);

	UFUNCTION(BlueprintPure, Category = "P02|Door")
	bool IsDoorOpen() const { return bOpen; }

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "P02|Door")
	FVector OpenOffset = FVector(0.0f, 0.0f, 400.0f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "P02|Door", meta = (ClampMin = "1.0"))
	float OpenSpeed = 350.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "P02|Door")
	FVector DoorScale = FVector(0.5f, 10.0f, 3.0f);

private:
	UPROPERTY(VisibleAnywhere, Category = "P02|Door")
	TObjectPtr<USceneComponent> SceneRoot;

	UPROPERTY(VisibleAnywhere, Category = "P02|Door")
	TObjectPtr<UStaticMeshComponent> DoorMesh;

	FVector ClosedRelativeLocation = FVector::ZeroVector;
	bool bOpen = false;
};

/** Simple collision-driven enemy used only by the transient test course. */
UCLASS()
class P02DUNGEON_API AP02PlaytestEnemy : public AActor
{
	GENERATED_BODY()

public:
	AP02PlaytestEnemy();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
		class AController* EventInstigator, AActor* DamageCauser) override;

	void SetEncounterActive(bool bActive) { bEncounterActive = bActive; }
	float GetHealth() const { return CurrentHealth; }

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "P02|Enemy")
	float MaxHealth = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "P02|Enemy")
	float MoveSpeed = 250.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "P02|Enemy")
	float AttackRange = 150.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "P02|Enemy")
	float AttackDamage = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "P02|Enemy")
	float AttackCooldown = 1.2f;

private:
	UPROPERTY(VisibleAnywhere, Category = "P02|Enemy")
	TObjectPtr<UStaticMeshComponent> EnemyMesh;

	float CurrentHealth = 100.0f;
	float NextAttackTime = 0.0f;
	bool bEncounterActive = false;
};

/** Starts an encounter, tracks enemy destruction, and opens a linked door. */
UCLASS(BlueprintType)
class P02DUNGEON_API AP02EncounterTrigger : public AActor
{
	GENERATED_BODY()

public:
	AP02EncounterTrigger();
	virtual void BeginPlay() override;

	void ConfigureRuntimeTest(const TArray<AP02PlaytestEnemy*>& InEnemies, AP02PlaytestDoor* InDoor);

	UFUNCTION(BlueprintCallable, Category = "P02|Encounter")
	void ActivateEncounter();

	UFUNCTION(BlueprintPure, Category = "P02|Encounter")
	int32 GetRemainingEnemies() const;

	UFUNCTION(BlueprintPure, Category = "P02|Encounter")
	bool IsCleared() const { return bCleared; }

	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "P02|Encounter")
	TArray<TObjectPtr<AP02PlaytestEnemy>> EncounterEnemies;

	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "P02|Encounter")
	TObjectPtr<AP02PlaytestDoor> LinkedDoor;

private:
	UFUNCTION()
	void HandleTriggerOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep,
		const FHitResult& SweepResult);

	UFUNCTION()
	void HandleEnemyDestroyed(AActor* DestroyedActor);

	void BindEnemies();
	void ClearEncounter();

	UPROPERTY(VisibleAnywhere, Category = "P02|Encounter")
	TObjectPtr<UBoxComponent> TriggerBox;

	bool bActivated = false;
	bool bCleared = false;
};

/** Marker for the start of a measured playtest run. Placement is designer-owned. */
UCLASS(BlueprintType)
class P02DUNGEON_API AP02StartMarker : public AActor
{
	GENERATED_BODY()

public:
	AP02StartMarker();

private:
	UPROPERTY(VisibleAnywhere, Category = "P02|Start")
	TObjectPtr<USceneComponent> SceneRoot;
};

/** Completes a run after the required encounter has cleared. */
UCLASS(BlueprintType)
class P02DUNGEON_API AP02EndTrigger : public AActor
{
	GENERATED_BODY()

public:
	AP02EndTrigger();
	virtual void BeginPlay() override;
	void SetRequiredEncounter(AP02EncounterTrigger* InEncounter) { RequiredEncounter = InEncounter; }

private:
	UFUNCTION()
	void HandleEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep,
		const FHitResult& SweepResult);

	UPROPERTY(VisibleAnywhere, Category = "P02|End")
	TObjectPtr<UBoxComponent> TriggerBox;

	UPROPERTY(EditInstanceOnly, Category = "P02|End")
	TObjectPtr<AP02EncounterTrigger> RequiredEncounter;

	bool bTriggered = false;
};

/**
 * PIE-only playtest layer. It leaves map assets untouched and provides camera,
 * dash, basic damage, timing, distance logging, and a transient end-to-end test course.
 */
UCLASS()
class P02DUNGEON_API UP02PlaytestSubsystem : public UTickableWorldSubsystem
{
	GENERATED_BODY()

public:
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
	virtual void OnWorldBeginPlay(UWorld& InWorld) override;
	virtual void Tick(float DeltaSeconds) override;
	virtual TStatId GetStatId() const override;

	void NotifyEncounterStarted(int32 EnemyCount);
	void NotifyEncounterUpdated(int32 EnemyCount);
	void NotifyEncounterCleared();
	void CompleteRun();
	void ApplyPlayerDamage(float DamageAmount);

	float GetPlayerHealth() const { return PlayerHealth; }
	float GetElapsedTime() const;
	float GetTravelDistance() const { return TravelDistance; }
	bool IsRunComplete() const { return bRunComplete; }

private:
	void TryInitializePlayer();
	void TickDash(float DeltaSeconds);
	void TryBasicAttack();
	void DrawPlaytestHUD() const;
	void SpawnTransientTestCourse();
	void ResetPlayerAfterDefeat();
	AActor* SpawnTestBlock(const FName& ActorName, const FVector& Location, const FVector& Scale);

	TWeakObjectPtr<APlayerController> PlayerController;
	TWeakObjectPtr<APawn> PlayerPawn;
	TWeakObjectPtr<AP02QuarterViewCameraRig> CameraRig;
	TWeakObjectPtr<AP02EncounterTrigger> ActiveEncounter;

	FVector StartLocation = FVector::ZeroVector;
	FVector LastPlayerLocation = FVector::ZeroVector;
	FVector DashDirection = FVector::ForwardVector;
	float RunStartTime = 0.0f;
	float RunEndTime = 0.0f;
	float TravelDistance = 0.0f;
	float PlayerHealth = 100.0f;
	float NextDashTime = 0.0f;
	float DashEndTime = 0.0f;
	float NextAttackTime = 0.0f;
	int32 EncounterRemaining = 0;
	bool bPlayerInitialized = false;
	bool bCourseSpawned = false;
	bool bIsDashing = false;
	bool bEncounterActive = false;
	bool bEncounterCleared = false;
	bool bRunComplete = false;
};
