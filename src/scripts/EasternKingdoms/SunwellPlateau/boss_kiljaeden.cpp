/*
 * This file is part of the OregonCore Project. See AUTHORS file for Copyright information
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */

/* ScriptData
SDName: Boss_Kiljaeden
SD%Complete: 80
SDComment: Sinister Reflection Model, Armageddon Visual, SAY_KJ_SHADOWSPIKE3, Emote, End Sequence
SDCategory: Sunwell_Plateau
EndScriptData */

//TODO rewrite Armageddon

#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "sunwell_plateau.h"
#include <math.h>

/*** Speech and sounds***/
enum Yells
{
    // These are used throughout Sunwell and Magisters(?). Players can hear this while running through the instances.
    SAY_KJ_OFFCOMBAT1                           = -1580066,
    SAY_KJ_OFFCOMBAT2                           = -1580067,
    SAY_KJ_OFFCOMBAT3                           = -1580068,
    SAY_KJ_OFFCOMBAT4                           = -1580069,
    SAY_KJ_OFFCOMBAT5                           = -1580070,

    // Encounter speech and sounds
    SAY_KJ_EMERGE                               = -1580071,
    SAY_KJ_SLAY1                                = -1580072,
    SAY_KJ_SLAY2                                = -1580073,
    SAY_KJ_REFLECTION1                          = -1580074,
    SAY_KJ_REFLECTION2                          = -1580075,
    SAY_KJ_DARKNESS1                            = -1580076,
    SAY_KJ_DARKNESS2                            = -1580077,
    SAY_KJ_DARKNESS3                            = -1580078,
    SAY_KJ_PHASE3                               = -1580079,
    SAY_KJ_PHASE4                               = -1580080,
    SAY_KJ_PHASE5                               = -1580081,
    SAY_KJ_DEATH                                = -1580093,
    EMOTE_KJ_DARKNESS                           = -1580094,

    /*** Kalecgos - Anveena speech at the beginning of Phase 5; Anveena's sacrifice ***/
    SAY_KALECGOS_AWAKEN                         = -1580082,
    SAY_ANVEENA_IMPRISONED                      = -1580083,
    SAY_KALECGOS_LETGO                          = -1580084,
    SAY_ANVEENA_LOST                            = -1580085,
    SAY_KALECGOS_FOCUS                          = -1580086,
    SAY_ANVEENA_KALEC                           = -1580087,
    SAY_KALECGOS_FATE                           = -1580088,
    SAY_ANVEENA_GOODBYE                         = -1580089,
    SAY_KALECGOS_ENCOURAGE                      = -1580091,

    /*** Kalecgos says throughout the fight ***/
    SAY_KALECGOS_JOIN                           = -1580092,
    SAY_KALEC_ORB_READY1                        = -1580095,
    SAY_KALEC_ORB_READY2                        = -1580096,
    SAY_KALEC_ORB_READY3                        = -1580097,
    SAY_KALEC_ORB_READY4                        = -1580098
};

/*** Spells used during the encounter ***/
enum Spells
{
    /* Hand of the Deceiver's spells and cosmetics */
    SPELL_SHADOW_BOLT_VOLLEY                    = 45770, // ~30 yard range Shadow Bolt Volley for ~2k(?) damage
    SPELL_SHADOW_INFUSION                       = 45772, // They gain this at 20% - Immunity to Stun/Silence and makes them look angry!
    SPELL_FELFIRE_PORTAL                        = 46875, // Creates a portal that spawns Felfire Fiends (LIVE FOR THE SWARM!1 FOR THE OVERMIND!)
    SPELL_SUMMON_FELFIRE_FIEND                  = 46464,
    SPELL_SHADOW_CHANNELING                     = 46757, // Channeling animation out of combat

    /* Volatile Felfire Fiend's spells */
    SPELL_FELFIRE_FISSION                       = 45779, // Felfire Fiends explode when they die or get close to target.

    /* Kil'Jaeden's spells and cosmetics */
    SPELL_TRANS                                 = 23188, // Surprisingly, this seems to be the right spell.. (Where is it used?)
    SPELL_REBIRTH                               = 44200, // Emerge from the Sunwell
    SPELL_SOUL_FLAY                             = 45442, // 9k Shadow damage over 3 seconds. Spammed throughout all the fight.
    SPELL_SOUL_FLAY_SLOW                        = 47106,
    SPELL_LEGION_LIGHTNING                      = 45664, // Chain Lightning, 4 targets, ~3k Shadow damage, 1.5k mana burn
    SPELL_FIRE_BLOOM                            = 45641, // Places a debuff on 5 raid members, which causes them to deal 2k Fire damage to nearby allies and selves. MIGHT NOT WORK
    SPELL_DESTROY_ALL_DRAKES                    = 46707, // when he use it?

    SPELL_SINISTER_REFLECTION                   = 45892, // Summon shadow copies of 5 raid members that fight against KJ's enemies
    SPELL_COPY_WEAPON                           = 41055, // }
    SPELL_COPY_WEAPON2                          = 41054, // }
    SPELL_COPY_OFFHAND                          = 45206, // }- Spells used in Sinister Reflection creation
    SPELL_COPY_OFFHAND_WEAPON                   = 45205, // }

    SPELL_SHADOW_SPIKE                          = 46680, // Bombard random raid members with Shadow Spikes (Very similar to Void Reaver orbs)
    SPELL_FLAME_DART                            = 45737, // Bombards the raid with flames every 3(?) seconds
    SPELL_DARKNESS_OF_A_THOUSAND_SOULS          = 46605, // Begins a 8-second channeling, after which he will deal 50'000 damage to the raid
    SPELL_DARKNESS_OF_A_THOUSAND_SOULS_DAMAGE   = 45657,

    /* Armageddon spells wrong visual */
    SPELL_ARMAGEDDON_TRIGGER                    = 45909, // Meteor spell trigger missile should cast Creature on himself
    SPELL_ARMAGEDDON_VISUAL                     = 45911, // Does the hellfire visual to indicate where the meteor missle lands
    SPELL_ARMAGEDDON_VISUAL2                    = 45914, // Does the light visual to indicate where the meteor missle lands
    SPELL_ARMAGEDDON_VISUAL3                    = 24207, // This shouldn't correct but same as seen on the movie
    SPELL_ARMAGEDDON_SUMMON_TRIGGER             = 45921, // Summons the triggers that cast the spells on himself need random target select
    SPELL_ARMAGEDDON_DAMAGE                     = 45915, // This does the area damage

    /* Shield Orb Spells*/
    SPELL_SHADOW_BOLT                           = 45680, //45679 would be correct but triggers to often //TODO fix console error

