#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../cpank/ext/iup/include/iup.h"
#include "../cpank/include/api.h"

int main(int argc , char ** argv){
  
  Ihandle * dlg , *label;

  IupOpen(&argc , &argv);

  label = IupLabel("Hello World");

  dlg = IupDialog(IupVbox(label , NULL));
  IupSetAttribute(dlg, "TITLE" , "Hello World Title");
  IupShowXY(dlg, IUP_CENTER, IUP_CENTER);
  IupMainLoop();
  IupClose();

  return EXIT_SUCCESS;

}

