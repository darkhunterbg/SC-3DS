#include "UnitDatabase.h"
#include "Platform.h"
#include "GraphicsDatabase.h"
#include "WeaponDatabase.h"
#include "Generated.h"
#include "../Entity/EntityUtil.h"

#include "../Game.h"

UnitDef UnitDatabase::Marine;
UnitDef UnitDatabase::SCV;
UnitDef UnitDatabase::CommandCenter;
UnitDef UnitDatabase::MineralField1;

std::vector<UnitDef*> UnitDatabase::Units = {
	&Marine, &SCV, &CommandCenter, &MineralField1
};

static void MarineData() {
	UnitDef& u = UnitDatabase::Marine;
	u.Name = "Terran Marine";
	u.Title = "Private";
	u.Health = 40;
	u.MovementSpeed = 4;
	u.RotationSpeed = 1;
	u.SetUseSupply(1);
	u.Vision = 7;
	u.Weapon = &WeaponDatabase::GaussRifle;
	u.MineralCost = 50;

	u.Sounds.Death = { "sound/terran/marine/tmadth", 2 };
	u.Sounds.What = { "sound/terran/marine/tmawht", 4 };
	u.Sounds.Yes = { "sound/terran/marine/tmayes", 4 };
	u.Sounds.Annoyed = { "sound/terran/marine/tmapss", 7 };
	u.Sounds.Ready = { "sound/terran/marine/tmardy", 1 };
	u.Graphics = &GraphicsDatabase::Marine;

	u.AudioPriority = 1;

	u.ArmorIconId = 292;
	u.PortraitId = 0;
	u.IconId = 0;

	u.BuildTime = TimeUtil::SecondsTime(24);
}
static void SCVData() {
	UnitDef& u = UnitDatabase::SCV;
	u.Name = "Terran SCV";
	u.Title = "Private";
	u.Health = 60;
	u.MovementSpeed = 5;
	u.RotationSpeed = 1;
	u.SetUseSupply(1);
	u.Vision = 7;
	u.Weapon = &WeaponDatabase::FusionCutter;
	u.MineralCost = 50;

	u.Sounds.Death = { "sound/terran/scv/tscdth", 1 };
	u.Sounds.What = { "sound/terran/scv/tscwht", 4 };
	u.Sounds.Yes = { "sound/terran/scv/tscyes", 4 };
	u.Sounds.Annoyed = { "sound/terran/scv/tscpss", 7 };
	u.Sounds.Ready = { "sound/terran/scv/tscrdy", 1 };
	u.Graphics = &GraphicsDatabase::SCV;

	u.ArmorIconId = 292;
	u.PortraitId = 1;
	u.IconId = 7;

	u.BuildTime = TimeUtil::SecondsTime(20);
}
static void CommandCenterData() {
	UnitDef& u = UnitDatabase::CommandCenter;
	u.Name = "Terran Command Center";
	u.Health = 1500;
	u.MovementSpeed = 0;
	u.RotationSpeed = 0;
	u.Vision = 10;
	u.Armor = 1;
	u.SetPovideSupply(10);
	u.IsBuilding = true;
	u.MineralCost = 400;

	u.Sounds.Death = { "sound/misc/explo4", 1 , true };
	u.Sounds.What = { "sound/misc/button", 1 , true };
	u.Graphics = &GraphicsDatabase::CommandCenter;

	u.PortraitId = 2;

	u.BuildTime = TimeUtil::SecondsTime(120);

	u.ProductionUnit = &UnitDatabase::SCV;
	u.IconId = 106;

	u.SpawnOffset = { 36,56 };
}
static void MineralField1Data() {
	UnitDef& u = UnitDatabase::MineralField1;
	u.Name = "Mineral Field";
	u.Health = 100000;
	u.MovementSpeed = 0;
	u.RotationSpeed = 0;
	u.Vision = 0;

	u.IsBuilding = true;
	u.IsResourceContainer = true;

	u.Sounds.What = { "sound/misc/button", 1 , true };
	u.Graphics = &GraphicsDatabase::Minerals1;

	u.PortraitId = 3;
}

void UnitDatabase::Init()
{
	MarineData();
	SCVData();
	CommandCenterData();
	MineralField1Data();
	int i = 0;

	for (auto unit : Units) {
		unit->Id = i++;
	}
}

void UnitDatabase::LoadUnitResources(UnitDef& def) {
	def.Graphics->LoadResourcesAction();

	if (def.HasArmor())
		def.ArmorIcon = SpriteDatabase::Load_unit_cmdbtns_cmdicons()
		->GetFrame(def.ArmorIconId).sprite;

	def.Icon = SpriteDatabase::Load_unit_cmdbtns_cmdicons()
		->GetFrame(def.IconId);

	def.Portrait = Game::AssetLoader.LoadAtlas("portrait.t3x")->GetSprite(def.PortraitId);

	def.LoadAllSounds();
}

void UnitDatabase::LoadAllUnitResources()
{
	for (auto unit : Units) {
		LoadUnitResources(*unit);
	}
}