    /* Anveena's spells and cosmetics (Or, generally, everything that has "Anveena" in name) */
    SPELL_ANVEENA_PRISON                        = 46367, // She hovers locked within a bubble
    SPELL_ANVEENA_ENERGY_DRAIN                  = 46410, // Sunwell energy glow animation (Control mob uses this)
    SPELL_SACRIFICE_OF_ANVEENA                  = 46474, // This is cast on Kil'Jaeden when Anveena sacrifices herself into the Sunwell

    /* Sinister Reflection Spells */
    SPELL_SR_CURSE_OF_AGONY                     = 46190,
    SPELL_SR_SHADOW_BOLT                        = 47076,

    SPELL_SR_EARTH_SHOCK                        = 47071,

    SPELL_SR_FIREBALL                           = 47074,

    SPELL_SR_HEMORRHAGE                         = 45897,

    SPELL_SR_HOLY_SHOCK                         = 38921,
    SPELL_SR_HAMMER_OF_JUSTICE                  = 37369,

    SPELL_SR_HOLY_SMITE                         = 47077,
    SPELL_SR_RENEW                              = 47079,

    SPELL_SR_SHOOT                              = 16496,
    SPELL_SR_MULTI_SHOT                         = 48098,
    SPELL_SR_WING_CLIP                          = 40652,

    SPELL_SR_WHIRLWIND                          = 17207,

    SPELL_SR_MOONFIRE                           = 47072,
    //SPELL_SR_PLAGU STRIKE                       = 58843, Dk Spell!

    /*** Other Spells (used by players, etc) ***/
    SPELL_VENGEANCE_OF_THE_BLUE_FLIGHT          = 45839, // Possess the blue dragon from the orb to help the raid.
    SPELL_RING_OF_BLUE_FLAMES                   = 45825,  //Cast this spell when the go is activated
};

/*** Error messages ***/
#define ERROR_KJ_NOT_SUMMONED "OSCR ERROR: Unable to summon Kil'Jaeden for some reason"

/*** Others ***/
#define FLOOR_Z         28.050388f
#define SHIELD_ORB_Z    45.000f

enum Phase
{
    PHASE_DECEIVERS     = 1, // Fight 3 adds
    PHASE_NORMAL        = 2, // Kil'Jaeden emerges from the sunwell
    PHASE_DARKNESS      = 3, // At 85%, he gains few abilities; Kalecgos joins the fight
    PHASE_ARMAGEDDON    = 4, // At 55%, he gains even more abilities
    PHASE_SACRIFICE     = 5, // At 25%, Anveena sacrifices herself into the Sunwell; at this point he becomes enraged and has *significally* shorter cooldowns.
};

//Timers
enum KilJaedenTimers
{
    TIMER_SPEECH           = 0,

    //Phase 2 Timer
    TIMER_SOUL_FLAY        = 1,
    TIMER_LEGION_LIGHTNING = 2,
    TIMER_FIRE_BLOOM       = 3,
    TIMER_SUMMON_SHILEDORB = 4,

    //Phase 3 Timer
    TIMER_SHADOW_SPIKE     = 5,
    TIMER_FLAME_DART       = 6,
    TIMER_DARKNESS         = 7,
    TIMER_ORBS_EMPOWER     = 8,

    //Phase 4 Timer
    TIMER_ARMAGEDDON       = 9
};

// Locations of the Hand of Deceiver adds
float DeceiverLocations[3][3] =
{
    {1682.045f, 631.299f, 5.936f},
    {1684.099f, 618.848f, 0.589f},
    {1694.170f, 612.272f, 1.416f},
};

// Locations, where Shield Orbs will spawn
float ShieldOrbLocations[4][2] =
{
    {1698.900f, 627.870f},    // middle pont of Sunwell
    {12, 3.14f},             // First one spawns northeast of KJ
    {12, 3.14f / 0.7f},       // Second one spawns southeast
    {12, 3.14f * 3.8f}        // Third one spawns (?)
};

struct Speech
{
    int32 textid;
    uint32 pCreature, timer;
};

// Timers
static Speech Speeches[] =
{
    //Kil Phase 1 -> Phase 2
    {SAY_KJ_EMERGE,             DATA_KILJAEDEN,     0},
    {SAY_KALECGOS_JOIN,         DATA_KALECGOS_KJ,   26000},
    //Kil Phase 2 -> Phase 3
    {SAY_KALECGOS_AWAKEN,       DATA_KALECGOS_KJ,   10000},
    {SAY_ANVEENA_IMPRISONED,    DATA_ANVEENA,       5000},
    {SAY_KJ_PHASE3,             DATA_KILJAEDEN,     5000},
    //Kil Phase 3 -> Phase 4
    {SAY_KALECGOS_LETGO,        DATA_KALECGOS_KJ,   10000},
    {SAY_ANVEENA_LOST,          DATA_ANVEENA,       8000},
    {SAY_KJ_PHASE4,             DATA_KILJAEDEN,     7000},
    //Kil Phase 4 -> Phase 5
    {SAY_KALECGOS_FOCUS,        DATA_KALECGOS_KJ,   4000},
    {SAY_ANVEENA_KALEC,         DATA_ANVEENA,       11000},
    {SAY_KALECGOS_FATE,         DATA_KALECGOS_KJ,   2000},
    {SAY_ANVEENA_GOODBYE,       DATA_ANVEENA,       6000},
    {SAY_KJ_PHASE5,             DATA_KILJAEDEN,     5500},
};

//AI for Kalecgos
struct boss_kalecgos_kjAI : public ScriptedAI
{
    boss_kalecgos_kjAI(Creature* c) : ScriptedAI(c)
    {
        pInstance = (ScriptedInstance*)c->GetInstanceData();
    }

    ScriptedInstance* pInstance;
    uint8 OrbsEmpowered;
    uint8 EmpowerCount;

    void Reset()
    {
        OrbsEmpowered = 0;
        EmpowerCount = 0;
        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);

