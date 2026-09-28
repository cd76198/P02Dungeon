#include "P02ObservationPoint.h"
#include "P02FieldTelemetry.h"
#include "P02PlaytestSystem.h"
#include "Blueprint/WidgetTree.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Components/Border.h"
#include "Components/BoxComponent.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/SizeBox.h"
#include "Components/ScaleBox.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "Engine/Font.h"
#include "Engine/Texture2D.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "InputCoreTypes.h"
#include "Kismet/GameplayStatics.h"

void UP02ObservationImageWidget::Configure(AP02ObservationPoint* Point, UTexture2D* Image, bool bPlaceholder)
{
    OwnerPoint = Point; DisplayImage = Image; bTemporaryImage = bPlaceholder;
    SetIsFocusable(true);
}

TSharedRef<SWidget> UP02ObservationImageWidget::RebuildWidget()
{
    if (!WidgetTree) WidgetTree=NewObject<UWidgetTree>(this);
    if (WidgetTree->RootWidget) return Super::RebuildWidget();
    if (UObject* Face = LoadObject<UObject>(nullptr, TEXT("/Game/Portfolio02/UI/ONE_Mobile_POP.ONE_Mobile_POP")))
    {
        KoreanFont = NewObject<UFont>(this);
        KoreanFont->FontCacheType = EFontCacheType::Runtime;
        FTypefaceEntry& Entry = KoreanFont->CompositeFont.DefaultTypeface.Fonts.AddDefaulted_GetRef();
        Entry.Name = TEXT("Regular"); Entry.Font = FFontData(Face);
    }
    UBorder* Back = WidgetTree->ConstructWidget<UBorder>();
    Back->SetBrushColor(FLinearColor(.015f,.015f,.02f,.98f));
    Back->SetHorizontalAlignment(HAlign_Center); Back->SetVerticalAlignment(VAlign_Center);
    WidgetTree->RootWidget = Back;
    UScaleBox* Scale = WidgetTree->ConstructWidget<UScaleBox>(); Scale->SetStretch(EStretch::ScaleToFit); Back->SetContent(Scale);
    USizeBox* ContentSize = WidgetTree->ConstructWidget<USizeBox>(); ContentSize->SetWidthOverride(1080); ContentSize->SetHeightOverride(740); Scale->SetContent(ContentSize);
    UVerticalBox* Stack = WidgetTree->ConstructWidget<UVerticalBox>(); ContentSize->SetContent(Stack);
    auto Text = [this, Stack](const FString& Value, int32 Size)
    {
        UTextBlock* Label = WidgetTree->ConstructWidget<UTextBlock>();
        Label->SetText(FText::FromString(Value)); Label->SetJustification(ETextJustify::Center);
        if (KoreanFont) Label->SetFont(FSlateFontInfo(KoreanFont, Size));
        UVerticalBoxSlot* Slot = Stack->AddChildToVerticalBox(Label); Slot->SetPadding(FMargin(16,8));
    };
    Text(TEXT("틈새 살펴보기"),28);
    Text(bTemporaryImage ? TEXT("임시 이미지 · 실제 던전 캡처 미연결") : TEXT("성당 내부의 일부"),18);
    USizeBox* Frame = WidgetTree->ConstructWidget<USizeBox>();
    Frame->SetWidthOverride(1000); Frame->SetHeightOverride(562.5f);
    if (DisplayImage)
    {
        UImage* Image = WidgetTree->ConstructWidget<UImage>(); Image->SetBrushFromTexture(DisplayImage);
        Frame->SetContent(Image);
    }
    else
    {
        UTextBlock* Missing = WidgetTree->ConstructWidget<UTextBlock>();
        Missing->SetText(FText::FromString(TEXT("최종 색 입력 장치 — 캡처 준비 중")));
        if (KoreanFont) Missing->SetFont(FSlateFontInfo(KoreanFont,26));
        Frame->SetContent(Missing);
    }
    Stack->AddChildToVerticalBox(Frame)->SetHorizontalAlignment(HAlign_Center);
    UButton* Close = WidgetTree->ConstructWidget<UButton>();
    UTextBlock* CloseLabel = WidgetTree->ConstructWidget<UTextBlock>();
    CloseLabel->SetText(FText::FromString(TEXT("닫기  [E / Esc]")));
    if (KoreanFont) CloseLabel->SetFont(FSlateFontInfo(KoreanFont,22));
    CloseLabel->SetColorAndOpacity(FSlateColor(FLinearColor::Black));
    Close->SetContent(CloseLabel); Close->OnClicked.AddDynamic(this,&UP02ObservationImageWidget::CloseClicked);
    Stack->AddChildToVerticalBox(Close)->SetPadding(FMargin(16,14));
    return Super::RebuildWidget();
}

FReply UP02ObservationImageWidget::NativeOnKeyDown(const FGeometry& Geometry, const FKeyEvent& Event)
{
    if (Event.IsRepeat()) return FReply::Handled();
    if (Event.GetKey()==EKeys::E || Event.GetKey()==EKeys::Escape)
    {
        CloseClicked(); return FReply::Handled();
    }
    return FReply::Handled(); // Keep gameplay keys inside the optional image screen.
}
void UP02ObservationImageWidget::CloseClicked()
{
    if (OwnerPoint.IsValid()) OwnerPoint->CloseObservation();
}

