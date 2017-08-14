#ifndef MODEL_FORMAT_H
#define MODEL_FORMAT_H

struct model_header
{
    char Format[4]; // M O D L ?
};

struct faces_chunk
{
    char FacesChunkID[4]; // F C E S
    long FacesChunkSize;
    unsigned char* FacesData; // Index buffer?
};

struct vertex_buffer_chunk
{
    char ChunkID[4]; // V B O _
    char Format[4]; // V U N C / V U N _ / V N _ _ / V _ _ _
    long ChunkSize;
    unsigned char* Data; // This is the VBO data in the format specified
};

#endif
