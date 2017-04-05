// Huge Credits to Detuks took like 99% of his code base :^)

#include "PluginSDK.h"

PluginSetup("nerfTeemo");

IMenu* MainMenu;
IMenu* comboMenu;
IMenu* harassMenu;
IMenu* laneclearMenu;
IMenu* jungleclearMenu;
IMenu* miscMenu;

IMenuOption* useQcombo;
IMenuOption* useAAQcombo;
IMenuOption* useWcombo;
IMenuOption* useRcombo;

IMenuOption* useQharass;
IMenuOption* useAAQharass;
IMenuOption* useWharass;
IMenuOption* useRharass;

IMenuOption* useQlaneclear;
IMenuOption* useAAQlaneclear;
IMenuOption* useWlaneclear;
IMenuOption* useRlaneclear;
IMenuOption* useRminminion;

IMenuOption* useQjungleclear;
IMenuOption* useAAQjungleclear;
IMenuOption* useWjungleclear;
IMenuOption* useRjungleclear;
IMenuOption* useRminmonster;

IUnit* myHero;

ISpell2* Q;
ISpell2* W;
ISpell2* R;


ISpell* Ignite;

int lastR;
float Rrange;

void  Menu()
{
	MainMenu = GPluginSDK->AddMenu("nerfTeemo");

	comboMenu = MainMenu->AddMenu("Combo Menu");
	useQcombo = comboMenu->CheckBox("Use Q in Combo", true);
	useAAQcombo = comboMenu->CheckBox("Use Q after AA", true);
	useWcombo = comboMenu->CheckBox("Use W in Combo", true);
	useRcombo = comboMenu->CheckBox("Use R in Combo", true);

	harassMenu = MainMenu->AddMenu("Harass Menu");
	useQharass = harassMenu->CheckBox("Use Q in Harass", true);
	useAAQharass = harassMenu->CheckBox("Use Q after AA", true);
	useWharass = harassMenu->CheckBox("Use W in Harass", false);
	useRharass = harassMenu->CheckBox("Use R in Harass", false);

	laneclearMenu = MainMenu->AddMenu("Lane Clear Menu");
	useQlaneclear = laneclearMenu->CheckBox("Use Q in Lane Clear", true);
	useAAQlaneclear = laneclearMenu->CheckBox("Use Q after AA", true);
	useWlaneclear = laneclearMenu->CheckBox("Use W in Lane Clear", false);
	useRlaneclear = laneclearMenu->CheckBox("Use R in Lane Clear", false);
	useRminminion = laneclearMenu->AddInteger("Min. Minions for R", 1, 10, 3);

	jungleclearMenu = MainMenu->AddMenu("Jungle Clear Menu");
	useQjungleclear = jungleclearMenu->CheckBox("Use Q in Lane Clear", true);
	useAAQjungleclear = jungleclearMenu->CheckBox("Use Q after AA", true);
	useWjungleclear = jungleclearMenu->CheckBox("Use W in Lane Clear", false);
	useRjungleclear = jungleclearMenu->CheckBox("Use R in Lane Clear", false);
	useRminmonster = jungleclearMenu->AddInteger("Min. Monster for R", 1, 10, 3);

	miscMenu = MainMenu->AddMenu("Misc Menu");
}

void LoadSpells()
{
	Q = GPluginSDK->CreateSpell2(kSlotQ, kTargetCast, false, false, static_cast<eCollisionFlags>(kCollidesWithYasuoWall));
	W = GPluginSDK->CreateSpell2(kSlotW, kTargetCast, false, false, static_cast<eCollisionFlags>(kCollidesWithNothing));
	R = GPluginSDK->CreateSpell2(kSlotR, kCircleCast, true, true, static_cast<eCollisionFlags>(kCollidesWithYasuoWall));

	//R->SetOverrideRange(Rrange);
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

static bool IsImmune(IUnit* target)
{
	return target->HasBuff("BlackShield") || target->HasBuff("SivirE") || target->HasBuff("NocturneShroudofDarkness") ||
		target->HasBuff("deathdefiedbuff");

}

/*static bool IsShroomed(IUnit* target)
{
	// todo : everything
	return;
}*/

void UseItems()
{

}

void Combo()
{
	if (useQcombo->Enabled() && Q->IsReady() && !useAAQcombo->Enabled())
	{
		auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, Q->Range());
		if (myHero->IsValidTarget(target, Q->Range()) && !IsImmune(target))
		{
			Q->CastOnTarget(target);
		}
	}
	if (useWcombo->Enabled() && W->IsReady())
	{
		auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, 1000);
		if (myHero->IsValidTarget(target, 1000))
		{
			W->CastOnPlayer();
		}
	}
	if (useRcombo->Enabled() && R->IsReady())
	{
		auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, 900);
		if (myHero->IsValidTarget(target, 400) && myHero->GetLevel() >= 6 && myHero->GetLevel() <= 10 /*&& GGame->CurrentTick() - lastR > 1000*/)
		{
			R->CastOnTarget(target);
			//lastR = GGame->CurrentTick();
		}
		if (myHero->IsValidTarget(target, 650) && myHero->GetLevel() >= 11 && myHero->GetLevel() <= 15 /*&& GGame->CurrentTick() - lastR > 1000*/)
		{
			R->CastOnTarget(target);
			//lastR = GGame->CurrentTick();
		}
		if (myHero->IsValidTarget(target, 900) && myHero->GetLevel() >= 16 /*&& GGame->CurrentTick() - lastR > 1000*/)
		{
			R->CastOnTarget(target);
			//lastR = GGame->CurrentTick();
		}
	}
}

