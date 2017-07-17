#ifndef GMAP_H
#define GMAP_H

#define INIT_SIZE 256

struct gmap;

typedef uint32_t (*hash_function)(gmap* Map, const void* Key);

struct gmap
{
    void** Data;
    int Count;
    hash_function Hash;
    
    animation& operator[](const void* Key)
    {
        return *((animation*)this->Data[this->Hash(this,Key)]);
    }
    
    int& operator[](const int Key)
    {
        return *((int*)this->Data[this->Hash(this,&Key)]);
    }
    
};

#endif