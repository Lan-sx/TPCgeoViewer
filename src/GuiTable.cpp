/*********************************************************************
 * Author           : Lan-sx
 * Email            : shexin@ihep.ac.cn
 * Last modified    : 2024-06-06 21:40
 * Filename         : GuiTable.cpp
 * Description      : src file of GuiTable
 * Update           : 
 * ******************************************************************/

#include <map>
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
	std::vector<X0tables> vResult;

	//merge T2K gas layers to avoiding too many rows of the gui table
	auto findT2Kmat = [](X0tables& X0table) { return X0table.matName == "T2KGas1"; };
	double sumDistance = 0.;
	double sumX0 = 0.;
	auto sumDistanceandX0 = [&](X0tables& x0table)
		{
			sumDistance += x0table.arrContent[0];
			sumX0 += x0table.arrContent[2];
		};


	auto numlayerT2K = std::count_if(v_X0tables.begin(), v_X0tables.end(), findT2Kmat);
	if (numlayerT2K >= 4)
	{
		//auto itfirst = std::find_if(v_X0tables.begin(), v_X0tables.end(), findT2Kmat);
		//auto itend = std::find_if_not(itfirst + 1, v_X0tables.end(), findT2Kmat);

		//std::for_each(itfirst, itend,sumDistanceandX0);

		//X0tables tempX0table;
		//tempX0table.matName = "T2KGas1";
		//tempX0table.arrContent[0] = sumDistance;
		//tempX0table.arrContent[1] = (*itfirst).arrContent[1];
		//tempX0table.arrContent[2] = sumDistance * 100 / (*itfirst).arrContent[1];

		//v_X0tables.erase(itfirst, itend);
		//v_X0tables.insert(itfirst, tempX0table);

		double sumD = 0.,T2KgasRadLen=0.;
		int Cnt = 0;
		auto it_start = v_X0tables.begin();
		X0tables tempX0table;
		tempX0table.matName = "T2KGas1";
		while (it_start != v_X0tables.end())
		{
			if ((*it_start).matName == "T2KGas1")
			{
				sumD += (*it_start).arrContent[0];
				Cnt++;
				T2KgasRadLen = (*it_start).arrContent[1];
				it_start = v_X0tables.erase(it_start);
			}
			else
			{
				if (Cnt > 0)
				{
					tempX0table.arrContent[0] = sumD;
					tempX0table.arrContent[1] = T2KgasRadLen;
					tempX0table.arrContent[2] = sumD * 100 / T2KgasRadLen;
					v_X0tables.insert(it_start, tempX0table);
					sumD = 0.;
					Cnt = 0;
				}
				++it_start;
			}
		}

		if (Cnt > 0)
		{
			tempX0table.arrContent[0] = sumD;
			tempX0table.arrContent[1] = T2KgasRadLen;
			tempX0table.arrContent[2] = sumD * 100 / T2KgasRadLen;
			v_X0tables.push_back(tempX0table);
		}

		vResult = std::move(v_X0tables);
	}
	else
		vResult = std::move(v_X0tables);

	sumDistance = 0.;
	sumX0 = 0.;
	std::for_each(vResult.begin(), vResult.end(), sumDistanceandX0);


	// Create a gui table to show the material budget along the track
	GuiTable* te = new GuiTable();

	for (int i = 0; i < vResult.size(); i++) {
		te->AddRow(Form("%s", vResult.at(i).matName.c_str()),
			Form("%.4f", vResult.at(i).arrContent[0]),
			Form("%.4f", vResult.at(i).arrContent[1]),
			Form("%.4f", vResult.at(i).arrContent[2])
		);
	}
	TString conttemp = Form("total material budget = %.4f", sumX0);
	auto cont = conttemp + "[%]";
	te->AddRow("Summary", Form("totalDistance = %.4f [cm]", sumDistance), "", cont.Data());

	te->Show();
}
