/* Copyright (C) 2006 - 2008 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*/
 
/* ScriptData
Dev: Acherus
SDName: Boss_Brutallus
SD%Complete: 90
SDComment: Find a way to start the intro, best code for the intro
EndScriptData */
 
#include "ScriptPCH.h"
#include "sunwell_plateau.h"
 
enum Quotes
{
YELL_INTRO                 =   -1580017,
YELL_INTRO_BREAK_ICE       =   -1580018,
YELL_INTRO_CHARGE          =   -1580019,
YELL_INTRO_KILL_MADRIGOSA  =   -1580020,
YELL_INTRO_TAUNT           =   -1580021,
 
YELL_MADR_ICE_BARRIER      =   -1580031,
YELL_MADR_INTRO            =   -1580032,
YELL_MADR_ICE_BLOCK        =   -1580033,
YELL_MADR_TRAP             =   -1580034,
YELL_MADR_DEATH            =   -1580035,
 
YELL_AGGRO                 =   -1580022,
YELL_KILL1                 =   -1580023,
YELL_KILL2                 =   -1580024,
YELL_KILL3                 =   -1580025,
YELL_LOVE1                 =   -1580026,
YELL_LOVE2                 =   -1580027,
YELL_LOVE3                 =   -1580028,
YELL_BERSERK               =   -1580029,
YELL_DEATH                 =   -1580030
};
 
enum Spells
{
    SPELL_METEOR_SLASH                 =   45150,
    SPELL_BURN                         =   46394,
    SPELL_STOMP                        =   45185,
    SPELL_BERSERK                      =   26662,
    SPELL_DUAL_WIELD                   =   42459,
 
    SPELL_INTRO_FROST_BLAST            =   45203,
    SPELL_INTRO_FROSTBOLT              =   44843,
SPELL_INTRO_FREEZE                 =   45065,
    SPELL_INTRO_ENCAPSULATE            =   45665,
    SPELL_INTRO_ENCAPSULATE_CHANELLING =   45661
};
 
#define FELMYST 25038
 
struct boss_brutallusAI : public ScriptedAI
{
    boss_brutallusAI(Creature *c) : ScriptedAI(c)
    {
        pInstance = c->GetInstanceData();
        Intro = true;
    }
 
    ScriptedInstance* pInstance;
 
    uint32 SlashTimer;
    uint32 BurnTimer;
    uint32 StompTimer;
    uint32 BerserkTimer;
 
    uint32 IntroPhase;
    uint32 IntroPhaseTimer;
uint32 IntroAttackTimer;
    uint32 IntroFrostBoltTimer;
uint32 IntroFlyTimer;
 
    bool Intro;
    bool IsIntro;
    bool Enraged;
 
    void Reset()
    {
        SlashTimer = 11000;
        StompTimer = 30000;
        BurnTimer = 60000;
        BerserkTimer = 360000;
 
        IntroPhase = 0;
        IntroPhaseTimer = 0;
        IntroFrostBoltTimer = 0;
IntroAttackTimer = 0;
 
        IsIntro = false;
        Enraged = false;
 
        DoCast(me, SPELL_DUAL_WIELD, true);
 
        if (pInstance)
            pInstance->SetData(DATA_BRUTALLUS_EVENT, NOT_STARTED);
    }
 
    void EnterCombat(Unit *who)
    {
        if(!Intro)
   DoScriptText(YELL_AGGRO, me);
 
        if (pInstance && !IsIntro)
            pInstance->SetData(DATA_BRUTALLUS_EVENT, IN_PROGRESS);
    }
 
    void KilledUnit(Unit* victim)
    {
        if(!Intro)
   DoScriptText(RAND(YELL_KILL1,YELL_KILL2,YELL_KILL3), me);
    }
 
    void JustDied(Unit* Killer)
    {
        DoScriptText(YELL_DEATH, me);
        if (pInstance)
        {
            pInstance->SetData(DATA_BRUTALLUS_EVENT, DONE);
            float x,y,z;
Creature *Madrigosa = Unit::GetCreature(*me, pInstance ? pInstance->GetData64(DATA_MADRIGOSA) : 0);
            Madrigosa->CastSpell(Madrigosa, 44885, true);
Madrigosa->GetPosition(x,y,z);
Madrigosa->SetVisibility(VISIBILITY_OFF);
            me->SummonCreature(FELMYST, x,y, z+30, me->GetOrientation(), TEMPSUMMON_MANUAL_DESPAWN, 0);
        }
    }
 
    void EnterEvadeMode()
    {
        if (!Intro)
            ScriptedAI::EnterEvadeMode();
    }
 
