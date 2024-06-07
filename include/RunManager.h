/*********************************************************************
 * Author           : Lan-sx
 * Email            : shexin@ihep.ac.cn
 * Last modified    : 2024-06-05 20:47
 * Filename         : RunManager.h
 * Description      : A control class for Geo debug and vis
 * Update           : 
 * ******************************************************************/
#ifndef __RunManager_H__
#define __RunManager_H__ 1

#include <iostream>
#include <TSystem.h>
#include <TString.h>
#include <string>

class RunManager
{
public:
	RunManager(int mode = 1) : fmode(mode) {};
	virtual ~RunManager() {}

	inline void SetMode(int mode);

	//Run Mode
	void RunMode(int mode);

	//Extract Gentle geo file
	bool RunExtractGentleFile();
	
	//Geo Display
	bool RunGeoDisplay();

protected:
	std::string InputRawGeofile();
	std::string InputGentleGeofile();

private:
	int fmode;
};

inline void RunManager::SetMode(int mode)
{
	fmode = mode;
}


#endif
