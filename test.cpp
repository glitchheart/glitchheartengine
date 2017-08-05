
using hash_function_texture = u64 (*)(u64 Size, char *Key);
struct hashed_pair_texture
{
    char *Key;
    u64 HashedKey;
    texture *Val;
};
struct texture_map;
Map_Status Scan(texture_map *Map, u64 HashedKey, char *Key);
void Rehash(texture_map *Map);
struct texture_map
{
    hashed_pair_texture *HashedPairs;
    hashed_pair_texture *ScanPairs;
    i32 KeyCount;
    i32 Count;
    hash_function_texture Hash;
    texture *&operator[](char *Key)
    {
        if (!(StrCmp(0, Key) != 0))
        {
            printf("Assertion failed in: %s on line %d\n", "c:\\users\\niels\\documents\\glitchheartgame2017\\src\\opengl_rendering.h", 103);
            exit(1);
        };
        auto HashV = this->Hash(this->Count, Key);
        auto Res = Scan(this, HashV, Key);
        switch (Res)
        {
        case Not_Found:
        {
            this->HashedPairs[HashV].Key = (char *)malloc(sizeof(char) * (strlen(Key) + 1));
            strcpy(this->HashedPairs[HashV].Key, Key);
            this->HashedPairs[HashV].HashedKey = HashV;
            this->ScanPairs[KeyCount].Key = (char *)malloc(sizeof(char) * (strlen(Key) + 1));
            strcpy(this->ScanPairs[KeyCount].Key, Key);
            this->ScanPairs[this->KeyCount].HashedKey = HashV;
            this->KeyCount++;
        }
        break;
        case Exists:
        {
        }
        break;
        case Collision:
        {
            Rehash(this);
            auto NewHash = this->Hash(this->Count, Key);
            this->HashedPairs[NewHash].Key = (char *)malloc(sizeof(char) * (strlen(Key) + 1));
            strcpy(this->HashedPairs[NewHash].Key, Key);
            this->HashedPairs[NewHash].HashedKey = NewHash;
            this->ScanPairs[KeyCount].Key = (char *)malloc(sizeof(char) * (strlen(Key) + 1));
            strcpy(this->ScanPairs[KeyCount].Key, Key);
            this->ScanPairs[this->KeyCount].HashedKey = NewHash;
            this->KeyCount++;
            return this->HashedPairs[NewHash].Val;
        }
        break;
        }
        return this->HashedPairs[HashV].Val;
    }
};
void texture_Map_Init(texture_map *Map, hash_function_texture Hash, i32 InitSize = 1024)
{
    Map->HashedPairs = (hashed_pair_texture *)calloc(InitSize, sizeof(hashed_pair_texture));
    Map->ScanPairs = (hashed_pair_texture *)calloc(1024, sizeof(hashed_pair_texture));
    Map->KeyCount = 0;
    Map->Count = InitSize;
    Map->Hash = Hash;
}
Map_Status Scan(texture_map *Map, u64 HashedKey, char *Key)
{
    for (i32 I = 0; I < Map->KeyCount; I++)
    {
        auto Pair = Map->ScanPairs[I];
        if (!(StrCmp(0, Pair.Key) != 0))
        {
            printf("Assertion failed in: %s on line %d\n", "c:\\users\\niels\\documents\\glitchheartgame2017\\src\\opengl_rendering.h", 103);
            exit(1);
        };
        if (StrCmp(0, Pair.Key) != 0 && Pair.HashedKey == HashedKey && StrCmp(Pair.Key, Key) != 0)
        {
            return Collision;
        }
        else if (Pair.HashedKey == HashedKey && Pair.Key == Key)
        {
            return Exists;
        }
    }
    return Not_Found;
}
void Rehash(texture_map *Map)
{
    for (i32 I = 0; I < Map->KeyCount; I++)
    {
        auto Pair = Map->ScanPairs[I];
        auto Val = Map->HashedPairs[Pair.HashedKey].Val;
        Map->ScanPairs[I].Val = Val;
    }
    Map->Count = Map->Count * 2;
    free(Map->HashedPairs);
    Map->HashedPairs = (hashed_pair_texture *)calloc(Map->Count, sizeof(hashed_pair_texture));
    for (i32 I = 0; I < Map->KeyCount; I++)
    {
        if (!(StrCmp(0, Map->ScanPairs[I].Key) != 0))
        {
            printf("Assertion failed in: %s on line %d\n", "c:\\users\\niels\\documents\\glitchheartgame2017\\src\\opengl_rendering.h", 103);
            exit(1);
        };
        auto NewHash = Map->Hash(Map->Count, Map->ScanPairs[I].Key);
        Map->HashedPairs[NewHash].Val = Map->ScanPairs[I].Val;
        Map->HashedPairs[NewHash].Key = (char *)malloc(sizeof(char) * (strlen(Map->ScanPairs[I].Key) + 1));
        strcpy(Map->HashedPairs[NewHash].Key, Map->ScanPairs[I].Key);
        Map->HashedPairs[NewHash].HashedKey = NewHash;
        Map->ScanPairs[I].HashedKey = NewHash;
    }
};
