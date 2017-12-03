#ifndef GMAP_H
#define GMAP_H

#define INIT_SIZE 1024

void* AllocateMemory(size_t Size);
void DeallocateMemory(void* Memory);

enum Map_Status
{
    Not_Found,
    Exists,
    Collision
};

/* Generically typed map structure
*  NAME:    The name prefix of the map type
*  TYPE:    The type for the values in the map
*  KEYTYPE: The type of the keys to be used in the map
*  COMPARE: A compare function returning an int (-1,0,1)
*  INVALID: The value that is seen as invalid (used in assertions)
*  FORMAT:  The format identifer for printf
*  ASSIGN:  The assignment macro. Strings need to be alloc'ed as an example
*  COPY:    The copy operator/function for the value type
*/
#define GENERIC_MAP(NAME, TYPE, KEYTYPE, COMPARE, INVALID, FORMAT, ASSIGN, COPY) \
\
using hash_function_ ## NAME = u64(*)(u64 Size, KEYTYPE Key);\
\
struct hashed_pair_## NAME\
{\
    KEYTYPE Key;\
    u64 HashedKey;\
    TYPE Val;\
};\
\
struct NAME ## _map;\
\
Map_Status Scan(NAME ## _map* Map, u64 HashedKey, KEYTYPE Key);\
\
void Rehash(NAME ## _map* Map);\
\
struct NAME ## _map \
{ \
    hashed_pair_ ## NAME* HashedPairs;\
    hashed_pair_ ## NAME* ScanPairs;\
    i32 KeyCount;\
    i32 Count; \
    b32 Initialized = false;\
    hash_function_ ## NAME Hash; \
    \
    TYPE& operator[](KEYTYPE Key) \
    { \
        Assert(this->Initialized);\
        Assert(this->HashedPairs);\
        Assert(this->ScanPairs);\
        Assert(COMPARE(INVALID,Key) != 0);\
        Assert(this->Hash);\
        auto HashV = this->Hash((u64)this->Count, Key);\
        auto Res = Scan(this,HashV,Key);\
        switch(Res)\
        {\
            case Not_Found:\
            {\
                ASSIGN(this->HashedPairs[HashV].Key,Key);\
                this->HashedPairs[HashV].HashedKey = HashV;\
                ASSIGN(this->ScanPairs[KeyCount].Key,Key);\
                this->ScanPairs[this->KeyCount].HashedKey = HashV;\
                this->KeyCount++;\
            }\
            break;\
            case Exists:\
            {\
            }\
            break;\
            case Collision:\
            {\
                Rehash(this);\
                auto NewHash = this->Hash((u64)this->Count, Key);\
                ASSIGN(this->HashedPairs[NewHash].Key,Key);\
                this->HashedPairs[NewHash].HashedKey = NewHash;\
                ASSIGN(this->ScanPairs[KeyCount].Key,Key);\
                this->ScanPairs[this->KeyCount].HashedKey = NewHash;\
                this->KeyCount++;\
                return this->HashedPairs[NewHash].Val;\
            }\
            break;\
        }\
        return this->HashedPairs[HashV].Val;\
    } \
    \
}; \
void NAME ##_Map_Init(NAME ## _map* Map, hash_function_ ## NAME Hash, i32 InitSize = INIT_SIZE) \
{ \
    if(Map->HashedPairs)\
    {\
        DeallocateMemory(Map->HashedPairs);\
    }\
    if(Map->ScanPairs)\
    {\
        DeallocateMemory(Map->ScanPairs);\
    }\
    Map->HashedPairs = (hashed_pair_ ## NAME*)AllocateMemory(InitSize * sizeof(hashed_pair_ ## NAME));\
    Map->ScanPairs = (hashed_pair_ ## NAME*)AllocateMemory(1024 * sizeof(hashed_pair_ ## NAME));\
    Map->KeyCount = 0;\
    Map->Count = InitSize; \
    Map->Hash = Hash; \
    Map->Initialized = true;\
}\
Map_Status Scan(NAME ## _map* Map, u64 HashedKey, KEYTYPE Key)\
{\
    for(i32 I = 0; I < Map->KeyCount; I++)\
    {\
        auto Pair = Map->ScanPairs[I];\
        Assert(COMPARE(INVALID,Pair.Key) != 0);\
        if(COMPARE(INVALID,Pair.Key) != 0 && Pair.HashedKey == HashedKey && COMPARE(Pair.Key, Key) != 0)\
        {\
            /* Collision! */\
            return Collision;\
        }\
        else if(Pair.HashedKey == HashedKey && COMPARE(Pair.Key,Key) == 0)\
        {\
            return Exists;\
        }\
    }\
    return Not_Found;\
}\
\
void Rehash(NAME ## _map* Map)\
{\
    for(i32 I = 0; I < Map->KeyCount; I++)\
    {\
        auto Pair = Map->ScanPairs[I];\
        /* If the following does not hold, then we need to add the value anyway, since this is the collision!*/\
        if(COMPARE(Pair.Key, Map->HashedPairs[Pair.HashedKey].Key) == 0)\
        {\
            COPY(Map->ScanPairs[I].Val, Map->HashedPairs[Pair.HashedKey].Val, TYPE);\
        }\
    }\
    Map->Count = Map->Count * 2;\
    DeallocateMemory(Map->HashedPairs);\
    Map->HashedPairs = (hashed_pair_ ## NAME*)AllocateMemory(Map->Count * sizeof(hashed_pair_ ## NAME));\
    for(i32 I = 0; I < Map->KeyCount; I++)\
    {\
        Assert(COMPARE(INVALID,Map->ScanPairs[I].Key) != 0);\
        auto NewHash = Map->Hash((u64)Map->Count, Map->ScanPairs[I].Key);\
        COPY(Map->HashedPairs[NewHash].Val, Map->ScanPairs[I].Val, TYPE);\
        ASSIGN(Map->HashedPairs[NewHash].Key,Map->ScanPairs[I].Key);\
        Map->HashedPairs[NewHash].HashedKey = NewHash;\
        Map->ScanPairs[I].HashedKey = NewHash;\
    }\
}\
\

i32 CmpInt(i32 I1, i32 I2)
{
    if(I1 < I2)
        return -1;
    if(I1 > I2)
        return 1;
    return 0;
}

i32 StrCmp(char* L, char* R)
{
    if(L && R)
    {
        return strcmp(L,R);
    }
    else
    {
        return -2;
    }
}

#define STR_ASSIGN(Dst,Src) Dst = (char*)AllocateMemory(sizeof(char) * (strlen(Src) + 1)); \
strcpy(Dst,Src)
#define INT_ASSIGN(Dst,Src) Dst = Src
#define VAL_COPY(Dst, Src, TYPE) Dst = Src

#define PTR_COPY(Dst, Src, TYPE) if(Dst) { *Dst = *Src; } else {Dst = (TYPE)AllocateMemory(sizeof(TYPE)); *Dst = *Src;}

GENERIC_MAP(integer, i32, i32, CmpInt, -1, "%d", INT_ASSIGN, VAL_COPY)

#endif