        for (uint8 i = 0; i < 4; ++i)
            if (GameObject* pOrb = GetOrb(i))
                pOrb->SetGoType(GAMEOBJECT_TYPE_BUTTON);
    }

    GameObject* GetOrb(int32 index)
    {
        if (!pInstance)
            return NULL;

        switch (index)
        {
        case 0:
            return pInstance->instance->GetGameObject(pInstance->GetData64(DATA_ORB_OF_THE_BLUE_DRAGONFLIGHT_1));
        case 1:
            return pInstance->instance->GetGameObject(pInstance->GetData64(DATA_ORB_OF_THE_BLUE_DRAGONFLIGHT_2));
        case 2:
            return pInstance->instance->GetGameObject(pInstance->GetData64(DATA_ORB_OF_THE_BLUE_DRAGONFLIGHT_3));
        case 3:
            return pInstance->instance->GetGameObject(pInstance->GetData64(DATA_ORB_OF_THE_BLUE_DRAGONFLIGHT_4));
        }

        return NULL;
    }

    void ResetOrbs()
    {
        me->RemoveDynObject(SPELL_RING_OF_BLUE_FLAMES);
        for (uint8 i = 0; i < 4; ++i)
            if (GameObject* pOrb = GetOrb(i))
                pOrb->SetUInt32Value(GAMEOBJECT_FACTION, 0);
    }

    void EmpowerOrb(bool all)
    {
        GameObject* pOrbEmpowered = GetOrb(OrbsEmpowered);
        if (!pOrbEmpowered)
            return;

        if (all)
        {
            me->RemoveDynObject(SPELL_RING_OF_BLUE_FLAMES);
            for (uint8 i = 0; i < 4; ++i)
            {
                if (GameObject* pOrb = GetOrb(i))
                {
                    pOrb->CastSpell(me, SPELL_RING_OF_BLUE_FLAMES);
                    pOrb->SetUInt32Value(GAMEOBJECT_FACTION, 35);
                    pOrb->setActive(true);
                    pOrb->Refresh();
                }
            }
            DoScriptText(SAY_KALECGOS_ENCOURAGE, me);
        }
        else
        {
            if (GameObject* pOrb = GetOrb(urand(0, 3)))
            {
                pOrb->CastSpell(me, SPELL_RING_OF_BLUE_FLAMES);
                pOrb->SetUInt32Value(GAMEOBJECT_FACTION, 35);
                pOrb->setActive(true);
                pOrb->Refresh();

                OrbsEmpowered = (OrbsEmpowered + 1) % 4;

                ++EmpowerCount;
                switch (EmpowerCount)
                {
                case 1:
                    DoScriptText(SAY_KALEC_ORB_READY1, me);
                    break;
                case 2:
                    DoScriptText(SAY_KALEC_ORB_READY2, me);
                    break;
                case 3:
                    DoScriptText(SAY_KALEC_ORB_READY3, me);
                    break;
                case 4:
                    DoScriptText(SAY_KALEC_ORB_READY4, me);
                    break;
                }
            }
        }
    }

    void UpdateAI(const uint32 /*diff*/)
    {
    }

    void SetRingOfBlueFlames()
    {
        me->RemoveDynObject(SPELL_RING_OF_BLUE_FLAMES);
        for (uint8 i = 0; i < 4; ++i)
        {
            if (GameObject* pOrb = GetOrb(i))
            {
                if (pOrb->GetUInt32Value(GAMEOBJECT_FACTION) == 35)
                {
                    pOrb->CastSpell(me, SPELL_RING_OF_BLUE_FLAMES);
                    pOrb->setActive(true);
                    pOrb->Refresh();
                }
            }
        }
    }
};

CreatureAI* GetAI_boss_kalecgos_kj(Creature* pCreature)
{
    return new boss_kalecgos_kjAI (pCreature);
}

bool GOHello_go_orb_of_the_blue_flight(Player* pPlayer, GameObject* pGo)
{
    if (pGo->GetUInt32Value(GAMEOBJECT_FACTION) == 35)
    {
        ScriptedInstance* pInstance = (ScriptedInstance*)pGo->GetInstanceData();
        pPlayer->SummonCreature(CREATURE_POWER_OF_THE_BLUE_DRAGONFLIGHT, pPlayer->GetPositionX(), pPlayer->GetPositionY(), pPlayer->GetPositionZ(), 0.0f, TEMPSUMMON_TIMED_DESPAWN, 121000);
        pPlayer->CastSpell(pPlayer, SPELL_VENGEANCE_OF_THE_BLUE_FLIGHT, false);
        pGo->SetUInt32Value(GAMEOBJECT_FACTION, 0);

        if (Creature* pKalec = Unit::GetCreature(*pPlayer, pInstance->GetData64(DATA_KALECGOS_KJ)))
            CAST_AI(boss_kalecgos_kjAI, pKalec->AI())->SetRingOfBlueFlames();

        pGo->Refresh();
    }
    return true;
}

//AI for Kil'jaeden Event Controller
struct mob_kiljaeden_controllerAI : public Scripted_NoMovementAI
{
    mob_kiljaeden_controllerAI(Creature* c) : Scripted_NoMovementAI(c), summons(me)
    {
        pInstance = (ScriptedInstance*)c->GetInstanceData();
    }

    ScriptedInstance* pInstance;
    SummonList summons;

    bool bSummonedDeceivers;
    bool bKiljaedenDeath;

    uint32 uiRandomSayTimer;
    uint32 phase;
    uint8 deceiverDeathCount;

    uint64 handDeceiver[3];

    void InitializeAI()
    {
        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        me->AddUnitState(UNIT_STATE_STUNNED);

        ScriptedAI::InitializeAI();
    }

    void Reset()
    {
        for (uint8 i = 0; i < 3; ++i)
            handDeceiver[i] = 0;

        if (Creature* pKalecKJ = Unit::GetCreature((*me), pInstance->GetData64(DATA_KALECGOS_KJ)))
            CAST_AI(boss_kalecgos_kjAI, pKalecKJ->AI())->ResetOrbs();

        if (pInstance)
        {
            if (pInstance->GetData(DATA_KILJAEDEN_EVENT) == DONE)
            {
                DoCast(me, SPELL_SUNWELL_IGNITION, true);
                return;
            }
            else
            {
                DoCast(me, SPELL_ANVEENA_ENERGY_DRAIN, true);
                pInstance->SetData(DATA_KILJAEDEN_EVENT, NOT_STARTED);
            }
        }

        phase = PHASE_DECEIVERS;

        deceiverDeathCount = 0;
        bKiljaedenDeath = false;
        uiRandomSayTimer = 30000;
        summons.DespawnAll();
        bSummonedDeceivers = false;
    }

    void MoveInLineOfSight(Unit* who)
    {
        if (pInstance && pInstance->GetData(DATA_KILJAEDEN_EVENT) == DONE)
            return;

        ScriptedAI::MoveInLineOfSight(who);

        DoZoneInCombatWithPlayers(true);

        if (bSummonedDeceivers)
        {
            for (uint8 i = 0; i < 3; ++i)
            {
                if (Creature *hand = pInstance->GetCreature(handDeceiver[i]))
                {
                    if (!hand->IsInCombat())
                        hand->AI()->AttackStart(who);

                    hand->AI()->DoZoneInCombatWithPlayers(true);
                }
            }
        }
    }

