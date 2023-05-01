# SourceMod Type Collection

This is basically just a dump of random methodmaps that I decided to make over time. ~~Yes, that includes CUtlVector, all of your sins in one methodmap~~ Yes, that includes all sorts of atrocities such as my own small vtable API. Have fun!

Note that this is only tested with TF2. For other games, you'll have to tweak the gamedata for the non-TF2 includes to your likings.

## How to use & dependencies
You can just drag over your desired header files from here (located in ./scripting/include/) and add them to your own project. 

The only third-party depenency currently is [Scags' SM-Memory Extension](https://github.com/Scags/SM-Memory). SMTC contains a modified include of SM-Memory, which replaces some of the C-named natives for their faster variants, whilst retaining the PascalCase natives which set the memory access, and only utilises the bare minimum required to have SMTC to work.

## SMTCHeader.inc and SMTC.inc
These includes have high significance and are required for the vast majority of these includes. Unless specified as not required, you will have to put "SMTCHeader.inc" before all of SMTC's includes, then "SMTC.inc" at the end of the SMTC includes. `SMTC_Initialize()` must then be called in `OnMapStart()`.

## Includes present:
- Vector.inc: A 3D vector. No more of "float buffer[3]" all over the place. There are multiple ways of instantiating vector objects. `Vector.Malloc()` will dynamically allocate the vector and you are responsible for freeing it afterwards - only use this for long-lasting vectors. `Vector.StackAlloc()` must be used with `STACK_GETRETURN()` and allocates a vector onto the stack frame, using arrays behind the scenes. Use this similarly to how you declare any typical variables, however the code for this will be slightly more ugly. `Vector.Accumulator()` should only be used in functions which return a temporary vector object. `Vector.Cache()` should be used for working with multiple temporary vector objects at once, such as with arithmetic operations.
- QAngle.inc: A 3D vector, but used for pitch/yaw/roll. There are multiple ways of instantiating QAngle objects. `QAngle.Malloc()` will dynamically allocate the QAngle and you are responsible for freeing it afterwards - only use this for long-lasting QAngles. `QAngle.StackAlloc()` must be used with `STACK_GETRETURN()` and allocates a QAngle onto the stack frame, using arrays behind the scenes. Use this similarly to how you declare any typical variables, however the code for this will be slightly more ugly. `QAngle.Accumulator()` should only be used in functions which return a temporary QAngle object. `QAngle.Cache()` should be used for working with multiple temporary QAngle objects at once, such as with arithmetic operations. This requires "Vector.inc".
- CUtlVector.inc: A dynamically expanding array responsible for all acts of terrorism. You're probably better off using SourceMod's ArrayList instead, but use this if you want. This will be necessary for any CUtlVectors used internally however, such as `CBasePlayer::m_hMyWearables`. Note, because this repository relies on Scags' SM-Memory extension, this will collide with the methodmap found in SM-Memory's vec.inc. You'll have to modify your smmem.inc to not include its own vec.inc. Unlike Scags' implementation, this one is made to have full functionality and also has its own CUtlMemory methodmap, much like how the internal CUtlVector class has its own CUtlMemory class. This requires "CUtlMemory.inc".
- CUtlMemory.inc: This is a dynamically expanding buffer that is pretty much the barebones of CUtlVector.
- cplane_t.inc: A surface plane structure, primarily found in CBaseTrace. This requires "Vector.inc".
- csurface_t.inc: A structure containing information of a surface, primarily found in CGameTrace. This requires "Vector.inc" and "cplane_t.inc".
- CBaseTrace.inc: The base object used for tracing information. This requires "Vector.inc" and "cplane_t.inc".
- CGameTrace.inc: typedef'd as "trace_t" internally, this expands upon CBaseTrace and is typically used in trace operations. This requires "Vector.inc" and "csurface_t.inc".
- shareddefs.inc (w/o): This just contains a few pieces of information for usage with the other headers. This is not complete, however it is slowly expanding and I figured this would be an appropriate place to include additional code.
- vtable.inc: Yeah this is quite ridiculous and requires knowledge of vtables. If you are working with objects that require vtable modification, this include allows for that - you can create a record for an object's vtable, create the necessary vpointers, then hook the vtable onto an object. You can call it with SDKCall using `PrepSDKCall_SetVirtual()`... if you even need to. An example use case of this include within this project are trace methodmaps. When using this, call `VTable.ClearVTables()` on plugin end.
- VectorAligned.inc: A vector object that is padded to take up 16 bytes of space, rather than Vector's 12 bytes. Functionally it is the exact same as Vector. See the documentation for "Vector.inc" to see how you can allocate VectorAligned objects as the process is the same. This requires "Vector.inc".
- Ray_t.inc: This is a ray object casted from one point to another, which can also have a specified hull size. This requires "VectorAligned.inc".
- UTIL.inc: This provides a small range of trace filters: ITraceFilter (abstract class), CTraceFilter, CTraceFilterEntitiesOnly, CTraceFilterWorldOnly, CTraceFilterWorldAndPropsOnly, CTraceFilterHitAll and CTraceFilterSimple. CTraceFilterSimple is the recommended filter to use for traces unless you know what you're doing. You can also expand off these trace methodmaps. Alongside this, three tracing functions are also exposed: `UTIL_TraceLine()`, `UTIL_TraceHull()` and `UTIL_TraceRay()`. This requires "Ray_t.inc", "VectorAligned.inc" and "vtable.inc".
- CBaseEntity.inc: A small methodmap that contains a few wrapper functions and properties, intended to mimick the internal class. Further things may be added to this to the future, so far it only contains a property for `m_RefEHandle` and wrapper functions for netprops.
- CHandle.inc: Handles are IDs unique to every entity in the game, with 11 bits reserved for the entity, and the last 21 reserved for a serial number that is unique to the specific entity.
- CEngineTrace.inc: A single object that is meant to be the enginetrace object, providing a few methods for entity tracing.
- string_t.inc: A string object that contains a string pointer. There isn't really much else going on here.
- CGlobalVarsBase.inc: These are some of the baseline game's global variables used across shared code.
- CGlobalVars.inc: This expands upon CGlobalVarsBase, providing more global variables. This also provides the `gpGlobals` object which allows you to access the server's variables. This requires "CGlobalVarsBase.inc".

## TF2 includes present:
- tf/tf_shareddefs.inc (w/o): Similar to shareddefs.inc, however this is TF2 specific. This include is not complete either, however it is slowly expanding.
- tf/CTakeDamageInfo.inc: An object that contains all information for damaging players. Note that you will have to create your own DHooks (in particular, the OnTakeDamage ones) in order to utilise this methodmap fully. Note that there is a bug where if you toggle mini crits manually before `CTFGameRules::ApplyOnDamageModifyRules()` is called, due to mini crits and crits both toggling the DMG_CRIT damage type, the effect will default to the full crit one. This can be fixed by including SMTC.inc and calling `CTakeDamageInfo::SetCritType()` instead of modifying `CTakeDamageInfo::m_eCritType()` directly. This requires "Vector.inc".
- tf/CTFRadiusDamageInfo.inc: An object that allows for damaging players within a specific radius. This requires "CTakeDamageInfo.inc" and "Vector.inc".
- tf/TFPlayerClassData_t.inc: An object that holds data for each class in TF2. This can be used to interact with the game's `g_pTFPlayerClassDataMgr`, which holds data about all of TF2's classes. This requires "tf_shareddefs.inc" and "Vector.inc".
- tf/tf_point_t.inc: An object that represents a point within a CTFPointManager entity. The hook `FORWARDTYPE_ADDPOINT` can be used with `SMTC_HookEntity()` on entites deriving from CTFPointManager. When hooking onto an entity with this forward type, the callback provided will be invoked whenever a new point has been added to this entity's `m_Points`. This requires "Vector.inc".
- tf/flame_point_t.inc: An object that represents a flame point within a CTFFlameManager entity. This derives from tf_point_t and can be worked with to modify flames spewed from flamethrowers. This requires "tf_point_t.inc".
- tf/CTFGameRules.inc: A single object that is meant to be the TF2 game rules object. It only has one method currently: CTFGameRules::RadiusDamage(CTFRadiusDamageInfo& info).

## Removed since previous versions (additions here will be cleared in the update after deprecation):
- Nothing!

## SourceMod version
This library is not fully compatible with SourceMod 1.11 due to the release builds having a bugged operator= overloading implementation, while that works on SourceMod 1.10, SMTC has not been tested with SourceMod 1.10. Either build SourceMod yourself with the most recent commit for SourcePawn, or avoid using overloaded functions for the = operator.