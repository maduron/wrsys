//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include <fstream>

#include "uorg.h"
#include "flogin.h"
#include "dmod1.h"
#include "fprojsel.h"
#include "lgndsdef.h"

using namespace std;
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TfrmLogin *frmLogin;
//---------------------------------------------------------------------------
__fastcall TfrmLogin::TfrmLogin(TComponent *Owner) : TForm(Owner) {
    /*
    // I copied usr1.dbf to usr.dbf. With this code I append usr2.dbf... Files sent from MÈrida
    Table1->DatabaseName="C:\\WRatesDB\\ConsMID\\";
    Table1->TableName="usr.dbf";
    Table1->Exclusive=true;
    Table2->DatabaseName="C:\\WRatesDB\\ConsMID\\";
    Table2->TableName="usr2.dbf";
    Table2->Exclusive=true;
    Table1->BatchMove(Table2, batAppend);
    */

    comboLang->Text = "English";
    comboLang->Items->Add("English");
    comboLang->Items->Add("EspaÒol");
    comboLang->Items->Add("FranÁais");
    comboLangChange(this);

    Usr.Logo = new TJPEGImage();
    Usr.Logo->LoadFromFile("Logo.jpg");

    Usr.Icon = new TIcon();
    Usr.Icon->LoadFromFile("Icon.ico");
    this->Icon = Usr.Icon;
}
//---------------------------------------------------------------------------
void TfrmLogin::SetLegends() {
    this->Caption = this->Name + " " + LgndLogin;
    btnClose->Caption = LgndBtnClose;
    btnConnect->Caption = LgndBtnConnect;
    lblUsrId->Caption = LgndLblUsrId;
    lblPassword->Caption = LgndLblPassword;
}
//---------------------------------------------------------------------------
#pragma argsused
void __fastcall TfrmLogin::btnCloseClick(TObject *Sender) {
    Close();
}
//---------------------------------------------------------------------------
#pragma argsused
void __fastcall TfrmLogin::btnConnectClick(TObject *Sender) {
    sleUsrId->Text = "usr"; // This is just until I implement a real password authentication.
    slePassword->Text = "RateSys";

    sleUsrId->Text = sleUsrId->Text.Trim();
    if(sleUsrId->Text.IsEmpty()) {
        Application->MessageBox(LgndUserIdReq, LgndErrorConnecting, MB_OK | MB_ICONERROR);
        sleUsrId->SetFocus();
        return;
    }

    slePassword->Text = slePassword->Text.Trim();
    if(slePassword->Text.IsEmpty()) {
        Application->MessageBox(LgndPasswordReq, LgndErrorConnecting, MB_OK | MB_ICONERROR);
        slePassword->SetFocus();
        return;
    }

    string usrId = sleUsrId->Text.Trim().c_str();
    string password = slePassword->Text.c_str();

    /*
    DM1->Database1->DriverName="Interbase";
    //DM1->Database1->Params->Add("SERVER NAME=C:\\WRatesDB\\WRATESSY.GDB");
    DM1->Database1->Params->Add("WAIT ON LOCKS=FALSE");
    DM1->Database1->Params->Add("USER_NAME=SYSDBA");
    DM1->Database1->Params->Add("PASSWORD=masterkey");
    DM1->Database1->ConnectionName="C:\\WRatesDB\\WRATESSY.GDB";
    DM1->Database1->LoginPrompt=false;
    */

    try {
        DM1->Database1->Connected = true;
        DM1->Query1->SQLConnection = DM1->Database1;
    } catch (Exception &ex) {
        MessageDlg(ex.Message, mtError, TMsgDlgButtons() << mbOK, 0);
        return;
    }

    //DM1->Query1->DatabaseName=DM1->Database1->DatabaseName;
    //DM1->Query1->SQLConnection=DM1->Database1;
    //DM1->Database1->StartTransaction();
    if(!DM1->LoadUsr(usrId, Usr)) {
        //   DM1->Database1->Commit();
        Application->MessageBox(LgndNonRegUser, LgndErrorConnecting, MB_OK | MB_ICONERROR);
        sleUsrId->SetFocus();
        DM1->Database1->Connected = false;
        return;
    }
    //DM1->Database1->Commit();
    if(password != Usr.UsrPassword) {
        Application->MessageBox(LgndPasswordInvalid, LgndErrorConnecting, MB_OK | MB_ICONERROR);
        slePassword->SetFocus();
        DM1->Database1->Connected = false;
        return;
    }
    DM1->UsrId = usrId;       // Subsequent Data Module functions will use it as a parameter
    //DM1->BuildBimDistrsFromSample(); // One time trick to build two-month consumption distr
    slePassword->Text = "";
    TfrmProjSel *frmProjSel = new TfrmProjSel(Usr, this);
    frmProjSel->ShowModal();
    delete frmProjSel;
    TFOrganizer *frmOrganizer = new TFOrganizer(Usr, this);
    frmOrganizer->ShowModal();
    delete frmOrganizer;
    DM1->Database1->Connected = false;
}
//---------------------------------------------------------------------------
#pragma argsused
void __fastcall TfrmLogin::comboLangChange(TObject *Sender) {
    if(comboLang->Text == "EspaÒol")
        LgndsDef('S');
    else if(comboLang->Text == "English")
        LgndsDef('E');
    else if(comboLang->Text == "FranÁais")
        LgndsDef('F');
    SetLegends();
}
//---------------------------------------------------------------------------
#pragma argsused
void __fastcall TfrmLogin::FormClose(TObject *Sender, TCloseAction &Action) {
    delete frmProjSel;
    delete Usr.Logo;
    delete Usr.Icon;
}
//---------------------------------------------------------------------------
/* Carga tablas ConsSamples y ConsSampleVals a partir de un archivo dbf, muestra MÈrida, Yucatç
n, MÈxico
// Pendiente de corregir formato de ConsSampleVals
#include <fstream>
using namespace std;

string now, yyyymmdd, hhmmss;
datetime(yyyymmdd, hhmmss);
now=yyyymmdd+hhmmss;

string consSampleId="MEXYUCMID";
CConsSample consSample(usrId, consSampleId,
                  "ESTADÕSTICAS DE FACTURACI”N",
                  "M…XICO", "YUCAT¡N", "M…RIDA",
                  "METROS C⁄BICOS",
                  now, now);
DM1->Database1->StartTransaction();
DM1->StoreConsSample(consSample);
DM1->Database1->Commit();

Table1->DatabaseName="C:\\WRatesDB\\ConsMID\\";
Table1->TableName="usr.dbf";
Table1->Exclusive=true;
TIndexOptions Options;
Options << ixExpression;
Table1->DeleteIndex("idxContr");
Table1->AddIndex("idxContr", "Zona+Contrato", Options, "");
Table1->Open();
Table1->IndexName="idxContr";
//Table1->Filter="Status='A'";
//Table1->Filtered=true;->AsString.c_str()
ofstream errs("c:\\errs.txt");
Table1->First();
AnsiString zonaAnt("");
AnsiString contrAnt("");
AnsiString tarAnt("");
while(!Table1->Eof)
   {
    string consUsrType=Table1->FieldByName("CveTarifa")->AsString.c_str();
    if(consUsrType=="1")
       consUsrType="D";
    else if(consUsrType=="2")
       consUsrType="C";
    else if(consUsrType=="3")
       consUsrType="G";
    else if(consUsrType=="4")
       consUsrType="H";
    else
       consUsrType="?";
   if((contrAnt==Table1->FieldByName("Contrato")->AsString && zonaAnt==Table1->FieldByName("Zona")->AsString) || consUsrType=="?")
      {
      errs << "__________________________" << endl;
      errs << zonaAnt.c_str() << "   " << contrAnt.c_str() << "   " << tarAnt.c_str() << endl;
      errs << Table1->FieldByName("Zona")->AsString.c_str()
            << "   "
            << Table1->FieldByName("Contrato")->AsString.c_str()
            << "   "
            << Table1->FieldByName("CveTarifa")->AsString.c_str() << endl;
      }
   else
      {
      std:string consUsrId=Table1->FieldByName("Zona")->AsString.c_str();
      consUsrId+=Table1->FieldByName("Contrato")->AsString.c_str();

      AnsiString baseCons=Table1->FieldByName("BaseCons11")->AsString.Trim();
      if(baseCons.Length())
         {
         string consType=baseCons.c_str();
         if(consType=="L")
            consType="M";
         else if(consType=="P")
            consType="A";
         else
            consType="E";
         long cons=Table1->FieldByName("Cons11")->AsInteger;
         CConsSampleVal v(usrId, consSampleId, consUsrId,"200411", consUsrType, consType, cons);
         DM1->StoreConsSampleVal(v);
         }
      baseCons=Table1->FieldByName("BaseCons12")->AsString.Trim();
      if(baseCons.Length())
         {
         string consType=baseCons.c_str();
         if(consType=="L")
            consType="M";
         else if(consType=="P")
            consType="A";
         else
            consType="E";
         long cons=Table1->FieldByName("Cons12")->AsInteger;
         CConsSampleVal v(usrId, consSampleId, consUsrId,"200412", consUsrType, consType, cons);
         DM1->StoreConsSampleVal(v);
         }
      baseCons=Table1->FieldByName("BaseCons01")->AsString.Trim();
      if(baseCons.Length())
         {
         string consType=baseCons.c_str();
         if(consType=="L")
            consType="M";
         else if(consType=="P")
            consType="A";
         else
            consType="E";
         long cons=Table1->FieldByName("Cons01")->AsInteger;
         CConsSampleVal v(usrId, consSampleId, consUsrId,"200501", consUsrType, consType, cons);
         DM1->StoreConsSampleVal(v);
         }
      baseCons=Table1->FieldByName("BaseCons02")->AsString.Trim();
      if(baseCons.Length())
         {
         string consType=baseCons.c_str();
         if(consType=="L")
            consType="M";
         else if(consType=="P")
            consType="A";
         else
            consType="E";
         long cons=Table1->FieldByName("Cons02")->AsInteger;
         CConsSampleVal v(usrId, consSampleId, consUsrId,"200502", consUsrType, consType, cons);
         DM1->StoreConsSampleVal(v);
         }
      baseCons=Table1->FieldByName("BaseCons03")->AsString.Trim();
      if(baseCons.Length())
         {
         string consType=baseCons.c_str();
         if(consType=="L")
            consType="M";
         else if(consType=="P")
            consType="A";
         else
            consType="E";
         long cons=Table1->FieldByName("Cons03")->AsInteger;
         CConsSampleVal v(usrId, consSampleId, consUsrId,"200503", consUsrType, consType, cons);
         DM1->StoreConsSampleVal(v);
         }
      baseCons=Table1->FieldByName("BaseCons04")->AsString.Trim();
      if(baseCons.Length())
         {
         string consType=baseCons.c_str();
         if(consType=="L")
            consType="M";
         else if(consType=="P")
            consType="A";
         else
            consType="E";
         long cons=Table1->FieldByName("Cons04")->AsInteger;
         CConsSampleVal v(usrId, consSampleId, consUsrId,"200504", consUsrType, consType, cons);
         DM1->StoreConsSampleVal(v);
         }
      baseCons=Table1->FieldByName("BaseCons05")->AsString.Trim();
      if(baseCons.Length())
         {
         string consType=baseCons.c_str();
         if(consType=="L")
            consType="M";
         else if(consType=="P")
            consType="A";
         else
            consType="E";
         long cons=Table1->FieldByName("Cons05")->AsInteger;
         CConsSampleVal v(usrId, consSampleId, consUsrId,"200505", consUsrType, consType, cons);
         DM1->StoreConsSampleVal(v);
         }
      baseCons=Table1->FieldByName("BaseCons06")->AsString.Trim();
      if(baseCons.Length())
         {
         string consType=baseCons.c_str();
         if(consType=="L")
            consType="M";
         else if(consType=="P")
            consType="A";
         else
            consType="E";
         long cons=Table1->FieldByName("Cons06")->AsInteger;
         CConsSampleVal v(usrId, consSampleId, consUsrId,"200506", consUsrType, consType, cons);
         DM1->StoreConsSampleVal(v);
         }
      baseCons=Table1->FieldByName("BaseCons07")->AsString.Trim();
      if(baseCons.Length())
         {
         string consType=baseCons.c_str();
         if(consType=="L")
            consType="M";
         else if(consType=="P")
            consType="A";
         else
            consType="E";
         long cons=Table1->FieldByName("Cons07")->AsInteger;
         CConsSampleVal v(usrId, consSampleId, consUsrId,"200507", consUsrType, consType, cons);
         DM1->StoreConsSampleVal(v);
         }
      baseCons=Table1->FieldByName("BaseCons08")->AsString.Trim();
      if(baseCons.Length())
         {
         string consType=baseCons.c_str();
         if(consType=="L")
            consType="M";
         else if(consType=="P")
            consType="A";
         else
            consType="E";
         long cons=Table1->FieldByName("Cons08")->AsInteger;
         CConsSampleVal v(usrId, consSampleId, consUsrId,"200508", consUsrType, consType, cons);
         DM1->StoreConsSampleVal(v);
         }
      baseCons=Table1->FieldByName("BaseCons09")->AsString.Trim();
      if(baseCons.Length())
         {
         string consType=baseCons.c_str();
         if(consType=="L")
            consType="M";
         else if(consType=="P")
            consType="A";
         else
            consType="E";
         long cons=Table1->FieldByName("Cons09")->AsInteger;
         CConsSampleVal v(usrId, consSampleId, consUsrId,"200509", consUsrType, consType, cons);
         DM1->StoreConsSampleVal(v);
         }
      baseCons=Table1->FieldByName("BaseCons10")->AsString.Trim();
      if(baseCons.Length())
         {
         string consType=baseCons.c_str();
         if(consType=="L")
            consType="M";
         else if(consType=="P")
            consType="A";
         else
            consType="E";
         long cons=Table1->FieldByName("Cons10")->AsInteger;
         CConsSampleVal v(usrId, consSampleId, consUsrId,"200510", consUsrType, consType, cons);
         DM1->StoreConsSampleVal(v);
         }
      }
   zonaAnt=Table1->FieldByName("Zona")->AsString;
   contrAnt=Table1->FieldByName("Contrato")->AsString;
   tarAnt=Table1->FieldByName("CveTarifa")->AsString;
   Table1->Next();
   }
*/
//---------------------------------------------------------------------------
/*
// Carga tablas ConsSamples y ConsSampleFreqs a partir de un archivo dbf, muestra MÈrida, Yucatç
n, MÈxico
// Este proceso es el preferido para cargar datos pues los carga pormenorizados pero sumarizados en distribuiciones

#include <fstream>
using namespace std;

string now, yyyymmdd, hhmmss;
datetime(yyyymmdd, hhmmss);
now=yyyymmdd+hhmmss;

string consSampleId="MEXYUCMID";
CConsSample consSample(usrId, consSampleId,
                       "ESTADÕSTICAS DE FACTURACI”N",
                       "M…XICO", "YUCAT¡N", "M…RIDA",
                       "METROS C⁄BICOS",
                       now, now);
DM1->Database1->StartTransaction();
DM1->StoreConsSample(consSample);
DM1->Database1->Commit();

Table1->DatabaseName="C:\\WRatesDB\\ConsMID\\";
Table1->TableName="usr.dbf";
Table1->Exclusive=true;
TIndexOptions Options;
Options << ixExpression;
Table1->DeleteIndex("idxContr");
Table1->AddIndex("idxContr", "Zona+Contrato", Options, "");
Table1->Open();
Table1->IndexName="idxContr";
//Table1->Filter="Status='A'";
//Table1->Filtered=true;->AsString.c_str()
ofstream dupls("c:\\dupl.txt");
vector<CConSampleFreqs> freqs;
vector<CConSampleVals> vals;
AnsiString contrAnt("");
AnsiString tarAnt("");
while(!Table1->Eof)
   {
   if(contrAnt==Table1->FieldByName("Contrato")->AsString && zonaAnt==Table1->FieldByName("Zona")->AsString)
      {
      dupls << "__________________________" << endl;
      dupls << zonaAnt.c_str() << "   " << contrAnt.c_str() << "   " << tarAnt.c_str() << endl;
      dupls << Table1->FieldByName("Zona")->AsString.c_str()
            << "   "
            << Table1->FieldByName("Contrato")->AsString.c_str()
            << "   "
            << Table1->FieldByName("CveTarifa")->AsString.c_str() << endl;
      }
   else
      {
      std:string consUsrId=Table1->FieldByName("Zona")->AsString.c_str();
      consUsrId+=Table1->FieldByName("Contrato")->AsString.c_str();
      string consUsrType=Table1->FieldByName("CveTarifa")->AsString.c_str();
      if(consUsrType=="1")
         consUsrType="D";
      else if(consUsrType=="2")
         consUsrType="C";
      else if(consUsrType=="3")
         consUsrType="G";
      else if(consUsrType=="4")
         consUsrType="H";

      AnsiString baseCons=Table1->FieldByName("BaseCons11")->AsString.Trim();
      if(baseCons.Length())
         {
         string consType=baseCons.c_str();
         if(consType=="L")
            consType="M";
         else if(consType=="P")
            consType="A";
         else
            consType="E";
         long cons=Table1->FieldByName("Cons11")->AsInteger;
         CConsSampleVal v(usrId, consSampleId, consUsrId,"200411", consUsrType, consType, cons);
         DM1->StoreConsSampleVal(v);
         }
      baseCons=Table1->FieldByName("BaseCons12")->AsString.Trim();
      if(baseCons.Length())
         {
         string consType=baseCons.c_str();
         if(consType=="L")
            consType="M";
         else if(consType=="P")
            consType="A";
         else
            consType="E";
         long cons=Table1->FieldByName("Cons12")->AsInteger;
         CConsSampleVal v(usrId, consSampleId, consUsrId,"200412", consUsrType, consType, cons);
         DM1->StoreConsSampleVal(v);
         }
      baseCons=Table1->FieldByName("BaseCons01")->AsString.Trim();
      if(baseCons.Length())
         {
         string consType=baseCons.c_str();
         if(consType=="L")
            consType="M";
         else if(consType=="P")
            consType="A";
         else
            consType="E";
         long cons=Table1->FieldByName("Cons01")->AsInteger;
         CConsSampleVal v(usrId, consSampleId, consUsrId,"200501", consUsrType, consType, cons);
         DM1->StoreConsSampleVal(v);
         }
      baseCons=Table1->FieldByName("BaseCons02")->AsString.Trim();
      if(baseCons.Length())
         {
         string consType=baseCons.c_str();
         if(consType=="L")
            consType="M";
         else if(consType=="P")
            consType="A";
         else
            consType="E";
         long cons=Table1->FieldByName("Cons02")->AsInteger;
         CConsSampleVal v(usrId, consSampleId, consUsrId,"200502", consUsrType, consType, cons);
         DM1->StoreConsSampleVal(v);
         }
      baseCons=Table1->FieldByName("BaseCons03")->AsString.Trim();
      if(baseCons.Length())
         {
         string consType=baseCons.c_str();
         if(consType=="L")
            consType="M";
         else if(consType=="P")
            consType="A";
         else
            consType="E";
         long cons=Table1->FieldByName("Cons03")->AsInteger;
         CConsSampleVal v(usrId, consSampleId, consUsrId,"200503", consUsrType, consType, cons);
         DM1->StoreConsSampleVal(v);
         }
      baseCons=Table1->FieldByName("BaseCons04")->AsString.Trim();
      if(baseCons.Length())
         {
         string consType=baseCons.c_str();
         if(consType=="L")
            consType="M";
         else if(consType=="P")
            consType="A";
         else
            consType="E";
         long cons=Table1->FieldByName("Cons04")->AsInteger;
         CConsSampleVal v(usrId, consSampleId, consUsrId,"200504", consUsrType, consType, cons);
         DM1->StoreConsSampleVal(v);
         }
      baseCons=Table1->FieldByName("BaseCons05")->AsString.Trim();
      if(baseCons.Length())
         {
         string consType=baseCons.c_str();
         if(consType=="L")
            consType="M";
         else if(consType=="P")
            consType="A";
         else
            consType="E";
         long cons=Table1->FieldByName("Cons05")->AsInteger;
         CConsSampleVal v(usrId, consSampleId, consUsrId,"200505", consUsrType, consType, cons);
         DM1->StoreConsSampleVal(v);
         }
      baseCons=Table1->FieldByName("BaseCons06")->AsString.Trim();
      if(baseCons.Length())
         {
         string consType=baseCons.c_str();
         if(consType=="L")
            consType="M";
         else if(consType=="P")
            consType="A";
         else
            consType="E";
         long cons=Table1->FieldByName("Cons06")->AsInteger;
         CConsSampleVal v(usrId, consSampleId, consUsrId,"200506", consUsrType, consType, cons);
         DM1->StoreConsSampleVal(v);
         }
      baseCons=Table1->FieldByName("BaseCons07")->AsString.Trim();
      if(baseCons.Length())
         {
         string consType=baseCons.c_str();
         if(consType=="L")
            consType="M";
         else if(consType=="P")
            consType="A";
         else
            consType="E";
         long cons=Table1->FieldByName("Cons07")->AsInteger;
         CConsSampleVal v(usrId, consSampleId, consUsrId,"200507", consUsrType, consType, cons);
         DM1->StoreConsSampleVal(v);
         }
      baseCons=Table1->FieldByName("BaseCons08")->AsString.Trim();
      if(baseCons.Length())
         {
         string consType=baseCons.c_str();
         if(consType=="L")
            consType="M";
         else if(consType=="P")
            consType="A";
         else
            consType="E";
         long cons=Table1->FieldByName("Cons08")->AsInteger;
         CConsSampleVal v(usrId, consSampleId, consUsrId,"200508", consUsrType, consType, cons);
         DM1->StoreConsSampleVal(v);
         }
      baseCons=Table1->FieldByName("BaseCons09")->AsString.Trim();
      if(baseCons.Length())
         {
         string consType=baseCons.c_str();
         if(consType=="L")
            consType="M";
         else if(consType=="P")
            consType="A";
         else
            consType="E";
         long cons=Table1->FieldByName("Cons09")->AsInteger;
         CConsSampleVal v(usrId, consSampleId, consUsrId,"200509", consUsrType, consType, cons);
         DM1->StoreConsSampleVal(v);
         }
      baseCons=Table1->FieldByName("BaseCons10")->AsString.Trim();
      if(baseCons.Length())
         {
         string consType=baseCons.c_str();
         if(consType=="L")
            consType="M";
         else if(consType=="P")
            consType="A";
         else
            consType="E";
         long cons=Table1->FieldByName("Cons10")->AsInteger;
         CConsSampleVal v(usrId, consSampleId, consUsrId,"200510", consUsrType, consType, cons);
         DM1->StoreConsSampleVal(v);
         }
      }
   zonaAnt=Table1->FieldByName("Zona")->AsString;
   contrAnt=Table1->FieldByName("Contrato")->AsString;
   tarAnt=Table1->FieldByName("CveTarifa")->AsString;
   Table1->Next();
   }
*/

