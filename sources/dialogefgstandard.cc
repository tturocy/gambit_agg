//
// FILE: dialogefgstandard.cc -- Standard solution methods for extensive form
//
// $Id$
//

#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#include "wx/mdi.h"
#endif

#include "efg.h"
#include "dialogauto.h"
#include "dialogefgstandard.h"

//========================================================================
//                dialogEfgStandard: Member functions
//========================================================================

const int idSTANDARD_TYPE = 1000;
const int idSTANDARD_NUM = 1001;

BEGIN_EVENT_TABLE(dialogEfgStandard, wxDialog)
  EVT_RADIOBOX(idSTANDARD_TYPE, dialogEfgStandard::OnChange)
  EVT_RADIOBOX(idSTANDARD_NUM, dialogEfgStandard::OnChange)
END_EVENT_TABLE()

dialogEfgStandard::dialogEfgStandard(wxWindow *p_parent, const Efg &p_efg)
  : guiAutoDialog(p_parent, "Standard Solution"), m_efg(p_efg)
{
  //  gText defaultsFile(gambitApp.ResourceFile());
  int standardType = 0, standardNum = 0, precision = 0;
  /*
  wxGetResource(SOLN_SECT, "Efg-Standard-Type", &standardType, 
		defaultsFile);
  wxGetResource(SOLN_SECT, "Efg-Standard-Num", &standardNum,
		defaultsFile);
  wxGetResource(SOLN_SECT, "Efg-Standard-Precision", &precision, defaultsFile);
  */

  wxString typeChoices[] = { "Nash", "Subgame Perfect", "Sequential" };
  m_standardType = new wxRadioBox(this, idSTANDARD_TYPE, "Type",
				  wxDefaultPosition, wxDefaultSize,
				  3, typeChoices, 0, wxRA_SPECIFY_ROWS);
  m_standardType->SetSelection(standardType);
  m_standardType->SetConstraints(new wxLayoutConstraints);
  m_standardType->GetConstraints()->left.SameAs(this, wxLeft, 10);
  m_standardType->GetConstraints()->top.SameAs(this, wxTop, 10);
  m_standardType->GetConstraints()->width.AsIs();
  m_standardType->GetConstraints()->height.AsIs();

  wxString numChoices[] = { "One", "Two", "All" };
  m_standardNum = new wxRadioBox(this, idSTANDARD_NUM, "Number",
				 wxDefaultPosition, wxDefaultSize,
				 3, numChoices, 0, wxRA_SPECIFY_ROWS);
  m_standardNum->SetSelection(standardNum);
  m_standardNum->SetConstraints(new wxLayoutConstraints);
  m_standardNum->GetConstraints()->left.SameAs(m_standardType, wxRight, 10);
  m_standardNum->GetConstraints()->top.SameAs(m_standardType, wxTop);
  m_standardNum->GetConstraints()->width.AsIs();
  m_standardNum->GetConstraints()->height.AsIs();

  wxString precisionChoices[] = { "Float", "Rational" };
  m_precision = new wxRadioBox(this, -1, "Precision", 
			       wxDefaultPosition, wxDefaultSize,
			       2, precisionChoices, 0, wxRA_SPECIFY_ROWS);
  m_precision->SetSelection(precision);
  m_precision->SetConstraints(new wxLayoutConstraints);
  m_precision->GetConstraints()->left.SameAs(m_standardNum, wxRight, 10);
  m_precision->GetConstraints()->top.SameAs(m_standardType, wxTop);
  m_precision->GetConstraints()->width.AsIs();
  m_precision->GetConstraints()->height.AsIs();
  
  m_description = new wxTextCtrl(this, -1);
  m_description->Enable(FALSE);
  m_description->SetConstraints(new wxLayoutConstraints);
  m_description->GetConstraints()->left.SameAs(m_standardType, wxLeft);
  m_description->GetConstraints()->right.SameAs(m_precision, wxRight);
  m_description->GetConstraints()->height.AsIs();
  m_description->GetConstraints()->top.SameAs(m_standardNum, wxBottom, 5);

  m_okButton->GetConstraints()->top.SameAs(m_description, wxBottom, 10);
  m_okButton->GetConstraints()->right.SameAs(m_cancelButton, wxLeft, 10);
  m_okButton->GetConstraints()->width.SameAs(m_cancelButton, wxWidth);
  m_okButton->GetConstraints()->height.AsIs();

  m_cancelButton->GetConstraints()->centreY.SameAs(m_okButton, wxCentreY);
  m_cancelButton->GetConstraints()->centreX.SameAs(m_description, wxCentreX);
  m_cancelButton->GetConstraints()->width.AsIs();
  m_cancelButton->GetConstraints()->height.AsIs();

  m_helpButton->GetConstraints()->centreY.SameAs(m_okButton, wxCentreY);
  m_helpButton->GetConstraints()->left.SameAs(m_cancelButton, wxRight, 10);
  m_helpButton->GetConstraints()->width.SameAs(m_cancelButton, wxWidth);
  m_helpButton->GetConstraints()->height.AsIs();

  OnChange();

  AutoSize();
}

