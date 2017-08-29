#ifndef MODEL_FORMAT_H
#define MODEL_FORMAT_H

struct model_header
{
    char Format[4]; // M O D L ?
    char Version[4];
};

struct bone_header
{
    long NumBones;
    long BoneChunkSize;
};

struct animation_header
{
    r64 TotalTime;
    i32 NumFrames;
};

struct mesh_header
{
    long NumVertices;
    long VertexChunkSize;
    long NumNormals;
    long NumUVs;
    long NormalsChunkSize;
    long NumFaces;
    long FacesChunkSize;
    bool HasTexture;
    char TextureFile[100];
};

struct chunk_format
{
    char Format[4]; // M E S H / E O F / B O N E / A N I M
};

#endif
