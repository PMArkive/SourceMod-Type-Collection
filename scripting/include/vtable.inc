//////////////////////////////////////////////////////////////////////////////
// MADE BY NOTNHEAVY. USES GPL-3, AS PER REQUEST OF SOURCEMOD               //
//////////////////////////////////////////////////////////////////////////////

// god

#if defined NOTNHEAVY_SOURCEMOD_TYPE_COLLECTION_VTABLE
#endinput
#endif
#define NOTNHEAVY_SOURCEMOD_TYPE_COLLECTION_VTABLE

#pragma semicolon true 
#pragma newdecls required

#if !defined SIZE_T
#define SIZE_T        int
#define SIZEOF_SIZE_T 4
#endif

#include "Pointer.inc"

//////////////////////////////////////////////////////////////////////////////
// GLOBALS                                                                  //
//////////////////////////////////////////////////////////////////////////////

enum
{
    VTABLE_INFO_OFFSET_NAME = 0,                                            // char Name[32];
    VTABLE_INFO_OFFSET_VTABLE = VTABLE_INFO_OFFSET_NAME + SIZEOF_CHAR * 32, // void* VTable;
    VTABLE_INFO_OFFSET_SIZE = VTABLE_INFO_OFFSET_VTABLE + SIZEOF_Pointer,   // size_t Size;

    SIZEOF_VTable_Info = VTABLE_INFO_OFFSET_SIZE + SIZEOF_SIZE_T            // sizeof(VTable_Info);
}

static Pointer vtables = NULL;
static SIZE_T vtablesLength = 0;

//////////////////////////////////////////////////////////////////////////////
// VTABLE_INFO METHODMAP                                                    //
//////////////////////////////////////////////////////////////////////////////

methodmap VTable_Info < Pointer
{
    // Constructor - wrap around an existing address to make way for a VTable_Info object.
    public VTable_Info(Address address)
    {
        return view_as<VTable_Info>(address);
    }

    // The name of the vtable info instance.
    property Pointer Name
    {
        public get() { return view_as<Pointer>(this.Address + VTABLE_INFO_OFFSET_NAME); }
    }

    // The pointer to the vtable.
    property Pointer VTable
    {
        public get() { return this.Dereference(VTABLE_INFO_OFFSET_VTABLE); }
        public set(Pointer value) { this.Write(value, VTABLE_INFO_OFFSET_VTABLE); }
    }

    // The length of the vtable.
    property SIZE_T Size
    {
        public get() { return this.Dereference(VTABLE_INFO_OFFSET_SIZE); }
        public set(SIZE_T value) { this.Write(value, VTABLE_INFO_OFFSET_SIZE); }
    }

    // Call constructor on existing type.
    public void ctor(char[] name, Pointer vtable, SIZE_T size)
    {
        memcpy(this.Name, AddressOfString(name), 32);
        this.Name.Write('\0', 31, NumberType_Int8);
        this.VTable = vtable;
        this.Size = size;
    }
}

//////////////////////////////////////////////////////////////////////////////
// VTABLE FUNCTIONALITY                                                     //
//////////////////////////////////////////////////////////////////////////////

static VTable_Info GetVTableInfo(char[] name)
{
    for (int i = 0; i < vtablesLength / SIZEOF_VTable_Info; ++i)
    {
        VTable_Info vtableInfo = view_as<VTable_Info>(vtables.Get(i, SIZEOF_VTable_Info));
        char buffer[32];
        PointerToCharBuffer(vtableInfo.Name, buffer, sizeof(buffer));
        if (StrEqual(name, buffer))
            return vtableInfo;
    }
    return view_as<VTable_Info>(NULL);
}

methodmap VTable
{
    // The name must be up to 32 characters, including the NUL character. The length is the number of vpointers to be used.
    public static Pointer CreateVTable(char[] name, SIZE_T length)
    {
        if (vtables == NULL)
        {
            vtables = malloc(SIZEOF_VTable_Info);
            vtablesLength = SIZEOF_VTable_Info;
        }
        else
        {
            vtablesLength += SIZEOF_VTable_Info;
            vtables = realloc(vtables, vtablesLength);
        }

        VTable_Info vtableInfo = view_as<VTable_Info>(vtables.Get(vtablesLength - SIZEOF_VTable_Info, 1));
        Pointer vtable = malloc(length * SIZEOF_Pointer);
        vtableInfo.ctor(name, vtable, length);
        return vtable;
    }

    // Get a vtable from the registered name.
    public static Pointer GetVTable(char[] name)
    {
        VTable_Info info = GetVTableInfo(name);
        return info != NULL ? info.VTable : NULL;
    }

    // Register a new vpointer.
    public static Pointer RegisterVPointer(char[] name, char[] asm, int index)
    {
        VTable_Info info = GetVTableInfo(name);
        if (info == NULL || index < 0 || index >= info.Size)
            return NULL;
        int length = strlen(asm) + 1;
        Pointer vpointer = malloc(length);
        memcpy(vpointer, AddressOfString(asm), length);
        info.VTable.Write(vpointer, index * SIZEOF_Pointer);
        return vpointer;
    }

    // Hook the vtable onto an existing object.
    public static void HookOntoObject(char[] name, Pointer obj)
    {
        VTable_Info info = GetVTableInfo(name);
        if (info == NULL)
            return;
        obj.Write(info.VTable);
    }

    // This should be called on plugin end.
    public static void ClearVTables()
    {
        for (int i = 0; i < vtablesLength / SIZEOF_VTable_Info; ++i)
        {
            VTable_Info vtableInfo = view_as<VTable_Info>(vtables.Get(i, SIZEOF_VTable_Info));
            for (int v = 0; v < vtableInfo.Size; ++v)
            {
                Pointer vpointer = vtableInfo.VTable.Dereference(v * SIZEOF_Pointer);
                if (vpointer != NULL)
                    free(vpointer);
            }
            free(vtableInfo.VTable);
        }
        free(vtables);
        vtables = NULL;
    }
}