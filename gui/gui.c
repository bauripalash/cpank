#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../cpank/ext/iup/include/iup.h"
#include "../cpank/include/api.h"



int run_btn_clicked(Ihandle * self){
  Ihandle * editor = IupGetHandle("editor");
  Ihandle * output = IupGetHandle("output");
  char * value = IupGetAttribute(editor, "VALUE");
  char * result = run_code(value);
  IupSetAttribute(output, "VALUE", result);
  free(result);
  return IUP_DEFAULT;
}


int main(int argc , char ** argv){
  
  Ihandle * dlg , *runbutton , *editor , *output , *vbox;

  IupOpen(&argc , &argv);
  IupSetGlobal("UTF8MODE", "Yes");
  
  runbutton = IupButton("Run", NULL);
  IupSetCallback(runbutton, "ACTION", (Icallback)run_btn_clicked);
  editor = IupText(NULL);
  IupSetHandle("editor", editor);
  output = IupText(NULL);
  IupSetHandle("output", output);
  
  IupSetAttributes(output, "READONLY=YES, MULTILINE=YES, EXPAND=YES" );
  vbox = IupVbox(runbutton , editor , output , NULL);
  IupSetAttributes(editor, "MULTILINE=YES, EXPAND=YES" );
  dlg = IupDialog(vbox);
  IupSetAttribute(dlg, "SIZE" , "QUARTERxQUARTER");
  IupSetAttribute(dlg, "TITLE" , "Pankti Editor");
  IupSetAttribute(dlg, "USERSIZE" , NULL);

  IupShowXY(dlg, IUP_CENTER , IUP_CENTER);
  
  IupMainLoop();
  IupClose();

  return EXIT_SUCCESS;

}

