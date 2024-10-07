/*
 * Copyright (C) 2016+ AzerothCore <www.azerothcore.org>, released under GNU GPLv2+ license
 * Written by Nefertumm : https://github.com/Nefertumm
 */

#include "ScriptMgr.h"
#include "CharacterDatabase.h"
#include "GameTime.h"
#include "Player.h"
#include "Config.h"
#include "Chat.h"

enum SpellIds
{
    SPELL_DESERTER = 26013
};

class CustomDeserter : public BGScript
{
public:
    CustomDeserter() : BGScript("CustomDeserter") { }

    uint32 GetDeserterCount(ObjectGuid::LowType const guid, time_t time)
    {
        uint32 const days = sConfigMgr->GetOption<uint32>("DeserterDays", 7);
        time_t from = days != 0 ? time - static_cast<time_t>(days) * DAY : 0;
        QueryResult result = CharacterDatabase.Query("SELECT COUNT(*) FROM custom_deserter WHERE Guid = {} AND Time BETWEEN {} AND {}", guid, from, time);
        if (result)
            return (*result)[0].Get<uint32>();
        else return 0;
    }

    void OnBattlegroundRemovePlayerAtLeave(Battleground* bg, Player* player) override
    {
        if (!player)
            return;

        if (bg->isBattleground() && !player->IsGameMaster())
        {
            BattlegroundStatus status = bg->GetStatus();
            if (status == STATUS_IN_PROGRESS || status == STATUS_WAIT_JOIN)
            {
                int32 const baseDuration = sConfigMgr->GetOption<int32>("DeserterBaseDuration", 30) * MINUTE * IN_MILLISECONDS;

                time_t now = GameTime::GetGameTime().count();
                ObjectGuid::LowType const guid = player->GetGUID().GetCounter();
                // First we get the amount of deserters from db
                uint32 deserterCount = GetDeserterCount(guid, now);
                // Calculate deserter duration
                int32 duration = baseDuration + (deserterCount * baseDuration);

                // After that we check if the player already has deserter debuff
                if (Aura* deserterAura = player->GetAura(SPELL_DESERTER))
                {
                    int32 auraDuration = deserterAura->GetDuration();
                    deserterAura->SetDuration(duration);
                }
                else
                {
                    Aura* aura = player->AddAura(SPELL_DESERTER, player);
                    if (!aura)
                        return;
                    aura->SetDuration(duration);
                }
                // Save deserter to db
                CharacterDatabase.DirectExecute("INSERT INTO custom_deserter (Guid, Time, Duration) VALUES ({}, {}, {})", guid, now, duration);
            }
        }
    }
};

// Add all scripts in one
void AddCustomDeserterScripts()
{
    new CustomDeserter();
}
