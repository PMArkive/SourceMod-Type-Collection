//////////////////////////////////////////////////////////////////////////////
// MADE BY NOTNHEAVY. USES GPL-3, AS PER REQUEST OF SOURCEMOD               //
//////////////////////////////////////////////////////////////////////////////

#if defined NOTNHEAVY_SOURCEMOD_TYPE_COLLECTION_CTAKEDAMAGEINFO
#endinput
#endif
#define NOTNHEAVY_SOURCEMOD_TYPE_COLLECTION_CTAKEDAMAGEINFO

#include "Vector.inc"

#pragma semicolon true 
#pragma newdecls required

#define BASEDAMAGE_NOT_SPECIFIED FLT_MAX

//////////////////////////////////////////////////////////////////////////////
// CTAKEDAMAGEINFO DATA                                                     //
//////////////////////////////////////////////////////////////////////////////

enum
{
    CTAKEDAMAGEINFO_OFFSET_M_VECDAMAGEFORCE = 0,                                                               // Vector m_vecDamageForce;
    CTAKEDAMAGEINFO_OFFSET_M_VECDAMAGEPOSITION = CTAKEDAMAGEINFO_OFFSET_M_VECDAMAGEFORCE + SIZEOF_Vector,      // Vector m_vecDamagePosition;
    CTAKEDAMAGEINFO_OFFSET_M_VECREPORTEDPOSITION = CTAKEDAMAGEINFO_OFFSET_M_VECDAMAGEPOSITION + SIZEOF_Vector, // Position players are told damage is coming from (Vector m_vecReportedPosition;)
    CTAKEDAMAGEINFO_OFFSET_M_HINFLICTOR = CTAKEDAMAGEINFO_OFFSET_M_VECREPORTEDPOSITION + SIZEOF_Vector,        // EHANDLE m_hInflictor;
    CTAKEDAMAGEINFO_OFFSET_M_HATTACKER = CTAKEDAMAGEINFO_OFFSET_M_HINFLICTOR + 4,                              // EHANDLE m_hAttacker;
    CTAKEDAMAGEINFO_OFFSET_M_HWEAPON = CTAKEDAMAGEINFO_OFFSET_M_HATTACKER + 4,                                 // EHANDLE m_hWeapon;
    CTAKEDAMAGEINFO_OFFSET_M_FLDAMAGE = CTAKEDAMAGEINFO_OFFSET_M_HWEAPON + 4,                                  // float m_flDamage;
    CTAKEDAMAGEINFO_OFFSET_M_FLMAXDAMAGE = CTAKEDAMAGEINFO_OFFSET_M_FLDAMAGE + 4,                              // float m_flMaxDamage;
    CTAKEDAMAGEINFO_OFFSET_M_FLBASEDAMAGE = CTAKEDAMAGEINFO_OFFSET_M_FLMAXDAMAGE + 4,                          // The damage amount before skill leve adjustments are made. Used to get uniform damage forces. (float m_flBaseDamage;)
    CTAKEDAMAGEINFO_OFFSET_M_BITSDAMAGETYPE = CTAKEDAMAGEINFO_OFFSET_M_FLBASEDAMAGE + 4,                       // int m_bitsDamageType;
    CTAKEDAMAGEINFO_OFFSET_M_IDAMAGECUSTOM = CTAKEDAMAGEINFO_OFFSET_M_BITSDAMAGETYPE + 4,                      // int m_iDamageCustom;
    CTAKEDAMAGEINFO_OFFSET_M_IDAMAGESTATS = CTAKEDAMAGEINFO_OFFSET_M_IDAMAGECUSTOM + 4,                        // int m_iDamageStats;
    CTAKEDAMAGEINFO_OFFSET_M_IAMMOTYPE = CTAKEDAMAGEINFO_OFFSET_M_IDAMAGESTATS + 4,                            // AmmoType of the weapon used to cause this damage, if any (int m_iAmmoType;)
    CTAKEDAMAGEINFO_OFFSET_M_IDAMAGEDOTHERPLAYERS = CTAKEDAMAGEINFO_OFFSET_M_IAMMOTYPE + 4,                    // int m_iDamagedOtherPlayers;
    CTAKEDAMAGEINFO_OFFSET_M_IPLAYERPENETRATIONCOUNT = CTAKEDAMAGEINFO_OFFSET_M_IDAMAGEDOTHERPLAYERS + 4,      // int m_iPlayerPenetrationCount;
    CTAKEDAMAGEINFO_OFFSET_M_FLDAMAGEBONUS = CTAKEDAMAGEINFO_OFFSET_M_IPLAYERPENETRATIONCOUNT + 4,             // Anything that increases damage (crit) - store the delta (float m_flDamageBonus;)
    CTAKEDAMAGEINFO_OFFSET_M_HDAMAGEBONUSPROVIDER = CTAKEDAMAGEINFO_OFFSET_M_FLDAMAGEBONUS + 4,                // Who gave us the ability to do extra damage? (EHANDLE m_hDamageBonusProvider;)
    CTAKEDAMAGEINFO_OFFSET_M_BFORCEFRIENDLYFIRE = CTAKEDAMAGEINFO_OFFSET_M_HDAMAGEBONUSPROVIDER + 4,           // Ideally this would be a dmg type, but we can't add more (bool m_bForceFriendlyFire;)

