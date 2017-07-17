#ifndef GMAP_H
#define GMAP_H

#define INIT_SIZE 1024

struct gmap;

typedef u32 (*hash_function)(gmap* Map, const void* Key);

struct gmap
{
    void** Data;
    i32 Count;
    hash_function Hash;
    
    animation& operator[](const void* Key)
    {
        if(this->Data[this->Hash(this,Key)])
            return *((animation*)this->Data[this->Hash(this,Key)]);
        else
        {
            this->Data[this->Hash(this,Key)] = malloc(sizeof(animation));
            return *((animation*)this->Data[this->Hash(this,Key)]);
        }
    }
    
    i32& operator[](const i32 Key)
    {
        if(this->Data[this->Hash(this,&Key)])
            return *((i32*)this->Data[this->Hash(this,&Key)]);
        else
        {
            this->Data[this->Hash(this,&Key)] = (i32*)malloc(sizeof(i32));
            return *((i32*)this->Data[this->Hash(this,&Key)]);
        }
    }
};

#endif