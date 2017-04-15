#include <Windows.h>

static GLchar* LoadShaderFromFile(const std::string Path)
{
	GLchar *Source = {};

	FILE *File;
	File = fopen(Path.c_str(), "rb");
	if(File)
	{
		fseek(File, 0, SEEK_END);
		uint32 Size = ftell(File);
		fseek(File, 0, SEEK_SET);

		Source = (GLchar *)malloc(Size+1);
		fread(Source, Size, 1, File); 
		Source[Size] = '\0';

		fclose(File);
	}
	else
	{
		std::cerr << "Could not read file " << Path << ". File does not exist." << std::endl;
	}	

	return Source;
}

struct asset_manager
{
	bool ListenForChanges;

	//shaders
	const char* VertexShaderPaths[2] = { "./assets/shaders/textureshader.vert", "./assets/shaders/tileshader.vert" };
	const char* FragmentShaderPaths[2] = { "./assets/shaders/textureshader.frag", "./assets/shaders/tileshader.frag" };
	uint32 DirtyVertexShaderIndices[2]; //2 is the number of shaders, set to 1 if they should be reloaded
	uint32 DirtyFragmentShaderIndices[2];
	time_t VertexShaderTimes[2];
	time_t FragmentShaderTimes[2];

	//dll's
	const char* DllPaths[1] = { "game.dll" };
	uint32 DirtyGameDll;
	FILETIME GameDllTime;
};

static FILETIME GetLastWriteTime(const char* FilePath)
{
	FILETIME LastWriteTime = {};

	WIN32_FIND_DATA FindData;
	HANDLE FindHandle = FindFirstFileA(FilePath, &FindData);

	if(FindHandle != INVALID_HANDLE_VALUE)
	{
		LastWriteTime = FindData.ftLastWriteTime;
		FindClose(FindHandle);
	}
	return LastWriteTime;
}

static void ListenToFileChanges(asset_manager* AssetManager)
{
	AssetManager->ListenForChanges = true;

	using namespace std::chrono_literals;

	while (AssetManager->ListenForChanges) 
	{
		for (int i = 0; i < 2; i++) 
		{
			struct stat sb;
			stat(AssetManager->VertexShaderPaths[i], &sb);

			time_t time = sb.st_mtime;
			auto last_time = AssetManager->VertexShaderTimes[i];

			if (last_time != 0 && last_time < time)
			{
				AssetManager->DirtyVertexShaderIndices[i] = 1;
			}
			AssetManager->VertexShaderTimes[i] = time;
		}

		for (int i = 0; i < 2; i++) 
		{
			struct stat sb;
			stat(AssetManager->FragmentShaderPaths[i], &sb);

			time_t time = sb.st_mtime;
			auto last_time = AssetManager->FragmentShaderTimes[i];

			if (last_time != 0 && last_time < time)
			{
				AssetManager->DirtyFragmentShaderIndices[i] = 1;
			}
			AssetManager->FragmentShaderTimes[i] = time;
		}

		std::this_thread::sleep_for(1s);
	}
}