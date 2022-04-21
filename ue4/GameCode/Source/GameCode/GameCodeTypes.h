#pragma once



UENUM(BlueprintType)
enum class EEquipableItemType : uint8
{
	None = 0,
	Pistol = 1,
	Rifle = 2,
	Throwable = 3,
	Melee = 4,
	Bow = 5,
	Max
};

UENUM(BlueprintType)
enum class EAmmoType : uint8
{
	None = 0,
	Pistol = 1,
	Rifle = 2,
	ShotgunShells = 3,
	FragGrenades = 4,
	RifleGrenades = 5,
	Arrows = 6,
	Max
};

UENUM(BlueprintType)
enum class EEquipmentSlots : uint8
{
	None = 0,
	SideArm = 1,
	PrimaryWeapon = 2,
	SecondaryWeapon = 3,
	PrimaryItemSlot = 4,
	MeleeWeapon = 5,
	Max UMETA(Hidden)
};

UENUM(BlueprintType)
enum class EMeleeAttackType : uint8
{
	None = 0,
	PrimaryAttack = 1,
	SecondaryAttack = 2,
	Max UMETA(Hidden)
};

UENUM(BlueprintType)
enum class EReticleType : uint8
{
	None = 0,
	Default = 1,
	SniperRifle = 2,
	Bow = 3,
	Max UMETA(Hidden)
};
UENUM(BlueprintType)
enum class EWeaponFireMode : uint8
{
	Single = 0,
	FullAuto = 1
};
UENUM(BlueprintType)
enum class ETeams : uint8
{
	Player = 0,
	Enemy = 1
};


#define ECC_Climbing ECC_GameTraceChannel1
#define ECC_InteractionVolume ECC_GameTraceChannel2
#define ECC_WallRunnable ECC_GameTraceChannel3
#define ECC_Bullet ECC_GameTraceChannel4
#define ECC_Melee ECC_GameTraceChannel5
const FName DebugCategoryLedgeDetection = FName("LedgeDetection");
const FName DebugCategoryCharacterAttributes = FName("CharacterAttributes");
const FName DebugCategoryRangeWeapon = FName("RangeWeapon");
const FName DebugCategoryMeleeWeapon = FName("MeleeWeapon");
const FName FXParamTraceEnd = FName("TraceEnd");
const FName CollisionProfilePawn = FName("Pawn");
const FName CollisionProfileRagdoll = FName("Ragdoll");
const FName CollisionProfileNoCollision = FName("NoCollision");
const FName CollisionProfilePawnInteractionVolume = FName("PawnInteractionVolume");
const FName SocketFPCamera = FName("CameraSocket");
const FName SocketCharacterWeapon = FName("CharacterWeaponSocket");
const FName SocketWeaponMuzzle = FName("Muzzle_Socket");
const FName SocketWeaponForeGrip = FName("ForeGripSocket");
const FName SocketCharacterThrowable = FName("GrenadeSocket");

const FName SectionMontageReloadEnd = FName("ReloadEnd");

const FName BB_CurrentTarget = FName("CurrentTarget");
const FName BB_NextLocation = FName("NextLocation");

const FName ActionInteract = FName("Interact");

const FVector Hell(-666, -666, -666);