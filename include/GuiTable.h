/*********************************************************************
 * Author           : Lan-sx
 * Email            : shexin@ihep.ac.cn
 * Last modified    : 2024-06-06 21:39
 * Filename         : GuiTable.h
 * Description      : A Gui Table to show the material budget 
 *                    along the straight track
 * Update           : 
 * ******************************************************************/
#ifndef __GuiTable_H__
#define __GuiTable_H__ 1

#include <TGButton.h>
#include <TGSplitter.h>
#include <TGTextEntry.h>

#include <vector>
#include <algorithm>
#include "geoEveViewer.h"

class GuiTable {

private:
	TGMainFrame* fMain;
	TGLayoutHints* fLo;
	TGVerticalFrame* fV1;
	TGVerticalFrame* fV2;
	TGVerticalFrame* fV3;
	TGVerticalFrame* fV4;

public:
	GuiTable(const char* col1 = "Material", const char* col2 = "D [cm]", const char* col3 = "X0 [cm]", const char* col4 = "D/X0 [%]");
	~GuiTable() { delete fMain; }



	//Show a gui table to record the material budget along the virtual track, call "Start_Stack()" first
	static void ShowX0guiTable(std::vector<X0tables>& v_X0tables);

protected:
	void AddRow(const char* e1 = "", const char* e2 = "", const char* e3 ="", const char* e4="");
	void Show();



};

#endif
