#ifndef GMAP_H
#define GMAP_H

#define INIT_SIZE 1024

typedef u32 (*hash_function)(u32 Size, const void* Key);

#define GENERIC_MAP(NAME,TYPE) \
struct NAME ## _map \
{ \
    void** Data; \
    i32 Count; \
    hash_function Hash; \
    \
    TYPE& operator[](const void* Key) \
    { \
        if(this->Data[this->Hash(this->Count,Key)]) \
        return *((TYPE*)this->Data[this->Hash(this->Count,Key)]); \
        else \
        { \
            this->Data[this->Hash(this->Count,Key)] = malloc(sizeof(TYPE)); \
            return *((TYPE*)this->Data[this->Hash(this->Count,Key)]); \
        } \
    } \
    \
    TYPE& operator[](const i32 Key) \
    { \
        if(this->Data[this->Hash(this->Count,&Key)]) \
        return *((TYPE*)this->Data[this->Hash(this->Count,&Key)]); \
        else \
        { \
            this->Data[this->Hash(this->Count,&Key)] = malloc(sizeof(TYPE)); \
            return *((TYPE*)this->Data[this->Hash(this->Count,&Key)]); \
        } \
    } \
}; \
void NAME ## _Map_Init(NAME ## _map* Map, hash_function Hash, i32 InitSize = INIT_SIZE) \
{ \
    Map->Data = (void**)calloc(InitSize, sizeof(TYPE*));\
    Map->Count = InitSize; \
    Map->Hash = Hash; \
}\
\
void ClearMap(NAME ## _map* Map)\
{\
    Map->Data = (void**)calloc(Map->Count, sizeof(TYPE*));\
}

GENERIC_MAP(integer, i32)

#endif