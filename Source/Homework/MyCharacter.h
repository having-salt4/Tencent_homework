// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MyCharacter.generated.h"


UCLASS()
class HOMEWORK_API AMyCharacter : public ACharacter
{
	GENERATED_BODY()
private:
	FTimerHandle MemberTimerHandle;
public:
	UPROPERTY(BlueprintReadWrite, ReplicatedUsing = OnRep_CurrentHealth)
	float Health = 0.8;
	UPROPERTY(BlueprintReadWrite, ReplicatedUsing = OnRep_CurrentHealth)
	float Armor = 0.4;
public:
	// Sets default values for this character's properties
	AMyCharacter();

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay|Projectile")
		TSubclassOf<class AProjectile> ProjectileClass;
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	/** 响应要更新的生命值。修改后，立即在服务器上调用，并在客户端上调用以响应RepNotify*/
	void OnHealthUpdate();
	/** 用于生成投射物的服务器函数。*/
	UFUNCTION(Server, Reliable)
		void HandleFire();
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION(BlueprintCallable, Category = "UMG Game")
		UUserWidget* myDamage(TSubclassOf<UUserWidget> DamageWidgetClass);
	/** 承受伤害的事件。从APawn覆盖。*/
	UFUNCTION(BlueprintCallable, Category = "Health")
		float TakeDamage(float DamageTaken, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
	void RecoverArmor();
	/** RepNotify，用于同步对当前生命值所做的更改。*/
	UFUNCTION()
		void OnRep_CurrentHealth();
	/** 属性复制 */
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(BlueprintCallable, Category = "Health")
		void  SetCurrentHealth(float healthValue, float armorValue);
};