AP02ObservationPoint::AP02ObservationPoint()
{
    PrimaryActorTick.bCanEverTick = true;
    InteractionArea = CreateDefaultSubobject<UBoxComponent>(TEXT("ObservationRange"));
    SetRootComponent(InteractionArea); InteractionArea->SetBoxExtent(FVector(650,230,130));
    InteractionArea->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

bool AP02ObservationPoint::CanObserve(APawn* Pawn) const
{
    if (!IsValid(Pawn) || Pawn->GetWorld()!=GetWorld()) return false;
    const FVector Local = InteractionArea->GetComponentTransform().InverseTransformPosition(Pawn->GetActorLocation());
    const FVector Extent = InteractionArea->GetUnscaledBoxExtent();
    if (FMath::Abs(Local.X)>Extent.X || FMath::Abs(Local.Y)>Extent.Y || FMath::Abs(Local.Z)>Extent.Z) return false;
    FHitResult Hit; FCollisionQueryParams Params(SCENE_QUERY_STAT(ObservationSight),false,Pawn);
    Params.AddIgnoredActor(this);
    const FVector Target = GetActorTransform().TransformPosition(SightTargetOffset);
    return !GetWorld()->LineTraceSingleByChannel(Hit,Pawn->GetActorLocation()+FVector(0,0,75),Target,ECC_Visibility,Params);
}

void AP02ObservationPoint::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
    if (Viewer) return;
    APawn* Pawn=UGameplayStatics::GetPlayerPawn(this,0);
    APlayerController* PC=UGameplayStatics::GetPlayerController(this,0);
    const bool bEligible=CanObserve(Pawn);
    if (bEligible!=bWasEligible)
    {
        UE_LOG(LogTemp,Display,TEXT("OBSERVATION eligible=%d pawn=%s"),bEligible,Pawn?*Pawn->GetActorLocation().ToCompactString():TEXT("none"));
        if(auto* Log=GetWorld()->GetSubsystem<UP02FieldTelemetry>()) Log->Event(bEligible?TEXT("observation_range_enter"):TEXT("observation_range_exit"));
        bWasEligible=bEligible;
    }
    if (!bEligible || !PC) return;
    if (AP02PlaytestHUD* HUD=Cast<AP02PlaytestHUD>(PC->GetHUD()))
        HUD->AddPrompt(Pawn->GetActorLocation()+FVector(0,0,160),TEXT("[E] 틈새 살펴보기"),FColor::White,.9f);
    if (PC->WasInputKeyJustPressed(EKeys::E)) OpenObservation();
}

void AP02ObservationPoint::OpenObservation()
{
    APlayerController* PC=UGameplayStatics::GetPlayerController(this,0);
    if (Viewer || !PC || !CanObserve(PC->GetPawn())) return;
    Viewer=CreateWidget<UP02ObservationImageWidget>(PC,UP02ObservationImageWidget::StaticClass());
    if (!Viewer) return;
    ObservingController=PC; bPreviousCursor=PC->bShowMouseCursor; bPreviousPaused=UGameplayStatics::IsGamePaused(this);
    if (ACharacter* Character=Cast<ACharacter>(PC->GetPawn())) Character->GetCharacterMovement()->StopMovementImmediately();
    PC->SetIgnoreMoveInput(true); PC->SetIgnoreLookInput(true);
    Viewer->Configure(this,ObservationImage,bPlaceholderImage); Viewer->AddToViewport(100);
    UWidgetBlueprintLibrary::SetInputMode_UIOnlyEx(PC,Viewer,EMouseLockMode::DoNotLock,true);
    PC->bShowMouseCursor=true; Viewer->SetKeyboardFocus();
    UGameplayStatics::SetGamePaused(this,true);
    if(auto* Log=GetWorld()->GetSubsystem<UP02FieldTelemetry>()) Log->Observation(true);
    UE_LOG(LogTemp,Display,TEXT("OBSERVATION opened placeholder=%d"),bPlaceholderImage);
}

void AP02ObservationPoint::CloseObservation()
{
    if (!Viewer) return;
    if(auto* Log=GetWorld()->GetSubsystem<UP02FieldTelemetry>()) Log->Observation(false);
    Viewer->RemoveFromParent(); Viewer=nullptr;
    if (APlayerController* PC=ObservingController.Get())
    {
        UGameplayStatics::SetGamePaused(this,bPreviousPaused);
        PC->SetIgnoreMoveInput(false); PC->SetIgnoreLookInput(false);
        PC->bShowMouseCursor=bPreviousCursor;
        UWidgetBlueprintLibrary::SetInputMode_GameOnly(PC,true);
    }
    ObservingController.Reset();
    UE_LOG(LogTemp,Display,TEXT("OBSERVATION closed; gameplay input restored"));
}
void AP02ObservationPoint::EndPlay(const EEndPlayReason::Type Reason)
{
    CloseObservation(); Super::EndPlay(Reason);
}