    void JustSummoned(Creature* summoned)
    {
        switch (summoned->GetEntry())
        {
        case CREATURE_HAND_OF_THE_DECEIVER:
            summoned->CastSpell(summoned, SPELL_SHADOW_CHANNELING, false);
            break;
        case CREATURE_ANVEENA:
            summoned->CastSpell(summoned, SPELL_ANVEENA_PRISON, true);
            me->CastSpell(summoned, SPELL_ANVEENA_ENERGY_DRAIN, true);
            summoned->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
            break;
        case CREATURE_KILJAEDEN:
            summoned->CastSpell(summoned, SPELL_REBIRTH, false);
            summoned->AddThreat(me->GetVictim(), 1.0f);
            break;
        case NPC_RIFTWALKER:
            summoned->CastSpell(summoned, SPELL_TELEPORT_VISUAL, true);
            break;
        case NPC_SOLDIER:
            summoned->CastSpell(summoned, SPELL_TELEPORT_VISUAL, true);
            summoned->SetWalk(false);
            summoned->SetSpeed(MOVE_RUN, 1.0f);
            break;
        case CREATURE_PROPHET:
            summoned->CastSpell(summoned, SPELL_TELEPORT_VISUAL, true);
            summoned->GetMotionMaster()->MovePoint(0, aOutroLocations[5].m_fX, aOutroLocations[5].m_fY, aOutroLocations[5].m_fZ);
            break;
        case CREATURE_LIADRIN:
            summoned->CastSpell(summoned, SPELL_TELEPORT_VISUAL, true);
            break;
        case NPC_CORE_ENTROPIUS:
            summoned->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
            summoned->CastSpell(summoned, SPELL_ENTROPIUS_BODY, true);
            break;
        }
        summons.Summon(summoned);
    }

    void UpdateAI(const uint32 diff)
    {
        if (pInstance && pInstance->GetData(DATA_KILJAEDEN_EVENT) == DONE)
            return;

        if (uiRandomSayTimer < diff)
        {
            if (pInstance && pInstance->GetData(DATA_MURU_EVENT) != DONE && pInstance->GetData(DATA_KILJAEDEN_EVENT) == NOT_STARTED)
                DoScriptText(RAND(SAY_KJ_OFFCOMBAT1, SAY_KJ_OFFCOMBAT2, SAY_KJ_OFFCOMBAT3, SAY_KJ_OFFCOMBAT4, SAY_KJ_OFFCOMBAT5), me);
            uiRandomSayTimer = 30000;
        }
        else uiRandomSayTimer -= diff;

        if (!bSummonedDeceivers)
        {
            for (uint8 i = 0; i < 3; ++i)
            {
                if (Creature* hand = me->SummonCreature(CREATURE_HAND_OF_THE_DECEIVER, DeceiverLocations[i][0], DeceiverLocations[i][1], FLOOR_Z, DeceiverLocations[i][2], TEMPSUMMON_DEAD_DESPAWN, 0))
                    handDeceiver[i] = hand->GetGUID();
            }

            DoSpawnCreature(CREATURE_ANVEENA,  0, 0, 40, 0, TEMPSUMMON_DEAD_DESPAWN, 0);
            bSummonedDeceivers = true;
        }

        if (deceiverDeathCount > 2 && phase == PHASE_DECEIVERS)
        {
            me->RemoveAurasDueToSpell(SPELL_ANVEENA_ENERGY_DRAIN);
            phase = PHASE_NORMAL;
            DoSpawnCreature(CREATURE_KILJAEDEN, 0, 0, 0, 0, TEMPSUMMON_MANUAL_DESPAWN, 0);
        }
    }
};

CreatureAI* GetAI_mob_kiljaeden_controller(Creature* pCreature)
{
    return new mob_kiljaeden_controllerAI (pCreature);
}

//AI for Kil'jaeden
struct boss_kiljaedenAI : public Scripted_NoMovementAI
{
    boss_kiljaedenAI(Creature* c) : Scripted_NoMovementAI(c), summons(me)
    {
        pInstance = (ScriptedInstance*)c->GetInstanceData();
    }

    ScriptedInstance* pInstance;
    SummonList summons;

    uint8 Phase;
    uint8 ActiveTimers;
    uint32 SpeechTimer;

    uint32 Timer[10];
    uint32 WaitTimer;
    uint8 speechCount;
    uint8 speechPhaseEnd;

    /* Boolean */
    bool IsInDarkness;
    bool TimerIsDeactivated[10];
    bool IsWaiting;
    bool OrbActivated;
    bool SpeechBegins;

    void InitializeAI()
    {
        Scripted_NoMovementAI::InitializeAI();
    }

    void Reset()
    {
        TimerIsDeactivated[TIMER_SPEECH] = false;
        Timer[TIMER_SPEECH]           = 0;

        //Phase 2 Timer
        Timer[TIMER_SOUL_FLAY]        = 11000;
        Timer[TIMER_LEGION_LIGHTNING] = 30000;
        Timer[TIMER_FIRE_BLOOM]       = 20000;
        Timer[TIMER_SUMMON_SHILEDORB] = 35000;

        //Phase 3 Timer
        Timer[TIMER_SHADOW_SPIKE]     = 4000;
        Timer[TIMER_FLAME_DART]       = 3000;
        Timer[TIMER_DARKNESS]         = 45000;
        Timer[TIMER_ORBS_EMPOWER]     = 35000;

        //Phase 4 Timer
        Timer[TIMER_ARMAGEDDON]       = 2000;

        ActiveTimers = 5;
        WaitTimer    = 0;
        speechCount = 0;
        SpeechTimer = 0;

        Phase = PHASE_NORMAL;

        IsInDarkness  = false;
        IsWaiting     = false;
        OrbActivated  = false;
        SpeechBegins  = true;

        if (pInstance)
        {
            if (Creature* pKalec = Unit::GetCreature(*me, pInstance->GetData64(DATA_KALECGOS_KJ)))
                pKalec->RemoveDynObject(SPELL_RING_OF_BLUE_FLAMES);
        }
        me->SetFloatValue(UNIT_FIELD_COMBATREACH, 12);
        ChangeTimers(false, 0);
        summons.DespawnAll();
    }

    void ChangeTimers(bool status, uint32 WTimer)
    {
        for (uint8 i = 1; i < ActiveTimers; ++i)
            TimerIsDeactivated[i] = status;

        if (WTimer > 0)
        {
            IsWaiting = true;
            WaitTimer = WTimer;
        }

        if (OrbActivated)
            TimerIsDeactivated[TIMER_ORBS_EMPOWER] = true;
        if (Timer[TIMER_SHADOW_SPIKE] == 0)
            TimerIsDeactivated[TIMER_SHADOW_SPIKE] = true;
        if (Phase == PHASE_SACRIFICE)
            TimerIsDeactivated[TIMER_SUMMON_SHILEDORB] = true;
    }

