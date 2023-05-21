#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ui.h>
#include "../cpank/include/api.h"

typedef struct Io_Struct {
  uiMultilineEntry *input;
  uiMultilineEntry *output;
} Io_Struct;

int onClosing(uiWindow *w, void *data) {
  uiQuit();
  return 1;
}

void onRunButton(uiButton *btn, void *data) {
  Io_Struct *io = (Io_Struct *)data;
  char * result = run_code(uiMultilineEntryText(io->input));
  uiMultilineEntrySetText(io->output, result);
  free(result);
}

int main(int argc, char **argv) {
  uiInitOptions o;
  const char *err;
  uiWindow *w;
  uiBox *edvbox;
  uiBox *btnpanel;
  uiMultilineEntry *input;
  uiMultilineEntry *output;
  uiButton *runbtn;

  memset(&o, 0, sizeof(uiInitOptions));

  err = uiInit(&o);

  if (err != NULL) {
    fprintf(stderr, "Error on uiInit : %s", err);
    uiFreeInitError(err);
    return EXIT_FAILURE;
  }

  w = uiNewWindow("Pankti Editor", 400, 300, 0);
  uiWindowSetMargined(w, 1);

  edvbox = uiNewVerticalBox();
  uiBoxSetPadded(edvbox, false);
  uiWindowSetChild(w, uiControl(edvbox));

  btnpanel = uiNewHorizontalBox();
  uiBoxSetPadded(btnpanel, true);
  input = uiNewMultilineEntry();
  // uiMultilineEntryOnChanged(input, onTextChange, NULL);

  output = uiNewMultilineEntry();
  uiMultilineEntrySetReadOnly(output, true);
  runbtn = uiNewButton("Run");
  Io_Struct io_control = (Io_Struct){.input = input, .output = output};
  uiButtonOnClicked(runbtn, onRunButton, (void *)&io_control);
  uiBoxAppend(btnpanel, uiControl(runbtn), false);
  uiBoxAppend(edvbox, uiControl(btnpanel), false);

  uiBoxAppend(edvbox, uiControl(input), true);
  uiBoxAppend(edvbox, uiControl(output), true);

  uiWindowOnClosing(w, onClosing, NULL);
  uiControlShow(uiControl(w));

  uiMain();

  return EXIT_SUCCESS;
}
