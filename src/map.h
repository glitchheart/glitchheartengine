#ifndef MAP_H
#define MAP_H

#define HASH_FUNCTION(name) uint64_t name(void* key)
typedef HASH_FUNCTION(HashFunction);

#define KEY_COMPARE_FUNCTION(name) b32 name(void* val_1, void* val_2)
typedef KEY_COMPARE_FUNCTION(KeyCompareFunction);

struct Map;


#define map_get(map, key) map__get(map, (void*)key)
void* map__get(Map* map, void* key);

struct Map
{
    void** keys;
    void** vals;
    size_t len;
    size_t cap;
    HashFunction* hash_function;
    KeyCompareFunction* key_compare_function;
};

KEY_COMPARE_FUNCTION(str_cmp)
{
    return strcmp((char*)val_1, (char*)val_2) == 0;
}

KEY_COMPARE_FUNCTION(ptr_cmp)
{
    return val_1 == val_2;
}

HASH_FUNCTION(uint64_hash) 
{
    uint64_t x = (uintptr_t)key;
    x *= 0xff51afd7ed558ccdul;
    x ^= x >> 32;
    return x;
}

HASH_FUNCTION(int_hash) {
    u64 k = (u64)((intptr_t)key) + 1;
    k = ((k >> 16) ^ k) * 0x45d9f3b;
    k = ((k >> 16) ^ k) * 0x45d9f3b;
    k = (k >> 16) ^ k;
    return k;
}

HASH_FUNCTION(ptr_hash)
{
    return uint64_hash(key);
}

HASH_FUNCTION(str_hash)
{
    char* str = (char*)key;
    uint64_t x = 0xcbf29ce484222325ull;
    // Only works if null-terminated string
    size_t len = strlen(str);
    for (size_t i = 0; i < len; i++) {
        x ^= str[i];
        x *= 0x100000001b3ull;
        x ^= x >> 32;
    }
    return x;
}

#define IS_POW2(x) (((x) != 0) && ((x) & ((x)-1)) == 0)

void map_init(Map* map, HashFunction* hash_function, KeyCompareFunction key_compare_function = ptr_cmp)
{
    map->hash_function = hash_function;
    map->key_compare_function = key_compare_function;
    assert(map->hash_function);
}

void* map__get(Map* map, void* key)
{
    assert(map->hash_function);
    if(map->len == 0)
    {
        return NULL;
    }
    assert(IS_POW2(map->cap));
    size_t i = (size_t)map->hash_function(key);
    assert(map->len < map->cap);
    for(;;)
    {
        i &= map->cap - 1;
        if(map->key_compare_function(map->keys[i], key))
        {
            return map->vals[i];
        }
        else if(!map->keys[i])
        {
            return NULL;
        }
        i++;
    }
}

void map__put(Map* map, void* key, void* val);

void map_grow(Map* map, size_t new_cap)
{
    assert(map->hash_function);
    new_cap = MAX(16, new_cap);
    Map new_map = {};
    new_map.keys = (void**)calloc(new_cap, sizeof(void*));
    new_map.vals = (void**)malloc(new_cap * sizeof(void*));
    new_map.cap = new_cap;
    new_map.hash_function = map->hash_function;
    new_map.key_compare_function = map->key_compare_function;
    
    for(size_t i = 0; i < map->cap; i++)
    {
        if(map->keys[i])
        {
            map__put(&new_map, map->keys[i], map->vals[i]);
        }
    }
    free(map->keys);
    free(map->vals);
    *map = new_map;
}

#define map_put(map, key, val) map__put(map, (void*)key, (void*)val)
void map__put(Map* map, void* key, void* val)
{
    assert(map->hash_function);
    //assert(key);
    //assert(val);
    if(2 * map->len >= map->cap)
    {
        map_grow(map, 2 * map->cap);
    }
    assert(2 * map->len < map->cap);
    assert(IS_POW2(map->cap));
    size_t i = (size_t)map->hash_function(key);
    for(;;)
    {
        i &= map->cap - 1;
        if(!map->keys[i])
        {
            map->len++;
            map->keys[i] = key;
            map->vals[i] = val;
            return;
        }
        else if(map->key_compare_function(map->keys[i], key))
        {
            map->vals[i] = val;
            return;
        }
        i++;
    }
}

void map_test()
{
    Map str_map = {0};
    map_init(&str_map, str_hash, str_cmp);
    map_put(&str_map, "peep", "poop");
    auto res = map_get(&str_map, "peep");
    UNUSED(res);
    
    Map int_map = {0};
    map_init(&int_map, str_hash, str_cmp);
    map_put(&int_map, "peep", 5);
    auto i_res = map_get(&int_map, "peep");
    UNUSED(i_res);
    
}

#endif