    void JustSummoned(Creature* summoned)
    {
        switch (summoned->GetEntry())
        {
            case CREATURE_ARMAGEDDON_TARGET:
                summoned->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                summoned->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                break;
            case CREATURE_SINISTER_REFLECTION:
                break;
            default:
                summoned->SetLevel(me->getLevel());
                break;
        }

        summoned->SetFaction(me->GetFaction());
        summons.Summon(summoned);
    }

    void JustDied(Unit* /*killer*/)
    {
        DoScriptText(SAY_KJ_DEATH, me);
        summons.DespawnAll();

        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);

        if (pInstance)
            pInstance->SetData(DATA_KILJAEDEN_EVENT, DONE);

        if (Creature *controller = pInstance->instance->GetCreature(pInstance->GetData64(DATA_KILJAEDEN_CONTROLLER)))
        {
            controller->SetFaction(35);
            controller->RemoveAllAuras();
            controller->DeleteThreatList();
            controller->CombatStop();
        }
    }

    void KilledUnit(Unit* /*victim*/)
    {
        DoScriptText(RAND(SAY_KJ_SLAY1, SAY_KJ_SLAY2), me);
    }

    void EnterEvadeMode()
    {
        Scripted_NoMovementAI::EnterEvadeMode();
        summons.DespawnAll();

        // Reset the controller
        if (pInstance)
            if (Creature* pControl = Unit::GetCreature(*me, pInstance->GetData64(DATA_KILJAEDEN_CONTROLLER)))
                CAST_AI(mob_kiljaeden_controllerAI, pControl->AI())->Reset();
    }

    void EnterCombat(Unit* /*who*/)
    {
        DoZoneInCombat();
    }

    void EnterNextPhase()
    {
        SpeechBegins  = true;
        OrbActivated  = false;
        ChangeTimers(true, 0);//stop every cast Shadow spike will reactivate em all
        TimerIsDeactivated[TIMER_SHADOW_SPIKE] = false;
        Timer[TIMER_SHADOW_SPIKE]     = 100;
        // empowered orbs before darkness
        Timer[TIMER_DARKNESS] = (Phase == PHASE_SACRIFICE) ? 15000 : urand(10000, 40000);
        Timer[TIMER_ORBS_EMPOWER] = (Phase == PHASE_SACRIFICE) ? 10000 : 5000;
    }

    void UpdateAI(const uint32 diff)
    {
        if (!UpdateVictim() || Phase < PHASE_NORMAL)
            return;

        if (IsWaiting)
        {
            if (WaitTimer <= diff)
            {
                IsWaiting = false;
                ChangeTimers(false, 0);
            }
            else WaitTimer -= diff;
        }

        for (uint8 t = 0; t < ActiveTimers; ++t)
        {
            if (Timer[t] <= diff && !TimerIsDeactivated[t])
            {
                switch (t)
                {
                case TIMER_SPEECH:
                    if (SpeechBegins)
                    {
                        SpeechBegins = false;
                        switch (Phase)
                        {
                        case PHASE_NORMAL:
                            speechPhaseEnd = 1;
                            break;
                        case PHASE_DARKNESS:
                            speechPhaseEnd = 4;
                            break;
                        case PHASE_ARMAGEDDON:
                            speechPhaseEnd = 7;
                            break;
                        case PHASE_SACRIFICE:
                            speechPhaseEnd = 12;
                            break;
                        }
                    }
                    if (Speeches[speechCount].timer < SpeechTimer)
                    {
                        SpeechTimer = 0;
                        if (pInstance)
                            if (Creature* pSpeechCreature = Unit::GetCreature(*me, pInstance->GetData64(Speeches[speechCount].pCreature)))
                                DoScriptText(Speeches[speechCount].textid, pSpeechCreature);
                        if (speechCount == 12)
                            if (Creature* pAnveena =  Unit::GetCreature(*me, pInstance->GetData64(DATA_ANVEENA)))
                                pAnveena->CastSpell(me, SPELL_SACRIFICE_OF_ANVEENA, false);
                        //   ChangeTimers(true, 10000); // Kil should do an emote while screaming without attacking for 10 seconds
                        if (speechCount == speechPhaseEnd)
                            TimerIsDeactivated[TIMER_SPEECH] = true;
                        speechCount++;
                    }
                    SpeechTimer += diff;
                    break;
                case TIMER_SOUL_FLAY:
                    if (!me->IsNonMeleeSpellCast(false))
                    {
                        DoCastVictim( SPELL_SOUL_FLAY_SLOW, false);
                        DoCastVictim( SPELL_SOUL_FLAY, false);
                        Timer[TIMER_SOUL_FLAY] = 3500;
                    }
                    break;
                case TIMER_LEGION_LIGHTNING:
                    if (!me->IsNonMeleeSpellCast(false))
                    {
                        Unit* pRandomPlayer = NULL;

                        me->RemoveAurasDueToSpell(SPELL_SOUL_FLAY);
                        for (uint8 z = 0; z < 6; ++z)
                        {
                            pRandomPlayer = SelectTarget(SELECT_TARGET_RANDOM, 0, 100, true);
                            if (!pRandomPlayer || !pRandomPlayer->HasAura(SPELL_VENGEANCE_OF_THE_BLUE_FLIGHT, 0))
                                break;
                        }

                        if (pRandomPlayer)
                            DoCast(pRandomPlayer, SPELL_LEGION_LIGHTNING, false);
                        else
                            error_log("try to cast SPELL_LEGION_LIGHTNING on invalid target");

                        Timer[TIMER_LEGION_LIGHTNING] = (Phase == PHASE_SACRIFICE) ? 18000 : 30000; // 18 seconds in PHASE_SACRIFICE
                        Timer[TIMER_SOUL_FLAY] = 2500;
                    }
                    break;
                case TIMER_FIRE_BLOOM:
                    if (!me->IsNonMeleeSpellCast(false))
                    {
                        me->RemoveAurasDueToSpell(SPELL_SOUL_FLAY);
                        DoCastAOE(SPELL_FIRE_BLOOM, false);
                        Timer[TIMER_FIRE_BLOOM] = (Phase == PHASE_SACRIFICE) ? 25000 : 40000; // 25 seconds in PHASE_SACRIFICE
                        Timer[TIMER_SOUL_FLAY] = 1000;
                    }
                    break;
                case TIMER_SUMMON_SHILEDORB:
                    for (uint8 i = 1; i < Phase; ++i)
                    {
                        float sx, sy;
                        sx = ShieldOrbLocations[0][0] + sin(ShieldOrbLocations[i][0]);
                        sy = ShieldOrbLocations[0][1] + sin(ShieldOrbLocations[i][1]);
                        me->SummonCreature(CREATURE_SHIELD_ORB, sx, sy, SHIELD_ORB_Z, 0, TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 45000);
                    }
                    Timer[TIMER_SUMMON_SHILEDORB] = urand(30000, 60000); // 30-60seconds cooldown
                    Timer[TIMER_SOUL_FLAY] = 2000;
                    break;
                case TIMER_SHADOW_SPIKE: //Phase 3
                    if (!me->IsNonMeleeSpellCast(false))
                    {
                        DoCastAOE(SPELL_SINISTER_REFLECTION, true);
                        DoCastAOE(SPELL_SHADOW_SPIKE, false);
                        ChangeTimers(true, 30000);
                        Timer[TIMER_SHADOW_SPIKE] = 0;
                        TimerIsDeactivated[TIMER_SPEECH] = false;
                    }
                    break;
                case TIMER_FLAME_DART: //Phase 3
                    DoCastAOE(SPELL_FLAME_DART, false);
                    Timer[TIMER_FLAME_DART] = 3000; //TODO Timer
                    break;
                case TIMER_DARKNESS: //Phase 3
                    if (!me->IsNonMeleeSpellCast(false))
                    {
                        // Begins to channel for 8 seconds, then deals 50'000 damage to all raid members.
                        if (!IsInDarkness)
                        {
                            DoScriptText(EMOTE_KJ_DARKNESS, me);
                            DoCastAOE(SPELL_DARKNESS_OF_A_THOUSAND_SOULS, false);
                            ChangeTimers(true, 9000);
                            Timer[TIMER_DARKNESS] = 8750;
                            TimerIsDeactivated[TIMER_DARKNESS] = false;
                            if (Phase == PHASE_SACRIFICE)
                                TimerIsDeactivated[TIMER_ARMAGEDDON] = false;
                            IsInDarkness = true;
                        }
                        else
                        {
                            Timer[TIMER_DARKNESS] = (Phase == PHASE_SACRIFICE) ? 15000 : urand(40000, 70000);
                            IsInDarkness = false;
                            DoCastAOE(SPELL_DARKNESS_OF_A_THOUSAND_SOULS_DAMAGE);
                            DoScriptText(RAND(SAY_KJ_DARKNESS1, SAY_KJ_DARKNESS2, SAY_KJ_DARKNESS3), me);
                        }
                        Timer[TIMER_SOUL_FLAY] = 9000;
                    }
                    break;
                case TIMER_ORBS_EMPOWER: //Phase 3
                    if (pInstance)
                        if (Creature* pKalec = Unit::GetCreature(*me, pInstance->GetData64(DATA_KALECGOS_KJ)))
                        {
                            switch (Phase)
                            {
                            case PHASE_SACRIFICE:
                                CAST_AI(boss_kalecgos_kjAI, pKalec->AI())->EmpowerOrb(true);
                                break;
                            default:
                                CAST_AI(boss_kalecgos_kjAI, pKalec->AI())->EmpowerOrb(false);
                                break;
                            }
                        }
                    OrbActivated = true;
                    TimerIsDeactivated[TIMER_ORBS_EMPOWER] = true;
                    break;
                case TIMER_ARMAGEDDON: //Phase 4
                    DoCast(me, SPELL_ARMAGEDDON_SUMMON_TRIGGER, true);
                    Timer[TIMER_ARMAGEDDON] = 26000;
                    break;
                }
            }
        }
        DoMeleeAttackIfReady();
        //Time runs over!
        for (uint8 i = 0; i < ActiveTimers; ++i)
            if (!TimerIsDeactivated[i])
            {
                Timer[i] -= diff;
                if (((int32)Timer[i]) < 0) Timer[i] = 0;
            }

        //Phase 3
        if (Phase <= PHASE_NORMAL && !IsInDarkness)
        {
            if (Phase == PHASE_NORMAL && HealthBelowPct(85))
            {
                Phase = PHASE_DARKNESS;
                ActiveTimers = 9;
                EnterNextPhase();
            }
            else return;
        }

        //Phase 4
        if (Phase <= PHASE_DARKNESS && !IsInDarkness)
        {
            if (Phase == PHASE_DARKNESS && HealthBelowPct(55))
            {
                Phase = PHASE_ARMAGEDDON;
                ActiveTimers = 10;
                EnterNextPhase();
            }
            else return;
        }

        //Phase 5 specific spells all we can
        if (Phase <= PHASE_ARMAGEDDON && !IsInDarkness)
        {
            if (Phase == PHASE_ARMAGEDDON && HealthBelowPct(25))
            {
                Phase = PHASE_SACRIFICE;
                EnterNextPhase();
            }
            else return;
        }
    }
};

