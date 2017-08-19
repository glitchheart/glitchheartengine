#ifndef MODEL_FORMAT_H
#define MODEL_FORMAT_H

struct model_header
{
    char Format[4]; // M O D L ?
    char Version[4];
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
    /*long NumUVs;
   long UVChunkSize;
   long NumNormals;
   long NormalsChunkSize;
   long NumColors;
   long ColorsChunkSize;*/
};

struct chunk_format
{
    char Format[4]; // M E S H / E O F
};

#endif
