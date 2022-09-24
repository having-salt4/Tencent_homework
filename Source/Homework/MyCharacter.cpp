// Fill out your copyright notice in the Description page of Project Settings.


#include "MyCharacter.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "Blueprint/UserWidget.h"
#include "UObject/UObjectGlobals.h"
#include "Net/UnrealNetwork.h"
#include "Engine/Engine.h"
#include "Projectile.h"

// Sets default values
AMyCharacter::AMyCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	ProjectileClass = AProjectile::StaticClass();

}

// Called when the game starts or when spawned
void AMyCharacter::BeginPlay()
{
	Super::BeginPlay();
	GetWorldTimerManager().SetTimer(MemberTimerHandle, this, &AMyCharacter::RecoverArmor, 1.0f, true, 1.0f);
}

// Called every frame
void AMyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT("Armor is %f"), Armor));

}

// Called to bind functionality to input
void AMyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	// 处理发射投射物
	//PlayerInputComponent->BindAction("Fire2", IE_Pressed, this, &AMyCharacter::HandleFire);

}

float AMyCharacter::TakeDamage(float DamageTaken, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) {
	if (Armor >= DamageTaken) {
		SetCurrentHealth(Health, Armor - DamageTaken);
		return Armor - DamageTaken;
	}
	else {
		SetCurrentHealth(Health - DamageTaken+Armor,Armor);
		SetCurrentHealth(Health, 0);
		return Health;
	}
}

UUserWidget* AMyCharacter::myDamage(TSubclassOf<UUserWidget> DamageWidgetClass) {
	UUserWidget* NewWidget = CreateWidget<UUserWidget>(GetWorld(), DamageWidgetClass);
	if (NewWidget != nullptr) {
		NewWidget->AddToViewport();
	}
	return NewWidget;
	
}
void AMyCharacter::RecoverArmor() {
	if (Armor >= 1)
	{
		SetCurrentHealth(Health, 1);
		GetWorldTimerManager().ClearTimer(MemberTimerHandle);
		// MemberTimerHandle现在可以复用于其他任意定时器。
	}
	else if(Health > 0){
		SetCurrentHealth(Health, Armor+0.01);
		
	}
}
void AMyCharacter::GetLifetimeReplicatedProps(TArray <FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	//复制当前生命值。
	DOREPLIFETIME(AMyCharacter, Health);
	//复制护甲
	DOREPLIFETIME(AMyCharacter, Armor);
}
void  AMyCharacter::OnHealthUpdate()
{
	//客户端特定的功能
	if (IsLocallyControlled())
	{
		FString healthMessage = FString::Printf(TEXT("You now have %f health remaining."), Health);
		FString armorMessage = FString::Printf(TEXT("You now have %f armor remaining."), Armor);
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, healthMessage);
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, armorMessage);
		if (Health <= 0)
		{
			FString deathMessage = FString::Printf(TEXT("You have been killed."));
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, deathMessage);
		}
	}

	//服务器特定的功能
	if (GetLocalRole() == ROLE_Authority)
	{
		FString healthMessage = FString::Printf(TEXT("%s now has %f health remaining."), *GetFName().ToString(), Health);
		FString armorMessage = FString::Printf(TEXT("%s now has %f armor remaining."), *GetFName().ToString(), Armor);
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, healthMessage);
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, armorMessage);
	}

	//在所有机器上都执行的函数。 
	/*
		因任何因伤害或死亡而产生的特殊功能都应放在这里。
	*/
}
void AMyCharacter::OnRep_CurrentHealth()
{
	OnHealthUpdate();
}
void  AMyCharacter::SetCurrentHealth(float healthValue,float armorValue)
{
	if (GetLocalRole() == ROLE_Authority)
	{
		Health = healthValue;
		Armor = armorValue;
		OnHealthUpdate();
	}
}
void AMyCharacter::HandleFire_Implementation()
{
	FVector spawnLocation = GetActorLocation() + (GetControlRotation().Vector() * 100.0f) + (GetActorUpVector() * 50.0f);
	FRotator spawnRotation = GetControlRotation();

	FActorSpawnParameters spawnParameters;
	spawnParameters.Instigator = GetInstigator();
	spawnParameters.Owner = this;

	AProjectile* spawnedProjectile = GetWorld()->SpawnActor<AProjectile>(spawnLocation, spawnRotation, spawnParameters);
	/*FString Message = FString::Printf(TEXT("1"));
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, Message);*/
}
void AMyCharacter::Fire() {
	HandleFire();
}