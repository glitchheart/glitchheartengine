
// Needs a relatively high count to work with zero collisions..
// Is this to be expected? Or can we find a better hash function
// or possibly find another solution?
inline u64 HashString(u64 Size, const void* Key)
{
    u64 Hash = 5381;
    char* K = (char*)Key;
    u64 C;
    
    while (C = *K++)
        Hash = ((Hash << 5) + Hash) + C; /* hash * 33 + c */
    
    return Hash & (Size - 1);
}

// Jenkins one at a time hash
// NOTE(niels): https://en.wikipedia.org/wiki/Jenkins_hash_function
inline u64 HashStringJenkins(u64 Size, const void* Key) 
{
    char* K = (char*)Key;
    u64 Length = (u64)strlen(K);
    u64 I = 0;
    u64 Hash = 0;
    while (I != Length) {
        Hash += K[I++];
        Hash += Hash << 10;
        Hash ^= Hash >> 6;
    }
    Hash += Hash << 3;
    Hash ^= Hash >> 11;
    Hash += Hash << 15;
    return Hash & (Size - 1);
}

inline u64 HashInt(u64 Size, const void* Key) {
    u64 K = (u64)(*(i32*)(Key));
    K = ((K >> 16) ^ K) * 0x45d9f3b;
    K = ((K >> 16) ^ K) * 0x45d9f3b;
    K = (K >> 16) ^ K;
    return K % (Size - 1);
}

/*
u32 HashInt(u32 Size, const void* A)
{
    u32 K = *(u32*)A;
    K += ~(K<<15);
    K ^=  (K>>10);
    K +=  (K<<3);
    K ^=  (K>>6);
    K += ~(K<<11);
    K ^=  (K>>16);
    return K & (Size - 1);
}*/

inline u64 HashIntKeys(u64 Size, const void* Key)
{
    u32 K = *(u32*)(Key);
    return K % (Size - 1);
}
