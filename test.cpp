
struct hashed_pair_i32
{
    i32 Key;
    u64 HashedKey;
};
struct integer_map
{
    hashed_pair_i32 *HashedPairs;
    i32 KeyCount;
    i32 **Data;
    i32 Count;
    hash_function Hash;
    i32 &operator[](const void *Key)
    {
        u64 HashV = this->Hash(this->Count, Key);
        if (this->Data[HashV])
            return *(this->Data[HashV]);
        else
        {
            this->Data[HashV] = (i32 *)malloc(sizeof(i32));
            return *(this->Data[HashV]);
        }
    }
    i32 &operator[](const i32 Key)
    {
        if (this->Data[this->Hash(this->Count, &Key)])
            return *(this->Data[this->Hash(this->Count, &Key)]);
        else
        {
            this->Data[this->Hash(this->Count, &Key)] = (i32 *)malloc(sizeof(i32));
            return *(this->Data[this->Hash(this->Count, &Key)]);
        }
    }
};
void integer_Map_Init(integer_map *Map, hash_function Hash, i32 InitSize = 1024)
{
    Map->Data = (i32 **)calloc(InitSize, sizeof(i32 *));
    Map->HashedPairs = (hashed_pair_i32 *)calloc(64, sizeof(hashed_pair_i32));
    Map->KeyCount = 0;
    Map->Count = InitSize;
    Map->Hash = Hash;
}
void ClearMap(integer_map *Map) { Map->Data = (i32 **)calloc(Map->Count, sizeof(i32 *)); }

struct hashed_pair_integer
{
    i32 Key;
    u64 HashedKey;
};
struct integer_map
{
    hashed_pair_integer *HashedPairs;
    i32 KeyCount;
    i32 *Data;
    i32 Count;
    hash_function Hash;
    i32 &operator[](const void *Key)
    {
        auto HashV = this->Hash(this->Count, Key);
        if (this->Data[HashV])
            return *(this->Data[HashV]);
        else
        {
            this->Data[HashV] = (i32 *)malloc(sizeof(i32));
            return *(this->Data[HashV]);
        }
    }
    i32 &operator[](const i32 Key)
    {
        auto HashV = this->Hash(this->Count, &Key);
        if (this->Data[HashV])
            return *(this->Data[HashV]);
        else
        {
            this->Data[HashV] = (i32 *)malloc(sizeof(i32));
            return *(this->Data[HashV]);
        }
    }
};
void integer_Map_Init(integer_map *Map, hash_function Hash, i32 InitSize = 1024)
{
    Map->Data = (i32 *)calloc(InitSize, sizeof(i32));
    Map->Count = InitSize;
    Map->Hash = Hash;
}
void ClearMap(integer_map *Map) { Map->Data = (i32 *)calloc(Map->Count, sizeof(i32)); }
