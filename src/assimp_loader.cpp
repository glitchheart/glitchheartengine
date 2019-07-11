#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

void print_node(aiNode *node, const aiScene *scene)
{
    printf("Node: %s\n", node->mName.data);
    
    for (i32 i = 0; i < node->mNumMeshes; i++)
    {
        const struct aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        printf("%d: %s\n", i, mesh->mName.data);
    }
    
    for(i32 i = 0; i < node->mNumChildren; i++)
    {
		print_node(node->mChildren[i], scene);
    }
}

// Currently loads any model file and prints all node- and mesh-name
b32 load_model_file(char* file_path)
{
  const aiScene* scene = aiImportFile(file_path, aiProcess_CalcTangentSpace |
                                                 aiProcess_Triangulate |
                                                 aiProcess_JoinIdenticalVertices |
                                                 aiProcess_SortByPType);
  
  if(!scene)
  {
      printf("%s\n", aiGetErrorString());
      return false;
  }

  aiNode *root_node = scene->mRootNode;
  print_node(root_node, scene);
  
  aiReleaseImport(scene);
  return true;
}
  
