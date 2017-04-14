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

	const char* VertexShaderPaths[2] = { "./assets/shaders/textureshader.vert", "./assets/shaders/tileshader.vert" };
	const char* FragmentShaderPaths[2] = { "./assets/shaders/textureshader.frag", "./assets/shaders/tileshader.frag" };
	uint32 DirtyVertexShaderIndices[2]; //2 is the number of shaders, set to 1 if they should be reloaded
	uint32 DirtyFragmentShaderIndices[2];

	time_t VertexShaderTimes[2];
	time_t FragmentShaderTimes[2];
};


static void ListenToFileChanges(asset_manager* AssetManager)
{
	AssetManager->ListenForChanges = true;

	// Available with VS2015, Recent GCC and Clang have it too
	using namespace std::chrono_literals;

	while (AssetManager->ListenForChanges) 
	{
		for (int i = 0; i < 2; i++) 
		{
			struct stat sb;
			stat(AssetManager->VertexShaderPaths[i], &sb);

			// Check the last time the file was written
			time_t time = sb.st_mtime;
			auto last_time = AssetManager->VertexShaderTimes[i];

			// And compare it with the known time. 
			if (last_time != 0 && last_time < time) {
				// If they do not match, mark the program as needing a reload
				AssetManager->DirtyVertexShaderIndices[i] = 1;
			}
			AssetManager->VertexShaderTimes[i] = time;
		}

		for (int i = 0; i < 2; i++) 
		{
			struct stat sb;
			stat(AssetManager->FragmentShaderPaths[i], &sb);

			// Check the last time the file was written
			time_t time = sb.st_mtime;
			auto last_time = AssetManager->FragmentShaderTimes[i];

			// And compare it with the known time. 
			if (last_time != 0 && last_time < time) {
				// If they do not match, mark the program as needing a reload
				AssetManager->DirtyFragmentShaderIndices[i] = 1;
			}
			AssetManager->FragmentShaderTimes[i] = time;
		}

		// Just run this code once every second
		std::this_thread::sleep_for(1s);
	}
}