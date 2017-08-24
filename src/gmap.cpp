
// Needs a relatively high count to work with zero collisions..
// Is this to be expected? Or can we find a better hash function
// or possibly find another solution?
inline u64 HashString(u64 Size, char* Key)
{
    u64 Hash = 5381;
    char* K = Key;
    u64 C;
    
    while (C = *K++)
        Hash = ((Hash << 5) + Hash) + C; /* hash * 33 + c */
    
    return Hash & (Size - 1);
}

// Jenkins one at a time hash
// NOTE(niels): https://en.wikipedia.org/wiki/Jenkins_hash_function
inline u64 HashStringJenkins(u64 Size, char* Key) 
{
    char* K = Key;
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

// Use folding on a string, summed 4 bytes at a time
inline u64 SFold(u64 M, char* s) {
    i32 intLength = (i32)strlen(s) / 4;
    unsigned long sum = 0;
    for (int j = 0; j < intLength; j++) 
    {
        char c[32];
        memcpy(c,&s[j * 4],(j * 4) + 4);
        
        unsigned long mult = 1;
        for (int k = 0; k < strlen(c); k++) {
            sum += c[k] * mult;
            mult *= 256;
        }
    }
    
    char c[32];
    memcpy(c,&s[intLength * 4],strlen(s) - 1);
    unsigned long mult = 1;
    for (int k = 0; k < strlen(c); k++) 
    {
        sum += c[k] * mult;
        mult *= 256;
    }
    
    return(sum & (M - 1));
}

inline u64 HashInt(u64 Size, i32 Key) {
    u64 K = (u64)((i32)Key);
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

inline u64 HashIntKeys(u64 Size, i32 Key)
{
    u64 K = (u64)((i32)(Key));
    return K % ((u32)Size - 1);
}

void* AllocateMemory(size_t Size)
{
    return Platform.AllocateMemory(Size, PM_UnderflowCheck | PM_OverflowCheck);
}

void DeallocateMemory(void* Block)
{
    return Platform.DeallocateMemory((platform_memory_block*)Block);
}
