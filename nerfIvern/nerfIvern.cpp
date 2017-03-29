// Huge Credits to Detuks took like 99% of his code base :^)

#include "PluginSDK.h"

PluginSetup("nerfIvern");

IMenu* MainMenu;
IMenu* QMenu;
IMenu* WMenu;
IMenu* EMenu;
IMenu* RMenu;
IMenu* PriorityE;
IMenu* DrawingsIvern;

IMenuOption* useQcombo;
IMenuOption* useQ2combo;
IMenuOption* useQmixed;
IMenuOption* autoQ;
IMenuOption* autoQD;
IMenuOption* autoQfury;

IMenuOption* useWcombo;
IMenuOption* useWharass;

IMenuOption* useEcombo;
IMenuOption* EShield1;
IMenuOption* EShield2;
IMenuOption* EShield3;
IMenuOption* EShield4;
IMenuOption* EShield5;
IMenuOption* ShieldE;
IMenuOption* ShieldEally;
IMenuOption* ShieldEmyself;
IMenuOption* ShieldEmy;
IMenuOption* DrawReady;
IMenuOption* DrawQ;
IMenuOption* DrawE;




IMenuOption* autoR;
IMenuOption* autoRpercent;

IUnit* Player;
IUnit* myHero;
IUnit* EShield01;
IUnit* EShield02;
IUnit* EShield03;
IUnit* EShield04;
IUnit* EShield05;

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



int lastq;
int lastq2;
int lastw;

void  Menu()
{
	MainMenu = GPluginSDK->AddMenu("nerfIvern");

	QMenu = MainMenu->AddMenu("Q Settings");
	useQcombo = QMenu->CheckBox("Use Q in Combo", true);
	useQ2combo = QMenu->CheckBox("Use Q2 in Combo", true);
	useQmixed = QMenu->CheckBox("Use Q in Mixed", true);
	autoQ = QMenu->CheckBox("Auto Q Immobile", true);
	autoQD = QMenu->CheckBox("Auto Q Dashing", true);

	WMenu = MainMenu->AddMenu("W Settings");
	useWcombo = WMenu->CheckBox("Use W in Combo", true);
	useWharass = WMenu->CheckBox("Use W in Harass", false);

	EMenu = MainMenu->AddMenu("E Settings");
	ShieldE = EMenu->CheckBox("Use auto E", true);
	ShieldEally = EMenu->AddInteger("Ally HP percent(if lower than)", 10, 100, 50);
	ShieldEmyself = EMenu->CheckBox("Use E on myself", true);
	ShieldEmy = EMenu->AddInteger("My HP percent(if lower than)", 10, 100, 40);
	PriorityE = MainMenu->AddMenu("E Priority");

	DrawingsIvern = MainMenu->AddMenu("Spell Drawings");
	DrawReady = DrawingsIvern->CheckBox("Draw Only Ready Spells", true);
	DrawQ = DrawingsIvern->CheckBox("Draw Q", true);
	DrawE = DrawingsIvern->CheckBox("Draw E", true);
}

