#include <iostream>
#include <fstream>
#include <sstream>

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