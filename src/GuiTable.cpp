/*********************************************************************
 * Author           : Lan-sx
 * Email            : shexin@ihep.ac.cn
 * Last modified    : 2024-06-06 21:40
 * Filename         : GuiTable.cpp
 * Description      : src file of GuiTable
 * Update           : 
 * ******************************************************************/

#include "GuiTable.h"

 //**********************************************************************
GuiTable::GuiTable(const char* col1, const char* col2, const char* col3, const char* col4)
{
	//
	fLo = new TGLayoutHints(kLHintsExpandX | kLHintsExpandY | kLHintsExpandX);

	TGLayoutHints* lo1 = new TGLayoutHints(kLHintsLeft | kLHintsExpandY);
	TGLayoutHints* lo2 = new TGLayoutHints(kLHintsLeft | kLHintsExpandX | kLHintsExpandY);
	TGLayoutHints* lo3 = new TGLayoutHints(kLHintsLeft | kLHintsExpandX | kLHintsExpandY);
	TGLayoutHints* lo4 = new TGLayoutHints(kLHintsRight | kLHintsExpandX | kLHintsExpandY);

	fMain = new TGMainFrame(gClient->GetRoot(), 1, 1, kHorizontalFrame);
	TGCompositeFrame* h = new TGCompositeFrame(fMain, 100, 100, kHorizontalFrame | kRaisedFrame);
	fMain->AddFrame(h, fLo);

	fV1 = new TGVerticalFrame(h, 2, 1, kFixedWidth);
	h->AddFrame(fV1, lo1);

	TGVSplitter* splitter = new TGVSplitter(h);
	splitter->SetFrame(fV1, kTRUE);
	h->AddFrame(splitter, lo1);

	TGTextButton* b = new TGTextButton(fV1, col1);
	fV1->AddFrame(b, fLo);
	fV1->Resize(b->GetDefaultSize());

	fV2 = new TGVerticalFrame(h, 1, 1);
	h->AddFrame(fV2, lo2);
	b = new TGTextButton(fV2, col2);
	fV2->AddFrame(b, fLo);

	fV3 = new TGVerticalFrame(h, 1, 1);
	h->AddFrame(fV3, lo3);
	b = new TGTextButton(fV3, col3);
	fV3->AddFrame(b, fLo);

	fV4 = new TGVerticalFrame(h, 1, 1);
	h->AddFrame(fV4, lo4);
	b = new TGTextButton(fV4, col4);
	fV4->AddFrame(b, fLo);

	h->MapSubwindows();
	fMain->Resize();
}

//**********************************************************************
void GuiTable::AddRow(const char* e1, const char* e2, const char* e3, const char* e4)
{
	//
	TGTextEntry* te = new TGTextEntry(fV1, e1);
	fV1->AddFrame(te, fLo);

	te = new TGTextEntry(fV2, e2);
	fV2->AddFrame(te, fLo);

	te = new TGTextEntry(fV3, e3);
	fV3->AddFrame(te, fLo);

	te = new TGTextEntry(fV4, e4);
	fV4->AddFrame(te, fLo);

	fMain->Resize();
}

//**********************************************************************
void GuiTable::Show()
{
	//
	fMain->MapSubwindows();
	fMain->MapRaised();
}

//**********************************************************************
void GuiTable::ShowX0guiTable(std::vector<X0tables>& v_X0tables)
{
	double totalDistance = 0.;
	double totalX0 = 0.;
	auto sumDistanceandX0 = [&](X0tables& x0table)
	{
		totalDistance += x0table.arrContent[0];
		totalX0 += x0table.arrContent[2];
	};

	std::for_each(v_X0tables.begin(), v_X0tables.end(), sumDistanceandX0);


	// Create a gui table to show the material budget along the track
	GuiTable* te = new GuiTable();

	for (int i = 0; i < v_X0tables.size(); i++) {
		te->AddRow(Form("%s", v_X0tables.at(i).matName.c_str()),
			Form("%.4f", v_X0tables.at(i).arrContent[0]),
			Form("%.4f", v_X0tables.at(i).arrContent[1]),
			Form("%.4f", v_X0tables.at(i).arrContent[2])
		);
	}
	TString conttemp = Form("total material budget = %.4f", totalX0);
	auto cont = conttemp + "[%]";
	te->AddRow("Summary", Form("totalDistance = %.4f [cm]", totalDistance), "", cont.Data());

	te->Show();
}
