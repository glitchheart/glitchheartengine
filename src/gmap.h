#ifndef GMAP_H
#define GMAP_H

#define INIT_SIZE 1024

enum Map_Status
{
    Not_Found,
    Exists,
    Collision
};

#define GENERIC_MAP(NAME, TYPE, KEYTYPE) \
using hash_function_ ## NAME = u64(*)(u64 Size, KEYTYPE Key);\
struct hashed_pair_## NAME\
{\
    KEYTYPE Key;\
    u64 HashedKey;\
    TYPE Val;\
};\
struct NAME ## _map;\
Map_Status Scan(NAME ## _map& Map, u64 HashedKey, KEYTYPE Key);\
struct NAME ## _map \
{ \
    hashed_pair_ ## NAME* HashedPairs;\
    hashed_pair_ ## NAME* ScanPairs;\
    i32 KeyCount;\
    i32 Count; \
    hash_function_ ## NAME Hash; \
    \
    TYPE& operator[](KEYTYPE Key) \
    { \
        auto HashV = this->Hash(this->Count,Key);\
        auto Res = Scan(*this,HashV,Key);\
        switch(Res)\
        {\
            case Not_Found:\
            {\
                this->HashedPairs[HashV].Key = Key;\
                this->HashedPairs[HashV].HashedKey = HashV;\
                this->ScanPairs[this->KeyCount].Key = Key;\
                this->ScanPairs[this->KeyCount].HashedKey = HashV;\
                this->KeyCount++;\
            }break;\
            case Exists:\
            {\
                \
            }break;\
            case Collision:\
            {\
                DEBUG_PRINT("%s\n", #NAME);\
                DEBUG_PRINT("Collision\n");\
            }break;\
        }\
        return (this->HashedPairs[HashV].Val); \
    } \
    \
}; \
void NAME ##_Map_Init(NAME ## _map* Map, hash_function_ ## NAME Hash, i32 InitSize = INIT_SIZE) \
{ \
    Map->HashedPairs = (hashed_pair_ ## NAME*)calloc(InitSize, sizeof(hashed_pair_ ## NAME));\
    Map->ScanPairs = (hashed_pair_ ## NAME*)calloc(256, sizeof(hashed_pair_ ## NAME));\
    Map->KeyCount = 0;\
    Map->Count = InitSize; \
    Map->Hash = Hash; \
}\
Map_Status Scan(NAME ## _map& Map, u64 HashedKey, KEYTYPE Key)\
{\
    for(i32 I = 0; I < Map.KeyCount; I++)\
    {\
        auto& Pair = Map.ScanPairs[I];\
        if(Pair.HashedKey == HashedKey && Pair.Key != Key)\
        {\
            /* Collision! */\
            return Collision;\
        }\
        else if(Pair.HashedKey == HashedKey && Pair.Key == Key)\
        {\
            return Exists;\
        }\
    }\
    return Not_Found;\
}\
\
void Rehash(NAME ## _map* Map)\
{\
    Map->Count = Map->Count * 2;\
    for(i32 I = 0; I < Map->KeyCount; I++)\
    {\
        auto& Pair = Map->HashedPairs[I];\
    }\
}\
\

GENERIC_MAP(integer, i32, i32)

#endif
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                     