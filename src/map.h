#ifndef MAP_H
#define MAP_H

#define HASH_FUNCTION(name) uint64_t name(void* key)
typedef HASH_FUNCTION(HashFunction);

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
    
    
    void* operator[](void* key)
    {
        return map__get(this, key);
    }
    
    
    void* operator[](char* key)
    {
        return map__get(this, (void*)key);
    }
};


HASH_FUNCTION(uint64_hash) 
{
    uint64_t x = (uintptr_t)key;
    x *= 0xff51afd7ed558ccdul;
    x ^= x >> 32;
    return x;
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

void map_init(Map* map, HashFunction* hash_function)
{
    map->hash_function = hash_function;
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
        if(map->keys[i] == key)
        {
            return map->vals[i];
        }
        else if(!map->keys[i])
        {
            return NULL;
        }
        i++;
    }
    return NULL;
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
    assert(key);
    assert(val);
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
        else if(map->keys[i] == key)
        {
            map->vals[i] = val;
            return;
        }
        i++;
    }
}

void map_test()
{
    Map map = {0};
    map_init(&map, str_hash);
    enum {N = 1024};
    for(size_t i = 1; i < N; i++)
    {
        map_put(&map, "boob", (i + 1));
    }
    
    //i32* val = (i32*)map["boob"];
    //i32* val = (i32*)map_get(&map, "boob");
    //assert(val == (i32*)N);
    
}

#endif