void Harass()
{
	if (useQcombo->Enabled() && Q->IsReady() && !useAAQharass->Enabled())
	{
		auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, Q->Range());
		if (myHero->IsValidTarget(target, Q->Range()) && !IsImmune(target))
		{
			Q->CastOnTarget(target);
		}
	}
	if (useWharass->Enabled() && W->IsReady())
	{
		auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, 1000);
		if (myHero->IsValidTarget(target, 1000))
		{
			W->CastOnPlayer();
		}
	}
	if (useRharass->Enabled() && R->IsReady())
	{
		auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, 900);
		if (myHero->IsValidTarget(target, 400) && myHero->GetLevel() >= 6 && myHero->GetLevel() <= 10 /*&& lastR > 1000*/)
		{
			R->CastOnTarget(target);
			//lastR = GGame->CurrentTick();
		}
		if (myHero->IsValidTarget(target, 650) && myHero->GetLevel() >= 11 && myHero->GetLevel() <= 15 /*&& lastR > 1000*/)
		{
			R->CastOnTarget(target);
			//lastR = GGame->CurrentTick();
		}
		if (myHero->IsValidTarget(target, 900) && myHero->GetLevel() >= 16 /*&& lastR > 1000*/)
		{
			R->CastOnTarget(target);
			//lastR = GGame->CurrentTick();
		}
	}
}


void killsteal()
{

}

PLUGIN_EVENT(void) OnGapcloser(GapCloserSpell const& args)
{

}

PLUGIN_EVENT(void) OnInterruptable(InterruptibleSpell const& Args)
{

}

PLUGIN_EVENT(void) OnAfterAttack(IUnit* source, IUnit* target)
{
	if (GOrbwalking->GetOrbwalkingMode() == kModeCombo && useAAQcombo->Enabled())
	{
		for (auto enemy : GEntityList->GetAllHeros(false, true))
		{
			if (myHero->IsValidTarget(enemy, 650))
			{
				Q->CastOnTarget(enemy);
			}
		}
	}

	if (GOrbwalking->GetOrbwalkingMode() == kModeMixed && useAAQharass->Enabled())
	{
		for (auto enemy : GEntityList->GetAllHeros(false, true))
		{
			if (myHero->IsValidTarget(enemy, 650))
			{
				Q->CastOnTarget(enemy);
			}
		}
	}

	if (GOrbwalking->GetOrbwalkingMode() == kModeLaneClear && useAAQlaneclear->Enabled())
	{
		for (auto minion : GEntityList->GetAllMinions(false, true, false))
		{
			if (myHero->IsValidTarget(minion, 650))
			{
				Q->CastOnTarget(minion);
			}
		}
	}

	if (GOrbwalking->GetOrbwalkingMode() == kModeLaneClear && useAAQjungleclear->Enabled())
	{
		for (auto monster : GEntityList->GetAllMinions(false, false, true))
		{
			if (myHero->IsValidTarget(monster, 650))
			{
				Q->CastOnTarget(monster);
			}
		}
	}
}

PLUGIN_EVENT(void) OnBeforeAttack(IUnit* source, IUnit* target)
{

}

PLUGIN_EVENT(void) OnRender()
{
	//if (R->IsReady()) { GRender->DrawOutlinedCircle(GEntityList->Player()->GetPosition(), Vec4(255, 255, 0, 255), Rrange); }
}


PLUGIN_EVENT(void) OnGameUpdate()
{
	if (GGame->IsChatOpen()) return;
	/*if (myHero->GetLevel() >= 6 && myHero->GetLevel() <= 10)
	{
		Rrange = 400;
	}
	if (myHero->GetLevel() >= 11 && myHero->GetLevel() <= 15)
	{
		Rrange = 650;
	}
	if (myHero->GetLevel() >= 16)
	{
		Rrange = 900;
	}*/
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

	if (strcmp(GEntityList->Player()->ChampionName(), "Teemo") == 0)
	{
		GGame->PrintChat("nerfTeemo: Loaded");
	}
	else
	{
		GGame->PrintChat("You are not playing Teemo...");
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