    CTAKEDAMAGEINFO_M_FLDAMAGEFORFORCE = CTAKEDAMAGEINFO_OFFSET_M_BFORCEFRIENDLYFIRE + 4,                      // float m_flDamageForForce;

    CTAKEDAMAGEINFO_M_ECRITTYPE = CTAKEDAMAGEINFO_M_FLDAMAGEFORFORCE + 4,                                      // ECritType m_eCritType;

    SIZEOF_CTakeDamageInfo = CTAKEDAMAGEINFO_M_ECRITTYPE + 4                                                   // sizeof(CTakeDamageInfo);
};

enum ECritType
{
    CRIT_NONE = 0,
    CRIT_MINI,
    CRIT_FULL,
};

static stock int SMTC_PlayerTakingMiniCrits = 0;

//////////////////////////////////////////////////////////////////////////////
// SMTC_CTakeDamageInfoForward()                                            //
//////////////////////////////////////////////////////////////////////////////

public void SMTC_CTakeDamageInfoForward()
{
    HookEvent("player_hurt", SMTC_ClientHurt, EventHookMode_Pre);
    DHookEnableDetour(SMTC_DHooks_CTFPlayer_OnTakeDamage_Alive, false, SMTC_CTFPlayer_OnTakeDamage_Alive);
}

//////////////////////////////////////////////////////////////////////////////
// CTAKEDAMAGEINFO METHODMAP                                                //
//////////////////////////////////////////////////////////////////////////////

