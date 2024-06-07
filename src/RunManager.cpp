/*********************************************************************
 * Author           : Lan-sx
 * Email            : shexin@ihep.ac.cn
 * Last modified    : 2024-06-05 20:47
 * Filename         : RunManager.cpp
 * Description      : src file of RunManager
 * Update           : 
 * ******************************************************************/

#include "RunManager.h"
#include "geoEveViewer.h"

void RunManager::RunMode(int mode)
{

	switch (mode)
	{
	case 0:
		//bool flags1 = this->RunExtractGentleFile();
		if (!this->RunExtractGentleFile())
			std::printf("=========> Extract Gentle file failed!\n");
		
		gSystem->Exit(-1);
	case 1:
		//bool flags2 = this->RunGeoDisplay();
		if (!this->RunGeoDisplay())
			std::printf("=========> RunGeoDisplay failed!\n");

		std::printf("Press any key!\n");
		std::cin.get();
	default:
		std::printf("================== Good Bye!\n");
		break;
	}
}

bool RunManager::RunExtractGentleFile()
{
	auto rawgeofile = InputGentleGeofile();
	auto gentlegeofile = InputGentleGeofile();

	//geoEveViewer::ExtractGentleVolume(rawgeofile, gentlegeofile, 80.);

	return true;
}

bool RunManager::RunGeoDisplay()
{

	return true;
}

std::string RunManager::InputRawGeofile()
{
	std::printf("===============input raw geo file !\n");
	char filename[50];

	std::scanf("> %s", filename);
	return std::string(filename);
}

std::string RunManager::InputGentleGeofile()
{
	std::printf("===============input gentle geo file !\n");
	char filename[50];

	std::scanf("%s", filename);
	return std::string(filename);

}