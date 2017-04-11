// Huge Credits to Diabaths took like 99% of his code base :^)

#include "PluginSDK.h"

PluginSetup("nerfTryndamere");

IMenu* MainMenu;
IMenu* QMenu;
IMenu* WMenu;
IMenu* EMenu;
IMenu* RMenu;

IMenuOption* autoQ;
IMenuOption* autoQpercent;
IMenuOption* autoQfury;

IMenuOption* comboW;
IMenuOption* harassW;

IMenuOption* comboE;

IMenuOption* autoR;
IMenuOption* autoRpercent;


IUnit* myHero;

ISpell2* Q;
ISpell2* W;
ISpell2* E;
ISpell2* R;


ISpell* Ignite;


IInventoryItem* blade;
IInventoryItem* Cutlass;
IInventoryItem* HealthPot;
IInventoryItem* CorruptPot;
IInventoryItem* Biscuit;
IInventoryItem* RefillPot;
IInventoryItem* hunter;
IInventoryItem* tiamat;
IInventoryItem* rhydra;
IInventoryItem* thydra;

void  Menu()
{
	MainMenu = GPluginSDK->AddMenu("nerfTryndamere");

	QMenu = MainMenu->AddMenu("Q Settings");
	autoQ = QMenu->CheckBox("Auto Heal Q", true);
	autoQpercent = QMenu->AddFloat("Health Percent for Q", 1, 100, 70);
	autoQfury = QMenu->AddFloat("Min. Fury Percent", 0, 100, 0);

	WMenu = MainMenu->AddMenu("W Settings");
	comboW = WMenu->CheckBox("Use W in Combo", true);
	harassW = WMenu->CheckBox("Use W in Harass", true);

	EMenu = MainMenu->AddMenu("E Settings");
	comboE = EMenu->CheckBox("Use E in Combo", true);

	RMenu = MainMenu->AddMenu("R Settings");
	autoR = RMenu->CheckBox("Auto use R", true);
	autoRpercent = RMenu->AddFloat("Health Percent for R", 1, 100, 20);
}

void LoadSpells()
{
	Q = GPluginSDK->CreateSpell2(kSlotQ, kTargetCast, false, false, static_cast<eCollisionFlags>(kCollidesWithNothing));
	W = GPluginSDK->CreateSpell2(kSlotW, kTargetCast, false, false, static_cast<eCollisionFlags>(kCollidesWithNothing));
	E = GPluginSDK->CreateSpell2(kSlotE, kLineCast, false, true, static_cast<eCollisionFlags>(kCollidesWithNothing));
	R = GPluginSDK->CreateSpell2(kSlotR, kTargetCast, false, false, static_cast<eCollisionFlags>(kCollidesWithNothing));



	auto slot1 = GPluginSDK->GetEntityList()->Player()->GetSpellName(kSummonerSlot1);
	auto slot2 = GPluginSDK->GetEntityList()->Player()->GetSpellName(kSummonerSlot2);

	if (strcmp(slot1, "SummonerDot") == 0)
	{
		Ignite = GPluginSDK->CreateSpell(kSummonerSlot1, 600);
	}
	if (strcmp(slot2, "SummonerDot") == 0)
	{
		Ignite = GPluginSDK->CreateSpell(kSummonerSlot2, 600);
	}
	else Ignite == nullptr;

	tiamat = GPluginSDK->CreateItemForId(3077, 0);
	rhydra = GPluginSDK->CreateItemForId(3074, 0);
	thydra = GPluginSDK->CreateItemForId(3748, 0);
}

int EnemiesInRange(IUnit* Source, float range)
{
	auto Targets = GEntityList->GetAllHeros(false, true);
	auto enemiesInRange = 0;

	for (auto target : Targets)
	{
		if (target != nullptr)
		{
			auto flDistance = (target->GetPosition() - Source->GetPosition()).Length();
			if (flDistance < range)
			{
				enemiesInRange++;
			}
		}
	}
	return enemiesInRange;
}

float GetDistance(IUnit* Player, IUnit* target)
{
	return (Player->GetPosition() - target->GetPosition()).Length2D();
}

int CountEnemiesInRange(float range)
{
	int enemies = 0;
	for (auto enemy : GEntityList->GetAllHeros(false, true))
	{
		if (enemy != nullptr && GetDistance(GEntityList->Player(), enemy) <= range)
		{
			enemies++;
		}
	}
	return enemies;
}

int CountMinionInRange(float range)
{
	int minions = 0;
	for (auto minion : GEntityList->GetAllMinions(false, true, false))
	{
		if (minion != nullptr && GetDistance(GEntityList->Player(), minion) <= range)
		{
			minions++;
		}
	}
	return minions;
}