// Holds all details for damaging an entity.
methodmap CTakeDamageInfo < Pointer
{
    // Constructor - wrap around an existing address to make way for a CTakeDamageInfo object.
    public CTakeDamageInfo(Address address)
    {
        return view_as<CTakeDamageInfo>(address);
    }

    // The damage force.
    property Vector m_vecDamageForce
    {
        public get() { return view_as<Vector>(this.Address + view_as<int>(CTAKEDAMAGEINFO_OFFSET_M_VECDAMAGEFORCE)); }
        public set(Vector value) { memcpy(this.Address + view_as<int>(CTAKEDAMAGEINFO_OFFSET_M_VECDAMAGEFORCE), value, SIZEOF_Vector); }
    }

    // The damage position.
    property Vector m_vecDamagePosition
    {
        public get() { return view_as<Vector>(this.Address + view_as<int>(CTAKEDAMAGEINFO_OFFSET_M_VECDAMAGEPOSITION)); }
        public set(Vector value) { memcpy(this.Address + view_as<int>(CTAKEDAMAGEINFO_OFFSET_M_VECDAMAGEPOSITION), value, SIZEOF_Vector); }
    }

    // The reported damage position.
    property Vector m_vecReportedPosition
    {
        public get() { return view_as<Vector>(this.Address + view_as<int>(CTAKEDAMAGEINFO_OFFSET_M_VECREPORTEDPOSITION)); }
        public set(Vector value) { memcpy(this.Address + view_as<int>(CTAKEDAMAGEINFO_OFFSET_M_VECREPORTEDPOSITION), value, SIZEOF_Vector); }
    }

    // The inflictor. This can be any entity.
    property int m_hInflictor
    {
        public get() { return this.DereferenceEHandle(CTAKEDAMAGEINFO_OFFSET_M_HINFLICTOR); }
        public set(int value) { this.WriteEHandle(value, CTAKEDAMAGEINFO_OFFSET_M_HINFLICTOR); }
    }

    // The attacker. If set, this is a CTFPlayer entity.
    property int m_hAttacker
    {
        public get() { return this.DereferenceEHandle(CTAKEDAMAGEINFO_OFFSET_M_HATTACKER); }
        public set(int value) { this.WriteEHandle(value, CTAKEDAMAGEINFO_OFFSET_M_HATTACKER); }
    }

    // The weapon. If set, this is a CTFWeaponBase entity.
    property int m_hWeapon
    {
        public get() { return this.DereferenceEHandle(CTAKEDAMAGEINFO_OFFSET_M_HWEAPON); }
        public set(int value) { this.WriteEHandle(value, CTAKEDAMAGEINFO_OFFSET_M_HWEAPON); }
    }

    // The amount of damage to deal.
    property float m_flDamage
    {
        public get() { return this.Dereference(CTAKEDAMAGEINFO_OFFSET_M_FLDAMAGE); }
        public set(float value) { this.Write(value, CTAKEDAMAGEINFO_OFFSET_M_FLDAMAGE); }
    }

    // The maximum amount of damage that can be dealt.
    property float m_flMaxDamage
    {
        public get() { return this.Dereference(CTAKEDAMAGEINFO_OFFSET_M_FLMAXDAMAGE); }
        public set(float value) { this.Write(value, CTAKEDAMAGEINFO_OFFSET_M_FLMAXDAMAGE); }
    }

    // The minimum amount of damage to deal.
    property float m_flBaseDamage
    {
        public get() { return this.Dereference(CTAKEDAMAGEINFO_OFFSET_M_FLBASEDAMAGE); }
        public set(float value) { this.Write(value, CTAKEDAMAGEINFO_OFFSET_M_FLBASEDAMAGE); }
    }

    // An integral value representing all damage type flags.
    property int m_bitsDamageType
    {
        public get() { return this.Dereference(CTAKEDAMAGEINFO_OFFSET_M_BITSDAMAGETYPE); }
        public set(int value) { this.Write(value, CTAKEDAMAGEINFO_OFFSET_M_BITSDAMAGETYPE); }
    }

    // The custom damage type.
    property int m_iDamageCustom
    {
        public get() { return this.Dereference(CTAKEDAMAGEINFO_OFFSET_M_IDAMAGECUSTOM); }
        public set(int value) { this.Write(value, CTAKEDAMAGEINFO_OFFSET_M_IDAMAGECUSTOM); }
    }

    // The damage stats.
    property int m_iDamageStats
    {
        public get() { return this.Dereference(CTAKEDAMAGEINFO_OFFSET_M_IDAMAGESTATS); }
        public set(int value) { this.Write(value, CTAKEDAMAGEINFO_OFFSET_M_IDAMAGESTATS); }
    }
    
    // The ammo type used with the weapon.
    property int m_iAmmoType
    {
        public get() { return this.Dereference(CTAKEDAMAGEINFO_OFFSET_M_IAMMOTYPE); }
        public set(int value) { this.Write(value, CTAKEDAMAGEINFO_OFFSET_M_IAMMOTYPE); }
    }

    // Have we damaged any other players?
    property int m_iDamagedOtherPlayers
    {
        public get() { return this.Dereference(CTAKEDAMAGEINFO_OFFSET_M_IDAMAGEDOTHERPLAYERS); }
        public set(int value) { this.Write(value, CTAKEDAMAGEINFO_OFFSET_M_IDAMAGEDOTHERPLAYERS); }
    }

    // Have we penetrated through any other players?
    property int m_iPlayerPenetrationCount
    {
        public get() { return this.Dereference(CTAKEDAMAGEINFO_OFFSET_M_IPLAYERPENETRATIONCOUNT); }
        public set(int value) { this.Write(value, CTAKEDAMAGEINFO_OFFSET_M_IPLAYERPENETRATIONCOUNT); }
    }

    // Anything that increases damage.
    property float m_flDamageBonus
    {
        public get() { return this.Dereference(CTAKEDAMAGEINFO_OFFSET_M_FLDAMAGEBONUS); }
        public set(float value) { this.Write(value, CTAKEDAMAGEINFO_OFFSET_M_FLDAMAGEBONUS); }
    }

    // Who gave us the ability to do extra damage?
    property int m_hDamageBonusProvider
    {
        public get() { return this.DereferenceEHandle(CTAKEDAMAGEINFO_OFFSET_M_HDAMAGEBONUSPROVIDER); }
        public set(int value) { this.WriteEHandle(value, CTAKEDAMAGEINFO_OFFSET_M_HDAMAGEBONUSPROVIDER); }
    }

    // Deal damage to own teammates. Would be a damage type but there's a limited amount possible.
    property int m_bForceFriendlyFire
    {
        public get() { return this.Dereference(CTAKEDAMAGEINFO_OFFSET_M_BFORCEFRIENDLYFIRE); }
        public set(int value) { this.Write(value, CTAKEDAMAGEINFO_OFFSET_M_BFORCEFRIENDLYFIRE, NumberType_Int8); }
    }

    // Damage for knockback force.
    property float m_flDamageForForce
    {
        public get() { return this.Dereference(CTAKEDAMAGEINFO_M_FLDAMAGEFORFORCE); }
        public set(float value) { this.Write(value, CTAKEDAMAGEINFO_M_FLDAMAGEFORFORCE); }
    }

    // The crit type.
    property ECritType m_eCritType
    {
        public get() { return this.Dereference(CTAKEDAMAGEINFO_M_ECRITTYPE); }
        public set(ECritType value) { this.Write(value, CTAKEDAMAGEINFO_M_ECRITTYPE); }
    }
    
    // Set up the CTakeDamageInfo object.
    public void Init(int inflictor, int attacker, int weapon, const Vector damageForce, const Vector damagePosition, const Vector reportedPosition, float damage, int damageType, int customDamage)
    {
        this.m_hInflictor = inflictor;
        if (attacker)
            this.m_hAttacker = attacker;
        else
            this.m_hAttacker = inflictor;

        this.m_hWeapon = weapon;

        this.m_flDamage = damage;

        this.m_flBaseDamage = BASEDAMAGE_NOT_SPECIFIED;

        this.m_bitsDamageType = damageType;
        this.m_iDamageCustom = customDamage;

        this.m_flMaxDamage = damage;
        this.m_vecDamageForce = damageForce;
        this.m_vecDamagePosition = damagePosition;
        this.m_vecReportedPosition = reportedPosition;
        this.m_iAmmoType = -1;
        this.m_iDamagedOtherPlayers = 0;
        this.m_iPlayerPenetrationCount = 0;
        this.m_flDamageBonus = 0.00;
        this.m_bForceFriendlyFire = false;
        this.m_flDamageForForce = 0.00;
        this.m_eCritType = CRIT_NONE;
    }

    // Call constructor on existing type.
    public void ctor(int inflictor, int attacker, int weapon, const Vector damageForce, const Vector damagePosition, float damage, int bitsDamageType, int killType, Vector reportedPosition = NULL)
    {
        Vector vecReported = vec3_origin;
        if (reportedPosition != NULL)
            vecReported = reportedPosition;
        this.Init(inflictor, attacker, weapon, damageForce, damagePosition, vecReported, damage, bitsDamageType, killType);
    }

    // Constructor - allocate a new CTakeDamageInfo onto the heap. Deallocation will be required when you are finished using free().
    public static CTakeDamageInfo Malloc(int inflictor, int attacker, int weapon, const Vector damageForce, const Vector damagePosition, float damage, int bitsDamageType, int killType, Vector reportedPosition = NULL)
    {
        CTakeDamageInfo pointer = malloc(SIZEOF_CTakeDamageInfo);
        pointer.ctor(inflictor, attacker, weapon, damageForce, damagePosition, damage, bitsDamageType, killType, reportedPosition);
        return pointer;
    }

    // Constructor - allocate a new CTakeDamageInfo onto the stack. When using the return value, you must use STACK_GETRETURN().
    public static STACK StackAlloc(int inflictor, int attacker, int weapon, const Vector damageForce, const Vector damagePosition, float damage, int bitsDamageType, int killType, Vector reportedPosition = NULL)
    {
        char buffer[SIZEOF_CTakeDamageInfo];
        view_as<CTakeDamageInfo>(AddressOfString(buffer)).ctor(inflictor, attacker, weapon, damageForce, damagePosition, damage, bitsDamageType, killType, reportedPosition);
        return buffer;
    }

    // Set the crit type. The victim parameter will not be utilised if SMTC.inc is not included.
    public void SetCritType(ECritType type, int victim = 0)
    {
        if (type == CRIT_NONE) // always let CRIT_NONE override the current setting
            this.m_eCritType = type;
        else // don't let CRIT_MINI override CRIT_FULL
            this.m_eCritType = type > this.m_eCritType ? type : this.m_eCritType;
        
        // For fixing the display bug with mini-crits.
        if (victim && this.m_eCritType == CRIT_MINI)
            SMTC_PlayerTakingMiniCrits = victim;
    }
}

//////////////////////////////////////////////////////////////////////////////
// MISCELLANEOUS                                                            //
//////////////////////////////////////////////////////////////////////////////

// This fixes the bug where mini-crits toggled manually show the full crit effect instead. 
// This is due to DMG_CRIT being toggled when using either mini-crits or crits, and DMG_CRIT
// defaulting to the full crit effect.
static Action SMTC_ClientHurt(Event event, const char[] name, bool dontBroadcast)
{
    int client = GetClientOfUserId(event.GetInt("userid"));
    if (SMTC_PlayerTakingMiniCrits == client) 
        event.SetInt("bonuseffect", 1); // CRIT_MINI
    SMTC_PlayerTakingMiniCrits = 0;
    return Plugin_Changed;
}

// Pre-call CTFPlayer::OnTakeDamage_Alive()
// If the crit type is not CRIT_MINI, reset the SMTC_PlayerTakingMiniCrits counter.
static MRESReturn SMTC_CTFPlayer_OnTakeDamage_Alive(int entity, DHookReturn returnValue, DHookParam parameters)
{
    CTakeDamageInfo info = parameters.Get(1);
    if (info.m_eCritType != CRIT_MINI)
        SMTC_PlayerTakingMiniCrits = 0;
    return MRES_Ignored;
}