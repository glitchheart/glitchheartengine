#ifndef GMAP_H
#define GMAP_H

#define INIT_SIZE 1024

void* allocate_memory(size_t size);
void deallocate_memory(void* memory);

enum MapStatus
{
    NOT_FOUND,
    EXISTS,
    COLLISION
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
using hash_function_ ## NAME = u64(*)(u64 Size, KEYTYPE key);\
\
struct hashed_pair_## NAME\
{\
    KEYTYPE key;\
    u64 hashed_key;\
    TYPE Val;\
};\
\
struct NAME ## _map;\
\
MapStatus Scan(NAME ## _map* map, u64 hashed_key, KEYTYPE key);\
\
void Rehash(NAME ## _map* map);\
\
struct NAME ## _map \
{ \
    hashed_pair_ ## NAME* hashed_pairs;\
    hashed_pair_ ## NAME* scan_pairs;\
    i32 key_count;\
    i32 Count; \
    b32 Initialized = false;\
    hash_function_ ## NAME Hash; \
    \
    TYPE& operator[](KEYTYPE key) \
    { \
        auto HashV = this->Hash((u64)this->Count, key);\
        auto Res = Scan(this,HashV,key);\
        switch(Res)\
        {\
            case NOT_FOUND:\
            {\
                ASSIGN(this->hashed_pairs[HashV].key,key);\
                this->hashed_pairs[HashV].hashed_key = HashV;\
                ASSIGN(this->scan_pairs[key_count].key,key);\
                this->scan_pairs[this->key_count].hashed_key = HashV;\
                this->key_count++;\
            }\
            break;\
            case EXISTS:\
            {\
            }\
            break;\
            case COLLISION:\
            {\
                Rehash(this);\
                auto new_hash = this->Hash((u64)this->Count, key);\
                ASSIGN(this->hashed_pairs[new_hash].key,key);\
                this->hashed_pairs[new_hash].hashed_key = new_hash;\
                ASSIGN(this->scan_pairs[key_count].key,key);\
                this->scan_pairs[this->key_count].hashed_key = new_hash;\
                this->key_count++;\
                return this->hashed_pairs[new_hash].Val;\
            }\
            break;\
        }\
        return this->hashed_pairs[HashV].Val;\
    } \
    \
}; \
void NAME ##_map_init(NAME ## _map* map, hash_function_ ## NAME Hash, i32 InitSize = INIT_SIZE) \
{ \
    if(map->hashed_pairs)\
    {\
        deallocate_memory(map->hashed_pairs);\
    }\
    if(map->scan_pairs)\
    {\
        deallocate_memory(map->scan_pairs);\
    }\
    map->hashed_pairs = (hashed_pair_ ## NAME*)allocate_memory(InitSize * sizeof(hashed_pair_ ## NAME));\
    map->scan_pairs = (hashed_pair_ ## NAME*)allocate_memory(1024 * sizeof(hashed_pair_ ## NAME));\
    map->key_count = 0;\
    map->Count = InitSize; \
    map->Hash = Hash; \
    map->Initialized = true;\
}\
MapStatus Scan(NAME ## _map* map, u64 hashed_key, KEYTYPE key)\
{\
    for(i32 I = 0; I < map->key_count; I++)\
    {\
        auto pair = map->scan_pairs[I];\
        assert(COMPARE(INVALID,pair.key) != 0);\
        if(COMPARE(INVALID,pair.key) != 0 && pair.hashed_key == hashed_key && COMPARE(pair.key, key) != 0)\
        {\
            /* COLLISION! */\
            return COLLISION;\
        }\
        else if(pair.hashed_key == hashed_key && COMPARE(pair.key,key) == 0)\
        {\
            return EXISTS;\
        }\
    }\
    return NOT_FOUND;\
}\
\
void Rehash(NAME ## _map* map)\
{\
    for(i32 I = 0; I < map->key_count; I++)\
    {\
        auto pair = map->scan_pairs[I];\
        /* If the following does not hold, then we need to add the value anyway, since this is the collision!*/\
        if(COMPARE(pair.key, map->hashed_pairs[pair.hashed_key].key) == 0)\
        {\
            COPY(map->scan_pairs[I].Val, map->hashed_pairs[pair.hashed_key].Val, TYPE);\
        }\
    }\
    map->Count = map->Count * 2;\
    deallocate_memory(map->hashed_pairs);\
    map->hashed_pairs = (hashed_pair_ ## NAME*)allocate_memory(map->Count * sizeof(hashed_pair_ ## NAME));\
    for(i32 I = 0; I < map->key_count; I++)\
    {\
        assert(COMPARE(INVALID,map->scan_pairs[I].key) != 0);\
        auto new_hash = map->Hash((u64)map->Count, map->scan_pairs[I].key);\
        COPY(map->hashed_pairs[new_hash].Val, map->scan_pairs[I].Val, TYPE);\
        ASSIGN(map->hashed_pairs[new_hash].key,map->scan_pairs[I].key);\
        map->hashed_pairs[new_hash].hashed_key = new_hash;\
        map->scan_pairs[I].hashed_key = new_hash;\
    }\
}\
\

i32 CmpInt(i32 i1, i32 i2)
{
    if(i1 < i2)
        return -1;
    if(i1 > i2)
        return 1;
    return 0;
}

i32 StrCmp(char* l, char* r)
{
    if(l && r)
    {
        return strcmp(l,r);
    }
    else
    {
        return -2;
    }
}

#define STR_ASSIGN(Dst,Src) Dst = (char*)allocate_memory(sizeof(char) * (strlen(Src) + 1)); \
strcpy(Dst,Src)
#define INT_ASSIGN(Dst,Src) Dst = Src
#define VAL_COPY(Dst, Src, TYPE) Dst = Src

#define PTR_COPY(Dst, Src, TYPE) if(Dst) { *Dst = *Src; } else {Dst = (TYPE)allocate_memory(sizeof(TYPE)); *Dst = *Src;}

GENERIC_MAP(integer, i32, i32, CmpInt, -1, "%d", INT_ASSIGN, VAL_COPY)

#endif