void UseItems()
{

}

void Combo()
{
	if (comboE->Enabled())
	{
		auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, 660);
		if (E->IsReady() && E->Range())
		{			
			if (target != nullptr)
			{
				if (target->HealthPercent() <= 10)
				{
					E->CastOnTarget(target);
				}
				if (GDamage->GetSpellDamage(GEntityList->Player(), target, kSlotE) >= target->GetHealth())
				{
					E->CastOnTarget(target);
				}
				if ((target->GetPosition() - GEntityList->Player()->GetPosition()).Length() > 400 && (target->GetPosition() - GEntityList->Player()->GetPosition()).Length() < E->Range())
				{
					E->CastOnTarget(target);
				}
			}
		}
	}
	if (comboW->Enabled())
	{
		auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, 1000);
		if (W->IsReady() && W->Range() && !target->IsFacing(myHero))
		{
			W->CastOnTarget(target);
		}
	}
}

void Harass()
{
	if (harassW->Enabled())
	{
		auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, 1000);
		if (W->IsReady() && W->Range() && !target->IsFacing(myHero))
		{
			W->CastOnTarget(target);
		}
	}
}


void killsteal()
{

}

void tryndaR()
{
	if (autoR->Enabled() && !myHero->IsDead() && myHero->HealthPercent() <= autoRpercent->GetFloat())
	{
		R->CastOnPlayer();
	}
}

void tryndaQ()
{
	if (autoQ->Enabled() && !myHero->IsDead() && myHero->HealthPercent() <= autoQpercent->GetFloat() && myHero->ManaPercent() <= autoQfury->GetFloat())
	{
		Q->CastOnPlayer();
	}
}

PLUGIN_EVENT(void) OnGapcloser(GapCloserSpell const& args)
{

}
PLUGIN_EVENT(void) OnInterruptable(InterruptibleSpell const& Args)
{

}

PLUGIN_EVENT(void) OnAfterAttack(IUnit* source, IUnit* target)
{
	if (GOrbwalking->GetOrbwalkingMode() == kModeCombo)
	{
		for (auto enemy : GEntityList->GetAllHeros(false, true))
		{
			if (myHero->IsValidTarget(enemy, 250))
			{
					if (tiamat->IsOwned() && tiamat->IsReady())
						tiamat->CastOnPlayer();
					if (rhydra->IsOwned() && rhydra->IsReady())
						rhydra->CastOnPlayer();
					if (thydra->IsOwned() && thydra->IsReady())
						thydra->CastOnPlayer();
			}

		}
	}
}

PLUGIN_EVENT(void) OnBeforeAttack(IUnit* source, IUnit* target)
{

}

PLUGIN_EVENT(void) OnRender()
{

}


PLUGIN_EVENT(void) OnGameUpdate()
{
	if (GGame->IsChatOpen()) return;
	if (GOrbwalking->GetOrbwalkingMode() == kModeCombo)
	{
		Combo();
	}

	if (GOrbwalking->GetOrbwalkingMode() == kModeMixed)
	{
		Harass();
	}
	if (GOrbwalking->GetOrbwalkingMode() == kModeLaneClear)
	{

	}
	killsteal();
	UseItems();
	tryndaR();
	tryndaQ();
}

PLUGIN_API void OnLoad(IPluginSDK* PluginSDK)
{
	PluginSDKSetup(PluginSDK);
	Menu();
	LoadSpells();
	myHero = GEntityList->Player();

	GEventManager->AddEventHandler(kEventOnGameUpdate, OnGameUpdate);
	GEventManager->AddEventHandler(kEventOnRender, OnRender);
	GEventManager->AddEventHandler(kEventOrbwalkAfterAttack, OnAfterAttack);
	GEventManager->AddEventHandler(kEventOrbwalkBeforeAttack, OnBeforeAttack);

	if (strcmp(GEntityList->Player()->ChampionName(), "Tryndamere") == 0)
	{
		GGame->PrintChat("nerfTryndamere: Loaded");
	}
	else
	{
		GGame->PrintChat("You are not playing Tryndamere...");
	}
}


PLUGIN_API void OnUnload()
{
	MainMenu->Remove();
		
	GEventManager->RemoveEventHandler(kEventOnGameUpdate, OnGameUpdate);
	GEventManager->RemoveEventHandler(kEventOnRender, OnRender);
	GEventManager->RemoveEventHandler(kEventOrbwalkAfterAttack, OnAfterAttack);
	GEventManager->RemoveEventHandler(kEventOrbwalkBeforeAttack, OnBeforeAttack);
}