dialogEfgStandard::~dialogEfgStandard()
{
  /*
  if (Completed() == wxOK) {
    gText defaultsFile(gambitApp.ResourceFile());
    wxWriteResource(SOLN_SECT, "Efg-Standard-Type",
		    m_standardType->GetSelection(), defaultsFile);
    wxWriteResource(SOLN_SECT, "Efg-Standard-Num",
		    m_standardNum->GetSelection(), defaultsFile);
    wxWriteResource(SOLN_SECT, "Efg-Standard-Precision",
		    m_precision->GetSelection(), defaultsFile);
  }
  */
}

void dialogEfgStandard::OnChange(void)
{
  switch (m_standardType->GetSelection()) {
  case 0:
  case 1:
    switch (m_standardNum->GetSelection()) {
    case 0:
      if (IsPerfectRecall(m_efg)) {
	if (m_efg.NumPlayers() == 2 && m_efg.IsConstSum()) {
	  m_description->SetValue("LpSolve[EFG]");
	  m_precision->Enable(TRUE);
	}
	else if (m_efg.NumPlayers() == 2) {
	  m_description->SetValue("LcpSolve[EFG]");
	  m_precision->Enable(TRUE);
	}
	else {
	  m_description->SetValue("SimpdivSolve[NFG]");
	  m_precision->SetSelection(0);
	  m_precision->Enable(FALSE);
	}
      }
      else {
	m_description->SetValue("QreSolve[EFG]");
	m_precision->SetSelection(0);
	m_precision->Enable(FALSE);
      }
      break;
    case 1:
      if (m_efg.NumPlayers() == 2) {
	m_description->SetValue("EnumMixedSolve[NFG]");
	m_precision->Enable(TRUE);
      }
      else {
	m_description->SetValue("LiapSolve[EFG]");
	m_precision->SetSelection(0);
	m_precision->Enable(FALSE);
      }
      break;
    case 2:
      if (m_efg.NumPlayers() == 2) {
	m_description->SetValue("EnumMixedSolve[NFG]");
	m_precision->Enable(TRUE);
      }
      else {
	m_description->SetValue("LiapSolve[EFG]");
	m_precision->SetSelection(0);
	m_precision->Enable(FALSE);
      }
      break;
    }
    break;
  case 2:
    switch (m_standardNum->GetSelection()) {
    case 0:
      m_description->SetValue("QreSolve[EFG]");
      m_precision->SetSelection(0);
      m_precision->Enable(FALSE);
      break;
    case 1:
    case 2:
      m_description->SetValue("LiapSolve[EFG]");
      m_precision->SetSelection(0);
      m_precision->Enable(FALSE);
      break;
    }
  }
}

efgStandardType dialogEfgStandard::Type(void) const
{
  switch (m_standardType->GetSelection()) {
  case 0:
    return efgSTANDARD_NASH;
  case 1:
    return efgSTANDARD_PERFECT;
  case 2:
    return efgSTANDARD_SEQUENTIAL;
  default:
    return efgSTANDARD_NASH;
  }
}

efgStandardNum dialogEfgStandard::Number(void) const
{
  switch (m_standardNum->GetSelection()) {
  case 0:
    return efgSTANDARD_ONE;
  case 1:
    return efgSTANDARD_TWO;
  case 2:
    return efgSTANDARD_ALL;
  default:
    return efgSTANDARD_ALL;
  }
}