void LoadSpells()
{
	Q = GPluginSDK->CreateSpell2(kSlotQ, kLineCast, true, false, static_cast<eCollisionFlags>(kCollidesWithHeroes | kCollidesWithMinions | kCollidesWithYasuoWall));
	W = GPluginSDK->CreateSpell2(kSlotW, kTargetCast, false, false, static_cast<eCollisionFlags>(kCollidesWithNothing));
	E = GPluginSDK->CreateSpell2(kSlotE, kTargetCast, false, false, static_cast<eCollisionFlags>(kCollidesWithNothing));
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

int CountallyInRange(float range)
{
	int allys = 0;
	for (auto ally : GEntityList->GetAllHeros(true, false))
	{
		if (ally != nullptr && GetDistance(GEntityList->Player(), ally) <= range)
		{
			allys++;
		}
	}
	return allys;
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

static bool IsImmune(IUnit* target)
{
	return target->HasBuff("BlackShield") || target->HasBuff("SivirE") || target->HasBuff("NocturneShroudofDarkness") ||
		target->HasBuff("deathdefiedbuff");

}


void ShieldEa()
{
	int indx = 0;
	auto Teamates = GEntityList->GetAllHeros(true, false);
	for (auto teamate : Teamates)
	{
		if (indx == 0)
		{
			indx++;
			EShield1 = PriorityE->CheckBox(teamate->ChampionName(), false);
			EShield01 = teamate;
		}
		else if (indx == 1)
		{
			indx++;
			EShield2 = PriorityE->CheckBox(teamate->ChampionName(), false);
			EShield02 = teamate;
		}
		else if (indx == 2)
		{
			indx++;
			EShield3 = PriorityE->CheckBox(teamate->ChampionName(), false);
			EShield03 = teamate;
		}
		else if (indx == 3)
		{
			indx++;
			EShield4 = PriorityE->CheckBox(teamate->ChampionName(), false);
			EShield04 = teamate;
		}
		else if (indx == 4)
		{
			EShield5 = PriorityE->CheckBox(teamate->ChampionName(), false);
			EShield05 = teamate;
			return;
		}
	}
}

void Shielding()
{
	for (auto Ally : GEntityList->GetAllHeros(true, false))
	{
		auto target = GTargetSelector->FindTarget(QuickestKill, PhysicalDamage, 2000);
		if (Ally != GEntityList->Player())		
		{
			if (EShield1 != nullptr && EShield1->Enabled() && EShield01->ChampionName() == Ally->ChampionName())
			{
				
				if (ShieldE->Enabled() && E->IsReady() && EShield01->IsValidTarget(GEntityList->Player(), E->Range()) && myHero->IsValidTarget(target, 2000))
				{
					if (EShield01->HealthPercent() < ShieldEally->GetInteger())
					{
						E->CastOnUnit(EShield01);
					}
					else if (EShield01->HealthPercent() > ShieldEally->GetInteger())
					{
						if (EShield02->IsValidTarget(GEntityList->Player(), E->Range()) && myHero->IsValidTarget(target, 2000))
						{
							if (EShield02->HealthPercent() < ShieldEally->GetInteger())
							{
								E->CastOnUnit(EShield02);
							}
						}
						if (EShield03->IsValidTarget(GEntityList->Player(), E->Range()) && myHero->IsValidTarget(target, 2000))
						{
							if (EShield03->HealthPercent() < ShieldEally->GetInteger())
							{
								E->CastOnUnit(EShield03);
							}
						}
						if (EShield04->IsValidTarget(GEntityList->Player(), E->Range()) && myHero->IsValidTarget(target, 2000))
						{
							if (EShield04->HealthPercent() < ShieldEally->GetInteger())
							{
								E->CastOnUnit(EShield04);
							}
						}
						if (EShield05->IsValidTarget(GEntityList->Player(), E->Range()) && myHero->IsValidTarget(target, 2000))
						{
							if (EShield05->HealthPercent() < ShieldEally->GetInteger())
							{
								E->CastOnUnit(EShield05);
							}
						}
					}

				}
			}

			else if (EShield1->Enabled() && !EShield01->IsValidTarget(GEntityList->Player(), E->Range()) && myHero->IsValidTarget(target, 2000))
			{
				if (ShieldE->Enabled() && E->IsReady() && Ally->IsValidTarget(GEntityList->Player(), E->Range()) && myHero->IsValidTarget(target, 2000))
				{
					if (Ally->HealthPercent() < ShieldEally->GetInteger())
					{
						if (Ally != nullptr)
						{
							E->CastOnUnit(Ally);
						}
					}
				}
			}
			if (EShield2 != nullptr && EShield2->Enabled() && EShield02->ChampionName() == Ally->ChampionName())
			{
				if (ShieldE->Enabled() && E->IsReady() && EShield02->IsValidTarget(GEntityList->Player(), E->Range()) && myHero->IsValidTarget(target, 2000))
				{
					if (EShield02->HealthPercent() < ShieldEally->GetInteger())
					{
						E->CastOnUnit(EShield02);
					}
					else if (EShield02->HealthPercent() > ShieldEally->GetInteger())
					{
						if (EShield01->IsValidTarget(GEntityList->Player(), E->Range()) && myHero->IsValidTarget(target, 2000))
						{
							if (EShield01->HealthPercent() < ShieldEally->GetInteger())
							{
								E->CastOnUnit(EShield01);
							}
						}
						if (EShield03->IsValidTarget(GEntityList->Player(), E->Range()) && myHero->IsValidTarget(target, 2000))
						{
							if (EShield03->HealthPercent() < ShieldEally->GetInteger())
							{
								E->CastOnUnit(EShield03);
							}
						}
						if (EShield04->IsValidTarget(GEntityList->Player(), E->Range()) && myHero->IsValidTarget(target, 2000))
						{
							if (EShield04->HealthPercent() < ShieldEally->GetInteger())
							{
								E->CastOnUnit(EShield04);
							}
						}
						if (EShield05->IsValidTarget(GEntityList->Player(), E->Range()) && myHero->IsValidTarget(target, 2000))
						{
							if (EShield05->HealthPercent() < ShieldEally->GetInteger())
							{
								E->CastOnUnit(EShield05);
							}
						}
					}

				}
			}

			else if (EShield2->Enabled() && !EShield02->IsValidTarget(GEntityList->Player(), E->Range()) && myHero->IsValidTarget(target, 2000))
			{
				if (ShieldE->Enabled() && E->IsReady() && Ally->IsValidTarget(GEntityList->Player(), E->Range()) && myHero->IsValidTarget(target, 2000))
				{
					if (Ally->HealthPercent() < ShieldEally->GetInteger())
					{
						if (Ally != nullptr)
						{
							E->CastOnUnit(Ally);
						}
					}
				}
			}
			if (EShield3 != nullptr && EShield3->Enabled() && EShield03->ChampionName() == Ally->ChampionName())
			{
				if (ShieldE->Enabled() && E->IsReady() && EShield03->IsValidTarget(GEntityList->Player(), E->Range()) && myHero->IsValidTarget(target, 2000))
				{
					if (EShield03->HealthPercent() < ShieldEally->GetInteger())
					{
						E->CastOnUnit(EShield03);
					}
					else if (EShield03->HealthPercent() > ShieldEally->GetInteger())
					{
						if (EShield01->IsValidTarget(GEntityList->Player(), E->Range()) && myHero->IsValidTarget(target, 2000))
						{
							if (EShield01->HealthPercent() < ShieldEally->GetInteger())
							{
								E->CastOnUnit(EShield01);
							}
						}
						if (EShield02->IsValidTarget(GEntityList->Player(), E->Range()) && myHero->IsValidTarget(target, 2000))
						{
							if (EShield02->HealthPercent() < ShieldEally->GetInteger())
							{
								E->CastOnUnit(EShield02);
							}
						}
						if (EShield04->IsValidTarget(GEntityList->Player(), E->Range()) && myHero->IsValidTarget(target, 2000))
						{
							if (EShield04->HealthPercent() < ShieldEally->GetInteger())
							{
								E->CastOnUnit(EShield04);
							}
						}
						if (EShield05->IsValidTarget(GEntityList->Player(), E->Range()) && myHero->IsValidTarget(target, 2000))
						{
							if (EShield05->HealthPercent() < ShieldEally->GetInteger())
							{
								E->CastOnUnit(EShield05);
							}
						}

					}
				}
			}
			else if (EShield3->Enabled() && !EShield03->IsValidTarget(GEntityList->Player(), E->Range()) && myHero->IsValidTarget(target, 2000))
			{
				if (ShieldE->Enabled() && E->IsReady() && Ally->IsValidTarget(GEntityList->Player(), E->Range()) && myHero->IsValidTarget(target, 2000))
				{
					if (Ally->HealthPercent() < ShieldEally->GetInteger())
					{
						if (Ally != nullptr)
						{
							E->CastOnUnit(Ally);
						}
					}
				}
			}
			if (EShield4 != nullptr && EShield4->Enabled() && EShield04->ChampionName() == Ally->ChampionName())
			{
				if (ShieldE->Enabled() && E->IsReady() && EShield04->IsValidTarget(GEntityList->Player(), E->Range()) && myHero->IsValidTarget(target, 2000))
				{
					if (EShield04->HealthPercent() < ShieldEally->GetInteger())
					{
						E->CastOnUnit(EShield04);
					}
					else if (EShield04->HealthPercent() > ShieldEally->GetInteger())
					{
						if (EShield01->IsValidTarget(GEntityList->Player(), E->Range()) && myHero->IsValidTarget(target, 2000))
						{
							if (EShield01->HealthPercent() < ShieldEally->GetInteger())
							{
								E->CastOnUnit(EShield01);
							}
						}
						if (EShield02->IsValidTarget(GEntityList->Player(), E->Range()) && myHero->IsValidTarget(target, 2000))
						{
							if (EShield02->HealthPercent() < ShieldEally->GetInteger())
							{
								E->CastOnUnit(EShield02);
							}
						}
						if (EShield03->IsValidTarget(GEntityList->Player(), E->Range()) && myHero->IsValidTarget(target, 2000))
						{
							if (EShield03->HealthPercent() < ShieldEally->GetInteger())
							{
								E->CastOnUnit(EShield03);
							}
						}
						if (EShield05->IsValidTarget(GEntityList->Player(), E->Range()) && myHero->IsValidTarget(target, 2000))
						{
							if (EShield05->HealthPercent() < ShieldEally->GetInteger())
							{
								E->CastOnUnit(EShield05);
							}
						}

					}
				}
			}

			else if (EShield4->Enabled() && !EShield04->IsValidTarget(GEntityList->Player(), E->Range()) && myHero->IsValidTarget(target, 2000))
			{
				if (ShieldE->Enabled() && E->IsReady() && Ally->IsValidTarget(GEntityList->Player(), E->Range()) && myHero->IsValidTarget(target, 2000))
				{
					if (Ally->HealthPercent() < ShieldEally->GetInteger())
					{
						if (Ally != nullptr)
						{
							E->CastOnUnit(Ally);
						}
					}
				}
			}
			if (EShield5 != nullptr && EShield5->Enabled() && EShield05->ChampionName() == Ally->ChampionName())
			{
				if (ShieldE->Enabled() && E->IsReady() && EShield05->IsValidTarget(GEntityList->Player(), E->Range()) && myHero->IsValidTarget(target, 2000))
				{
					if (EShield05->HealthPercent() < ShieldEally->GetInteger())
					{
						E->CastOnUnit(EShield05);
					}
					else if (EShield05->HealthPercent() > ShieldEally->GetInteger())
					{
						if (EShield02->IsValidTarget(GEntityList->Player(), E->Range()) && myHero->IsValidTarget(target, 2000))
						{
							if (EShield02->HealthPercent() < ShieldEally->GetInteger())
							{
								E->CastOnUnit(EShield02);
							}
						}
						if (EShield03->IsValidTarget(GEntityList->Player(), E->Range()) && myHero->IsValidTarget(target, 2000))
						{
							if (EShield03->HealthPercent() < ShieldEally->GetInteger())
							{
								E->CastOnUnit(EShield03);
							}
						}
						if (EShield04->IsValidTarget(GEntityList->Player(), E->Range()) && myHero->IsValidTarget(target, 2000))
						{
							if (EShield04->HealthPercent() < ShieldEally->GetInteger())
							{
								E->CastOnUnit(EShield04);
							}
						}
						if (EShield01->IsValidTarget(GEntityList->Player(), E->Range()) && myHero->IsValidTarget(target, 2000))
						{
							if (EShield01->HealthPercent() < ShieldEally->GetInteger())
							{
								E->CastOnUnit(EShield01);
							}
						}

					}
				}
			}

			else if (EShield5->Enabled() && !EShield05->IsValidTarget(GEntityList->Player(), E->Range()) && myHero->IsValidTarget(target, 2000))
			{
				if (ShieldE->Enabled() && E->IsReady() && Ally->IsValidTarget(GEntityList->Player(), E->Range()) && myHero->IsValidTarget(target, 2000))
				{
					if (Ally->HealthPercent() < ShieldEally->GetInteger())
					{
						if (Ally != nullptr)
						{
							E->CastOnUnit(Ally);
						}
					}
				}
			}
			if (!EShield1->Enabled() && !EShield2->Enabled() && !EShield3->Enabled() && !EShield4->Enabled() && !EShield5->Enabled())
			{
				if (ShieldE->Enabled() && E->IsReady() && Ally->IsValidTarget(GEntityList->Player(), E->Range()) && myHero->IsValidTarget(target, 2000))
				{
					if (Ally->HealthPercent() < ShieldEally->GetInteger())
					{
						if (Ally != nullptr)
						{
							E->CastOnUnit(Ally);
						}
					}
				}
			}
		}
		if (Ally != nullptr && Ally->IsValidTarget())
		{

			if (ShieldE->Enabled() && ShieldEmy->GetInteger() > GEntityList->Player()->HealthPercent() && E->IsReady() && !GEntityList->Player()->IsRecalling() && !GUtility->IsPositionInFountain(GEntityList->Player()->ServerPosition()) && myHero->IsValidTarget(target, 2000))
			{
				E->CastOnPlayer();
			}
		}
	}
}

void CastQ(IUnit* target)
{
	AdvPredictionOutput prediction_output;
	Q->RunPrediction(target, true, kCollidesWithYasuoWall | kCollidesWithMinions, &prediction_output);
	if (prediction_output.HitChance >= kHitChanceHigh)
	{
		Q->CastOnTarget(target, kHitChanceCollision);
		lastq = GGame->CurrentTick();
	}
}

void Combo()
{
	if (useQcombo->Enabled() && Q->IsReady() && !myHero->HasBuff("ivernqallyjump") && GGame->CurrentTick() - lastq > 250)
	{
		auto target = GTargetSelector->FindTarget(QuickestKill, PhysicalDamage, Q->Range());
		if (myHero->IsValidTarget(target, Q->Range()) && !IsImmune(target))
		{
			CastQ(target);
		}
	}

	if (useQ2combo->Enabled())
	{
		auto target = GTargetSelector->FindTarget(QuickestKill, PhysicalDamage, Q->Range());
		if (myHero->IsValidTarget(target, 1000) && target->HasBuff("IvernQ") && CountEnemiesInRange(1500) <= CountallyInRange(1500) && GGame->CurrentTick() - lastq > 100)
		{
			Q->CastOnPlayer();
		}
	}

	if (useWcombo->Enabled() && W->IsReady() && !myHero->HasBuff("ivernwpassive") && GGame->CurrentTick() - lastw > 250)
	{	
		auto target = GTargetSelector->FindTarget(QuickestKill, PhysicalDamage, 450);
		if (myHero->IsValidTarget(target, 450))
		{
			W->CastOnPlayer();
			lastw = GGame->CurrentTick();
		}
	}
}

void Harass()
{

}


void AutoImmobile()
{
	if (GGame->IsChatOpen() || myHero->ManaPercent() < 30) return;
	if (Q->IsReady())
	{
		auto target = GTargetSelector->FindTarget(QuickestKill, PhysicalDamage, Q->Range());
		if (target != nullptr && myHero->IsValidTarget(target, Q->Range()) && !target->IsInvulnerable())
		{
			//if (autoQ->Enabled() && HaveQ1())

				{
					Q->CastOnTarget(target, kHitChanceImmobile);
				}

				{
					Q->CastOnTarget(target, kHitChanceDashing);
				}

		}
	}
}

static bool HaveCC(IUnit* Ally)
{
	return Ally->HasBuffOfType(BUFF_Charm) || Ally->HasBuffOfType(BUFF_CombatDehancer) ||
		Ally->HasBuffOfType(BUFF_Fear) || Ally->HasBuffOfType(BUFF_Knockback) ||
		Ally->HasBuffOfType(BUFF_Knockup) || Ally->HasBuffOfType(BUFF_Polymorph) ||
		Ally->HasBuffOfType(BUFF_Snare) || Ally->HasBuffOfType(BUFF_Stun) ||
		Ally->HasBuffOfType(BUFF_Suppression) || Ally->HasBuffOfType(BUFF_Taunt);
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
	if (DrawReady->Enabled())
	{
		if (Q->IsReady() && DrawQ->Enabled()) { GRender->DrawOutlinedCircle(GEntityList->Player()->GetPosition(), Vec4(255, 255, 0, 255), Q->Range()); }

		if (E->IsReady() && DrawE->Enabled()) { GRender->DrawOutlinedCircle(GEntityList->Player()->GetPosition(), Vec4(255, 255, 0, 255), E->Range()); }
	}
	else
	{
		if (DrawQ->Enabled()) { GRender->DrawOutlinedCircle(GEntityList->Player()->GetPosition(), Vec4(255, 255, 0, 255), Q->Range()); }

		if (DrawE->Enabled()) { GRender->DrawOutlinedCircle(GEntityList->Player()->GetPosition(), Vec4(255, 255, 0, 255), E->Range()); }
	}
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
	Shielding();
}

PLUGIN_API void OnLoad(IPluginSDK* PluginSDK)
{
	PluginSDKSetup(PluginSDK);
	Menu();
	LoadSpells();
	ShieldEa();
	myHero = GEntityList->Player();

	GEventManager->AddEventHandler(kEventOnGameUpdate, OnGameUpdate);
	GEventManager->AddEventHandler(kEventOnRender, OnRender);
	GEventManager->AddEventHandler(kEventOrbwalkAfterAttack, OnAfterAttack);
	GEventManager->AddEventHandler(kEventOrbwalkBeforeAttack, OnBeforeAttack);

	if (strcmp(GEntityList->Player()->ChampionName(), "Ivern") == 0)
	{
		GGame->PrintChat("nerfIvern: Loaded");
	}
	else
	{
		GGame->PrintChat("You are not playing Ivern...");
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