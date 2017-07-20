
// Needs a relatively high count to work with zero collisions..
// Is this to be expected? Or can we find a better hash function
// or possibly find another solution?
u32 HashString(u32 Size, const void* Key)
{
    unsigned long Hash = 5381;
    char* K = (char*)Key;
    u32 C;
    
    while (C = *K++)
        Hash = ((Hash << 5) + Hash) + C; /* hash * 33 + c */
    
    return Hash % Size;
}

u32 HashInt(u32 Size, const void* Key) {
    u32 K = *(u32*)(Key);
    K = ((K >> 16) ^ K) * 0x45d9f3b;
    K = ((K >> 16) ^ K) * 0x45d9f3b;
    K = (K >> 16) ^ K;
    //printf("Before mod: %d\n", K);
    //printf("Hash: %d\n", K % Size);
    return K % Size;
}