    void StartIntro()
    {
        if (!Intro || IsIntro)
            return;
        error_log("Start Intro");
        Creature *Madrigosa = Unit::GetCreature(*me, pInstance ? pInstance->GetData64(DATA_MADRIGOSA) : 0);
        if (Madrigosa)
        {
            Madrigosa->Respawn();
Madrigosa->setActive(true);
            IsIntro = true;
            Madrigosa->SetMaxHealth(me->GetMaxHealth());
            Madrigosa->SetHealth(me->GetMaxHealth());
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
           
        }else
        {
            //Madrigosa not found, end intro
            error_log("Madrigosa was not found");
            EndIntro();
        }
    }
 
    void EndIntro()
    {
        me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
Intro = false;
        IsIntro = false;
        error_log("End Intro");
    }
 
    void AttackStart(Unit* pWho)
    {
        if (!pWho || Intro || IsIntro)
            return;
        ScriptedAI::AttackStart(pWho);
    }
 
    void DoIntro()
    {
        Creature *Madrigosa = Unit::GetCreature(*me, pInstance ? pInstance->GetData64(DATA_MADRIGOSA) : 0);
        if (!Madrigosa)
            return;
 
        switch (IntroPhase)
        {
   case 0:
IntroPhaseTimer = 1000;
break;
   case 1:
                DoScriptText(YELL_MADR_ICE_BARRIER, Madrigosa);
IntroPhaseTimer = 8000;
                break;
            case 2:
                DoScriptText(YELL_MADR_INTRO, Madrigosa, me);
Madrigosa->SetReactState(REACT_PASSIVE);
                IntroPhaseTimer = 7000;
                break;
            case 3:
                DoScriptText(YELL_INTRO, me, Madrigosa);
me->SetInFront(Madrigosa);
                Madrigosa->SetInFront(me);
                IntroPhaseTimer = 4000;
                break;
case 4:
Madrigosa->CombatStart(me, true);
IntroAttackTimer = 2000;
IntroPhaseTimer = 10000;
break;
case 5:
me->AttackStop();
                Madrigosa->AttackStop();
Madrigosa->SetUnitMovementFlags(MOVEFLAG_LEVITATING | MOVEFLAG_ONTRANSPORT);
Madrigosa->SetSpeed(MOVE_RUN, 3.0f, true);
Madrigosa->GetMotionMaster()->MovePath(31000, false);
Madrigosa->SetInFront(me);
IntroAttackTimer = 3500;
                IntroFrostBoltTimer = 3500;
                IntroPhaseTimer = 13000;
                break;
            case 6:
Madrigosa->SetPosition(Madrigosa->GetPositionX(),Madrigosa->GetPositionY(),Madrigosa->GetPositionZ(),4.1f,true);
                DoScriptText(YELL_INTRO_BREAK_ICE, me);
                IntroPhaseTimer = 6000;
                break;
            case 7:
Madrigosa->SetUnitMovementFlags(MOVEFLAG_NONE);
                Madrigosa->CastSpell(me, 45661, true);
                DoScriptText(YELL_MADR_TRAP, Madrigosa);
                DoCast(me, SPELL_INTRO_ENCAPSULATE);
me->SetSpeed(MOVE_RUN, 4.0f, true);
me->GetMotionMaster()->MovePoint(0,1494.691895, 548.588989, 39.162289);
me->SetPosition(1494.691895, 548.588989, 25.652, 1.34f, true);
IntroAttackTimer = 3000;
                IntroPhaseTimer = 6000;
                break;
case 8:
DoScriptText(YELL_INTRO_CHARGE, me);
me->SetSpeed(MOVE_RUN, 3.0f, true);
me->GetMotionMaster()->MovePath(30000, false);
   IntroPhaseTimer = 4000;
                break;
case 9:
                me->DealDamage(Madrigosa, Madrigosa->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, true);
                DoScriptText(YELL_MADR_DEATH, Madrigosa);
                me->SetHealth(me->GetMaxHealth());
                me->AttackStop();
me->SetSpeed(MOVE_RUN, 2.0f, true);
IntroPhaseTimer = 7000;
                break;
            case 10:
                DoScriptText(YELL_INTRO_KILL_MADRIGOSA, me);
                me->StopMoving();
me->SetSpeed(MOVE_RUN, 1.0f, true);
me->GetMotionMaster()->MovePoint(0, me->GetPositionX()+3,me->GetPositionY(),me->GetPositionZ());
me->SetPosition(me->GetPositionX()+3,me->GetPositionY(),me->GetPositionZ(), 1.1, false);
                Madrigosa->setDeathState(CORPSE);
                IntroPhaseTimer = 8000;
                break;
            case 11:
                DoScriptText(YELL_INTRO_TAUNT, me);
                IntroPhaseTimer = 5000;
                break;
            case 12:
                EndIntro();
                break;
        }
    }
 
