//---------------------------------------------------------------------------

#ifndef FLoginH
#define FLoginH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include <Graphics.hpp>
#include <jpeg.hpp>
#include <DB.hpp>
#include <DBTables.hpp>

#include "wrates.h"
//---------------------------------------------------------------------------
class TfrmLogin : public TForm {
__published:   // IDE-managed Components
  TButton *btnClose;
  TButton *btnConnect;
  TComboBox *comboLang;
  TEdit *slePassword;
  TEdit *sleUsrId;
  TImage *Image1;
  TLabel *lblPassword;
  TLabel *lblUsrId;
  TTable *Table1;
  TTable *Table2;
  void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
  void __fastcall btnCloseClick(TObject *Sender);
  void __fastcall btnConnectClick(TObject *Sender);
  void __fastcall comboLangChange(TObject *Sender);
private: // User declarations
public:     // User declarations
  void SetLegends();
  __fastcall TfrmLogin(TComponent *Owner);
  CUsr Usr;
};
//---------------------------------------------------------------------------
extern PACKAGE TfrmLogin *frmLogin;
//---------------------------------------------------------------------------
#endif
