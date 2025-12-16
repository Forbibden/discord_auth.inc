#if defined _DISCORD_AUTH_INC_
    #endinput
#endif
#define _DISCORD_AUTH_INC_

#define DISCORD_CODE_PREFIX "LSDM"

/* Discord verification area settings - Discord doğrulama alanı ayarları */

#define DISCORD_INT            3
#define DISCORD_POS_X          1527.04
#define DISCORD_POS_Y          -12.02
#define DISCORD_POS_Z          1002.09

#define DISCORD_CAM_X          1526.668457
#define DISCORD_CAM_Y          -10.096300
#define DISCORD_CAM_Z          1002.318481

#define DISCORD_LOOK_X         1526.356
#define DISCORD_LOOK_Y         -9.150
#define DISCORD_LOOK_Z         1002.235

/* Forward's - Forwardlar */

forward DiscordDogrulamaTimer(playerid);
forward DiscordDogrulamaKontrol(playerid);

/* fonksiyonlar - functions */
stock DiscordDogrulamaOrtamiHazirla(playerid)
{
    TogglePlayerControllable(playerid, 0);

    SetPlayerInterior(playerid, DISCORD_INT);
    SetPlayerPos(playerid,
        DISCORD_POS_X,
        DISCORD_POS_Y,
        DISCORD_POS_Z
    );

    SetPlayerCameraPos(playerid,
        DISCORD_CAM_X,
        DISCORD_CAM_Y,
        DISCORD_CAM_Z
    );
    SetPlayerCameraLookAt(playerid,
        DISCORD_LOOK_X,
        DISCORD_LOOK_Y,
        DISCORD_LOOK_Z
    );

    SetPlayerVirtualWorld(playerid, playerid + 10);

    return 1;
}

stock DiscordDogrulamaBaslat(playerid)
{
    if(!IsPlayerConnected(playerid)) return 0;

    PlayerData[playerid][pState] = PLAYER_STATE_DISCORD;

    format(pDogrulamaKod[playerid], 30,
    "%s%04d", DISCORD_CODE_PREFIX, random(10000));

    printf("[Discord] %s için doğrulama kodu üretildi: %s",
        ReturnName(playerid), pDogrulamaKod[playerid]);

    new query[256];
    format(query, sizeof(query),
        "UPDATE `kullanicilar` SET `DogrulamaKodu`='%s' WHERE `Isim`='%s'",
        pDogrulamaKod[playerid], ReturnName(playerid)
    );
    mysql_tquery(mysqlC, query);

    SendClientMessage(playerid, -1,"Oyun hesabınızın Discord ile bağdaştırılması gerekmektedir.");

    new msg[128];
    format(msg, sizeof(msg),
        "Size özel doğrulama kodu: {E6D8A8}%s", pDogrulamaKod[playerid]);
    SendClientMessage(playerid, -1, msg);

    SendClientMessage(playerid, -1,
        "Kodu {E6D8A8}#bot-kanal {FFFFFF}üzerinden gönderin.");

    DiscordDogrulamaOrtamiHazirla(playerid);

    PlayerData[playerid][pDiscordTimer] =
        SetTimerEx("DiscordDogrulamaTimer", 1000, true, "i", playerid);

    return 1;
}

/* Timer - Zamanlayıcı */
public DiscordDogrulamaTimer(playerid)
{
    if(!IsPlayerConnected(playerid)) return 0;
    if(PlayerData[playerid][pState] != PLAYER_STATE_DISCORD) return 0;

    new query[128];
    format(query, sizeof(query),
        "SELECT `DiscordID` FROM `kullanicilar` WHERE `Isim`='%s'",
        ReturnName(playerid)
    );

    printf("[Discord] %s doğrulama kontrolü yapılıyor", ReturnName(playerid));

    mysql_tquery(mysqlC, query, "DiscordDogrulamaKontrol", "i", playerid);
    return 1;
}

/* SQL Callback - SQL Çağrıları*/

public DiscordDogrulamaKontrol(playerid)
{
    new rows;
    cache_get_row_count(rows);
    if(!rows) return 1;
    if(!IsPlayerConnected(playerid)) return 0;
    if(PlayerData[playerid][pState] != PLAYER_STATE_DISCORD) return 0;

    cache_get_value_name(0, "DiscordID", PlayerData[playerid][pDiscordID]);

    if(strlen(PlayerData[playerid][pDiscordID]) == 18)
    {
        printf("[Discord] %s doğrulandı. DiscordID: %s",
            ReturnName(playerid), PlayerData[playerid][pDiscordID]);

        DiscordDogrulamaBitir(playerid);
    }
    return 1;
}

/* Ending Verification - Doğrulamayı Sonlandırma */

stock DiscordDogrulamaBitir(playerid)
{
    if(PlayerData[playerid][pDiscordTimer])
    {
        KillTimer(PlayerData[playerid][pDiscordTimer]);
        PlayerData[playerid][pDiscordTimer] = 0;
    }

    SendClientMessage(playerid, -1,
        "Discord doğrulaması tamamlandı. Sunucuya tekrar bağlanın.");

    printf("[Discord] %s doğrulama tamamlandı, kickleniyor",
        ReturnName(playerid));

    SetTimerEx("KickTimer", 1000, false, "i", playerid);
    return 1;
}