    void MoveInLineOfSight(Unit *who)
    {
if (!who->isTargetableForAttack() || !me->IsHostileTo(who))
            return;
        if (pInstance && Intro)
            pInstance->SetData(DATA_BRUTALLUS_EVENT, SPECIAL);
 
        if (Intro && !IsIntro)
            StartIntro();
        if (!Intro)
            ScriptedAI::MoveInLineOfSight(who);
    }
 
    void UpdateAI(const uint32 diff)
    {
        if (IsIntro)
        {
            if (IntroPhaseTimer > diff)
                IntroPhaseTimer = IntroPhaseTimer - diff;
            else
{
++IntroPhase;
DoIntro();
   }
 
            if (IntroPhase == 3 + 1 )
            {
                if (IntroAttackTimer < diff)
                {
                    if (Creature *Madrigosa = Unit::GetCreature(*me, pInstance ? pInstance->GetData64(DATA_MADRIGOSA) : 0))
                    {
              me->Attack(Madrigosa, true);
  me->AttackerStateUpdate(Madrigosa,BASE_ATTACK,false);
  Madrigosa->Attack(me, true);
  Madrigosa->AttackerStateUpdate(me,BASE_ATTACK,false);
  IntroAttackTimer = 1000;
}
} else IntroAttackTimer -= diff;
}
 
if (IntroPhase == 4 + 1)
            {
if (IntroAttackTimer < diff)
{
if (Creature *Madrigosa = Unit::GetCreature(*me, pInstance ? pInstance->GetData64(DATA_MADRIGOSA) : 0))
                    {
  Madrigosa->CastSpell(me, SPELL_INTRO_FREEZE, true);
      DoCast(me, SPELL_INTRO_FROST_BLAST);
  IntroAttackTimer = 12000;
   }
} else IntroAttackTimer -= diff;
                if (IntroFrostBoltTimer < diff)
                {
                    if (Creature *Madrigosa = Unit::GetCreature(*me, pInstance ? pInstance->GetData64(DATA_MADRIGOSA) : 0))
                    {
Madrigosa->SetInCombatState(true,me);
Madrigosa->CastSpell(me, SPELL_INTRO_FROSTBOLT, true);
                        IntroFrostBoltTimer = 1100;
                    }
 
                } else IntroFrostBoltTimer -= diff;
 
            }
if (IntroPhase == 7 + 1)
            {
if (IntroAttackTimer < diff)
                {
                    if (Creature *Madrigosa = Unit::GetCreature(*me, pInstance ? pInstance->GetData64(DATA_MADRIGOSA) : 0))
                    {
              me->Attack(Madrigosa, true);
  me->CastSpell(Madrigosa, 45185, true);
  IntroAttackTimer = 1700;
}
} else IntroAttackTimer -= diff;
}
            if (!UpdateVictim())
                return;
            DoMeleeAttackIfReady();
        }
 
        if (!UpdateVictim() || IsIntro)
            return;
 
        if (SlashTimer <= diff)
        {
            DoCast(me->getVictim(), SPELL_METEOR_SLASH);
            SlashTimer = 11000;
        } else SlashTimer -= diff;
 
        if (StompTimer <= diff)
        {
            DoScriptText(RAND(YELL_LOVE1,YELL_LOVE2,YELL_LOVE3), me);
            DoCast(me->getVictim(), SPELL_STOMP);
            StompTimer = 30000;
        } else StompTimer -= diff;
 
        if (BurnTimer <= diff)
        {
            std::list<Unit*> pTargets;
            SelectTargetList(pTargets, 10, SELECT_TARGET_RANDOM, 100, true);
            for (std::list<Unit*>::const_iterator i = pTargets.begin(); i != pTargets.end(); ++i)
                if (!(*i)->HasAura(SPELL_BURN, 0))
                {
                    (*i)->CastSpell((*i), SPELL_BURN, true);
                    break;
                }
            BurnTimer = urand(60000,180000);
        } else BurnTimer -= diff;
 
        if (BerserkTimer <= diff && !Enraged)
        {
            DoScriptText(YELL_BERSERK, me);
            DoCast(me, SPELL_BERSERK);
            Enraged = true;
        } else BerserkTimer -= diff;
 
        DoMeleeAttackIfReady();
    }
};
 
CreatureAI* GetAI_boss_brutallus(Creature* pCreature)
{
    return new boss_brutallusAI (pCreature);
}
 
void AddSC_boss_brutallus()
{
    Script *newscript;
 
    newscript = new Script;
    newscript->Name = "boss_brutallus";
    newscript->GetAI = &GetAI_boss_brutallus;
    newscript->RegisterSelf();
}