CreatureAI* GetAI_boss_kiljaeden(Creature* pCreature)
{
    return new boss_kiljaedenAI (pCreature);
}

//AI for Hand of the Deceiver
struct mob_hand_of_the_deceiverAI : public ScriptedAI
{
    mob_hand_of_the_deceiverAI(Creature* c) : ScriptedAI(c)
    {
        pInstance = (ScriptedInstance*)c->GetInstanceData();
    }

    ScriptedInstance* pInstance;

    uint32 ShadowBoltVolleyTimer;
    uint32 FelfirePortalTimer;

    void Reset()
    {
        // @todo Timers!
        ShadowBoltVolleyTimer = urand(8000, 14000); // So they don't all cast it in the same moment.
        FelfirePortalTimer = 20000;
        if (pInstance)
            pInstance->SetData(DATA_KILJAEDEN_EVENT, NOT_STARTED);
    }

    void JustSummoned(Creature* summoned)
    {
        summoned->SetFaction(me->GetFaction());
        summoned->SetLevel(me->getLevel());
    }

    void EnterCombat(Unit* who)
    {
        if (pInstance)
        {
            pInstance->SetData(DATA_KILJAEDEN_EVENT, IN_PROGRESS);
            if (Creature* pControl = Unit::GetCreature(*me, pInstance->GetData64(DATA_KILJAEDEN_CONTROLLER)))
                pControl->AddThreat(who, 1.0f);
        }
        me->InterruptNonMeleeSpells(true);
    }

    void JustDied(Unit* /*killer*/)
    {
        if (!pInstance)
            return;

        if (Creature* pControl = Unit::GetCreature(*me, pInstance->GetData64(DATA_KILJAEDEN_CONTROLLER)))
            ++(CAST_AI(mob_kiljaeden_controllerAI, pControl->AI())->deceiverDeathCount);
    }

