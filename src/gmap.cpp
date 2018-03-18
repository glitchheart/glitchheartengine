
// Needs a relatively high count to work with zero collisions..
// Is this to be expected? Or can we find a better hash function
// or possibly find another solution?
inline u64 hash_string(u64 size, char* key)
{
    u64 hash = 5381;
    char* k = key;
    u64 c;
    
    while ((c = (u64)*k++))
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
    
    return hash & (size - 1);
}

// Jenkins one at a time hash
// NOTE(niels): https://en.wikipedia.org/wiki/Jenkins_hash_function
inline u64 hash_string_jenkins(u64 size, char* key) 
{
    char* k = key;
    u64 length = (u64)strlen(k);
    u64 i = 0;
    u64 hash = 0;
    while (i != length) {
        hash += k[i++];
        hash += hash << 10;
        hash ^= hash >> 6;
    }
    hash += hash << 3;
    hash ^= hash >> 11;
    hash += hash << 15;
    return hash & (size - 1);
}

// Use folding on a string, summed 4 bytes at a time
inline u64 s_fold(u64 m, char* s) {
    i32 int_length = (i32)strlen(s) / 4;
    u64 sum = 0;
    for(i32 j = 0; j < int_length; j++) 
    {
        char c[32];
        memcpy(c, &s[j * 4], (u64)(j * 4) + 4);
        
        u64 mult = 1;
        for(size_t k = 0; k < strlen(c); k++) {
            sum += c[k] * mult;
            mult *= 256;
        }
    }
    
    char c[32];
    memcpy(c,&s[int_length * 4], strlen(s) - 1);
    u64 mult = 1;
    for(size_t k = 0; k < strlen(c); k++) 
    {
        sum += c[k] * mult;
        mult *= 256;
    }
    
    return(sum & (m - 1));
}

inline u64 hash_int(u64 size, i32 key) {
    u64 k = (u64)((i32)key);
    k = ((k >> 16) ^ k) * 0x45d9f3b;
    k = ((k >> 16) ^ k) * 0x45d9f3b;
    k = (k >> 16) ^ k;
    return k % (size - 1);
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

inline u64 hash_int_keys(u64 size, i32 key)
{
    u64 k = (u64)((i32)(key));
    return k % ((u32)size - 1);
}

void* allocate_memory(size_t size)
{
    return platform.allocate_memory(size, PM_UNDERFLOW_CHECK | PM_OVERFLOW_CHECK);
}

void deallocate_memory(void* block)
{
    return platform.deallocate_memory((PlatformMemoryBlock*)block);
}