    void UpdateAI(const uint32 diff)
    {
        if (!me->IsInCombat())
            DoCast(me, SPELL_SHADOW_CHANNELING);

        if (!UpdateVictim())
            return;

        // Gain Shadow Infusion at 20% health
        if (HealthBelowPct(20) && !me->HasAura(SPELL_SHADOW_INFUSION, 0))
            DoCast(me, SPELL_SHADOW_INFUSION, true);

        // Shadow Bolt Volley - Shoots Shadow Bolts at all enemies within 30 yards, for ~2k Shadow damage.
        if (ShadowBoltVolleyTimer <= diff)
        {
            DoCastVictim( SPELL_SHADOW_BOLT_VOLLEY);
            ShadowBoltVolleyTimer = 12000;
        }
        else
            ShadowBoltVolleyTimer -= diff;

        // Felfire Portal - Creatres a portal, that spawns Volatile Felfire Fiends, which do suicide bombing.
        if (FelfirePortalTimer <= diff)
        {
            DoCast(me, SPELL_FELFIRE_PORTAL);
            FelfirePortalTimer = 20000;
        }
        else FelfirePortalTimer -= diff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_mob_hand_of_the_deceiver(Creature* pCreature)
{
    return new mob_hand_of_the_deceiverAI (pCreature);
}

//AI for Felfire Portal
struct mob_felfire_portalAI : public Scripted_NoMovementAI
{
    mob_felfire_portalAI(Creature* c) : Scripted_NoMovementAI(c) {}

    uint32 uiSpawnFiendTimer;

    void Reset()
    {
        uiSpawnFiendTimer = 5000;
        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_NON_ATTACKABLE);
    }

    void JustSummoned(Creature* summoned)
    {
        summoned->SetFaction(me->GetFaction());
        summoned->SetLevel(me->getLevel());
    }

    void UpdateAI(const uint32 diff)
    {
        if (!UpdateVictim())
            return;

        if (uiSpawnFiendTimer <= diff)
        {
            DoCast(me, SPELL_SUMMON_FELFIRE_FIEND);
            uiSpawnFiendTimer = urand(4000, 8000);
        }
        else uiSpawnFiendTimer -= diff;
    }
};

CreatureAI* GetAI_mob_felfire_portal(Creature* pCreature)
{
    return new mob_felfire_portalAI (pCreature);
}

//AI for Felfire Fiend
struct mob_volatile_felfire_fiendAI : public ScriptedAI
{
    mob_volatile_felfire_fiendAI(Creature* c) : ScriptedAI(c)
    {
        pInstance = (ScriptedInstance*)c->GetInstanceData();
    }

    ScriptedInstance* pInstance;

    uint32 uiExplodeTimer;
    uint32 uiKillTimer;

    bool bLockedTarget;

    void Reset()
    {
        uiExplodeTimer = 2000;
        uiKillTimer = 0;
        DoZoneInCombatWithPlayers(true);
        bLockedTarget = false;
    }

    void DamageTaken(Unit* /*done_by*/, uint32& damage)
    {
        if (damage > me->GetHealth())
            DoCast(me, SPELL_FELFIRE_FISSION, true);
    }

    void UpdateAI(const uint32 diff)
    {
        if (pInstance && pInstance->GetData(DATA_KILJAEDEN_EVENT) == NOT_STARTED)
            me->DisappearAndDie(false);

        if (!UpdateVictim())
            return;

        if (!bLockedTarget)
        {
            me->AddThreat(me->GetVictim(), 10000000.0f);
            bLockedTarget = true;
        }

        if (uiExplodeTimer)
        {
            if (uiExplodeTimer <= diff)
                uiExplodeTimer = 0;
            else uiExplodeTimer -= diff;
        }
        else if (me->IsWithinDistInMap(me->GetVictim(), 3)) // Explode if it's close enough to it's target
        {
            DoCastVictim( SPELL_FELFIRE_FISSION);
            uiKillTimer = 500;
        }

        if (uiKillTimer)
        {
            if (uiKillTimer <= diff)
                me->DisappearAndDie();
            else uiKillTimer -= diff;
        }
    }
};

CreatureAI* GetAI_mob_volatile_felfire_fiend(Creature* pCreature)
{
    return new mob_volatile_felfire_fiendAI (pCreature);
}

//AI for Armageddon target
struct mob_armageddonAI : public Scripted_NoMovementAI
{
    mob_armageddonAI(Creature* c) : Scripted_NoMovementAI(c) {}

    uint8 spell;
    uint32 uiTimer;

    void Reset()
    {
        spell = 0;
        uiTimer = 0;
    }

    void UpdateAI(const uint32 diff)
    {
        if (uiTimer <= diff)
        {
            switch (spell)
            {
            case 0:
                DoCast(me, SPELL_ARMAGEDDON_VISUAL, true);
                ++spell;
                break;
            case 1:
                DoCast(me, SPELL_ARMAGEDDON_VISUAL2, true);
                uiTimer = 9000;
                ++spell;
                break;
            case 2:
                DoCast(me, SPELL_ARMAGEDDON_TRIGGER, true);
                ++spell;
                uiTimer = 5000;
                break;
            case 3:
                me->DisappearAndDie();
                break;
            }
        }
        else uiTimer -= diff;
    }
};

CreatureAI* GetAI_mob_armageddon(Creature* pCreature)
{
    return new mob_armageddonAI (pCreature);
}

//AI for Shield Orbs
struct mob_shield_orbAI : public ScriptedAI
{
    mob_shield_orbAI(Creature* c) : ScriptedAI(c)
    {
        pInstance = (ScriptedInstance*)c->GetInstanceData();
    }

    ScriptedInstance* pInstance;

    bool bPointReached;
    bool bClockwise;
    uint32 uiTimer;
    uint32 uiCheckTimer;
    float x, y, r, c, mx, my;

    void Reset()
    {
        bPointReached = true;
        uiTimer = urand(500, 1000);
        uiCheckTimer = 2000;
        r = 17;
        c = 0;
        mx = ShieldOrbLocations[0][0];
        my = ShieldOrbLocations[0][1];
        bClockwise = urand(0, 1);
        DoZoneInCombatWithPlayers(true);
    }

    void UpdateAI(const uint32 diff)
    {
        if (bPointReached)
        {
            if (bClockwise)
            {
                y = my - r * sin(c);
                x = mx - r * cos(c);
            }
            else
            {
                y = my + r * sin(c);
                x = mx + r * cos(c);
            }
            bPointReached = false;
            uiCheckTimer = 1000;
            me->GetMotionMaster()->MovePoint(1, x, y, SHIELD_ORB_Z);
            c += float(M_PI) / 32;
            if (c >= 2 * M_PI) c = 0;
        }
        else
        {
            if (uiCheckTimer <= diff)
                bPointReached = true;
            else uiCheckTimer -= diff;
        }

        if (uiTimer <= diff)
        {
            if (Unit* random = Unit::GetUnit(*me, pInstance ? pInstance->GetData64(DATA_PLAYER_GUID) : 0))
                DoCast(random, SPELL_SHADOW_BOLT, false);
            uiTimer = urand(500, 1000);
        }
        else uiTimer -= diff;
    }

    void MovementInform(uint32 type, uint32 /*id*/)
    {
        if (type != POINT_MOTION_TYPE)
            return;

        bPointReached = true;
    }
};

CreatureAI* GetAI_mob_shield_orb(Creature* pCreature)
{
    return new mob_shield_orbAI (pCreature);
}

//AI for Sinister Reflection
struct mob_sinster_reflectionAI : public ScriptedAI
{
    mob_sinster_reflectionAI(Creature* c) : ScriptedAI(c) {}

    uint8 victimClass;
    uint32 uiTimer[3];

    void Reset()
    {
        uiTimer[0] = 0;
        uiTimer[1] = 0;
        uiTimer[2] = 0;
        victimClass = 0;
    }

    void UpdateAI(const uint32 diff)
    {
        if (!UpdateVictim())
            return;

        if ((victimClass == 0) && me->GetVictim())
        {
            victimClass = me->GetVictim()->getClass();
            switch (victimClass)
            {
            case CLASS_DRUID:
                break;
            case CLASS_HUNTER:
                break;
            case CLASS_MAGE:
                break;
            case CLASS_WARLOCK:
                break;
            case CLASS_WARRIOR:
                me->SetCanDualWield(true);
                break;
            case CLASS_PALADIN:
                break;
            case CLASS_PRIEST:
                break;
            case CLASS_SHAMAN:
                me->SetCanDualWield(true);
                break;
            case CLASS_ROGUE:
                me->SetCanDualWield(true);
                break;
            }
        }

        switch (victimClass)
        {
        case CLASS_DRUID:
            if (uiTimer[1] <= diff)
            {
                DoCastVictim( SPELL_SR_MOONFIRE, false);
                uiTimer[1] = urand(2000, 4000);
            }
            DoMeleeAttackIfReady();
            break;
        case CLASS_HUNTER:
            if (uiTimer[1] <= diff)
            {
                DoCastVictim( SPELL_SR_MULTI_SHOT, false);
                uiTimer[1] = urand(8000, 10000);
            }
            if (uiTimer[2] <= diff)
            {
                DoCastVictim( SPELL_SR_SHOOT, false);
                uiTimer[2] = urand(4000, 6000);
            }
            if (me->IsWithinMeleeRange(me->GetVictim(), 6))
            {
                if (uiTimer[0] <= diff)
                {
                    DoCastVictim( SPELL_SR_MULTI_SHOT, false);
                    uiTimer[0] = urand(6000, 8000);
                }
                DoMeleeAttackIfReady();
            }
            break;
        case CLASS_MAGE:
            if (uiTimer[1] <= diff)
            {
                DoCastVictim( SPELL_SR_FIREBALL, false);
                uiTimer[1] = urand(2000, 4000);
            }
            DoMeleeAttackIfReady();
            break;
        case CLASS_WARLOCK:
            if (uiTimer[1] <= diff)
            {
                DoCastVictim( SPELL_SR_SHADOW_BOLT, false);
                uiTimer[1] = urand(3000, 5000);
            }
            if (uiTimer[2] <= diff)
            {
                DoCast(SelectTarget(SELECT_TARGET_RANDOM, 0, 100, true), SPELL_SR_CURSE_OF_AGONY, true);
                uiTimer[2] = urand(2000, 4000);
            }
            DoMeleeAttackIfReady();
            break;
        case CLASS_WARRIOR:
            if (uiTimer[1] <= diff)
            {
                DoCastVictim( SPELL_SR_WHIRLWIND, false);
                uiTimer[1] = urand(9000, 11000);
            }
            DoMeleeAttackIfReady();
            break;
        case CLASS_PALADIN:
            if (uiTimer[1] <= diff)
            {
                DoCastVictim( SPELL_SR_HAMMER_OF_JUSTICE, false);
                uiTimer[1] = urand(6000, 8000);
            }
            if (uiTimer[2] <= diff)
            {
                DoCastVictim( SPELL_SR_HOLY_SHOCK, false);
                uiTimer[2] = urand(2000, 4000);
            }
            DoMeleeAttackIfReady();
            break;
        case CLASS_PRIEST:
            if (uiTimer[1] <= diff)
            {
                DoCastVictim( SPELL_SR_HOLY_SMITE, false);
                uiTimer[1] = urand(4000, 6000);
            }
            if (uiTimer[2] <= diff)
            {
                DoCast(me, SPELL_SR_RENEW, false);
                uiTimer[2] = urand(6000, 8000);
            }
            DoMeleeAttackIfReady();
            break;
        case CLASS_SHAMAN:
            if (uiTimer[1] <= diff)
            {
                DoCastVictim( SPELL_SR_EARTH_SHOCK, false);
                uiTimer[1] = urand(4000, 6000);
            }
            DoMeleeAttackIfReady();
            break;
        case CLASS_ROGUE:
            if (uiTimer[1] <= diff)
            {
                DoCastVictim( SPELL_SR_HEMORRHAGE, true);
                uiTimer[1] = urand(4000, 6000);
            }
            DoMeleeAttackIfReady();
            break;
        }
        debug_log("Sinister-Timer");
        for (uint8 i = 0; i < 3; ++i)
            uiTimer[i] -= diff;
    }
};

CreatureAI* GetAI_mob_sinster_reflection(Creature* pCreature)
{
    return new mob_sinster_reflectionAI (pCreature);
}

void AddSC_boss_kiljaeden()
{
    Script* newscript;

    newscript = new Script;
    newscript->pGOHello = &GOHello_go_orb_of_the_blue_flight;
    newscript->Name = "go_orb_of_the_blue_flight";
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->GetAI = &GetAI_boss_kalecgos_kj;
    newscript->Name = "boss_kalecgos_kj";
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->GetAI = &GetAI_boss_kiljaeden;
    newscript->Name = "boss_kiljaeden";
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->GetAI = &GetAI_mob_kiljaeden_controller;
    newscript->Name = "mob_kiljaeden_controller";
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->GetAI = &GetAI_mob_hand_of_the_deceiver;
    newscript->Name = "mob_hand_of_the_deceiver";
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->GetAI = &GetAI_mob_felfire_portal;
    newscript->Name = "mob_felfire_portal";
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->GetAI = &GetAI_mob_volatile_felfire_fiend;
    newscript->Name = "mob_volatile_felfire_fiend";
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->GetAI = &GetAI_mob_armageddon;
    newscript->Name = "mob_armageddon";
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->GetAI = &GetAI_mob_shield_orb;
    newscript->Name = "mob_shield_orb";
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->GetAI = &GetAI_mob_sinster_reflection;
    newscript->Name = "mob_sinster_reflection";
    newscript->RegisterSelf();
}
