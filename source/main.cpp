#include <ctrcommon/input.hpp>
#include <ctrcommon/gpu.hpp>
#include <ctrcommon/platform.hpp>
#include <ctrcommon/service.hpp>
#include <3ds.h>
#include "constants.h"
#include <sstream>
#include <stdio.h>
#include "patches.h"
#include "kernel11.h"
#include <3ds/services/ns.h>

using namespace std;

#define log(...) fprintf(stderr, __VA_ARGS__)

bool loadTitle(){
  //need to patch it here to allow debug
  //make the changes and exit
  if(!serviceRequire("kernel")){
    return false;
  }
  gputDrawString("khax done!", (gpuGetViewportWidth() - gputGetStringWidth("khax done!", 8)) / 2, 100, 8, 8, 0, 0, 0);

  SaveVersionConstants();
  PatchSrvAccess();
  gputDrawString("srv patched", (gpuGetViewportWidth() - gputGetStringWidth("srv patched", 8)) / 2, 130, 8, 8, 0, 0, 0);

  if(!nsInit()){
    gputDrawString("ns initialized", (gpuGetViewportWidth() - gputGetStringWidth("patch applied!", 8)) / 2, 70, 8, 8,0 ,0, 0);
  }

  NS_RebootToTitle(mediatype_GAMECARD,0);

  nsExit();

  return true;
}

int main(int argc, char **argv) {
  if(!platformInit()) {
    return 0;
  }

  stringstream stream;
  stream << "A - Load title" << "\n";
  stream << "B - Exit" << "\n";
  string str = stream.str();

  gpuViewport(TOP_SCREEN, 0, 0, TOP_WIDTH, TOP_HEIGHT);
  gputOrtho(0, TOP_WIDTH, 0, TOP_HEIGHT, -1, 1);
  gpuClearColor(0xFF, 0xFF, 0xFF, 0xFF);

  while(platformIsRunning()) {
    inputPoll();

    if(inputIsPressed(BUTTON_A)) {
      loadTitle();
    }

    if(inputIsPressed(BUTTON_B)) {
      break;
    }

    const string title = "Ygw free region title loader";
    const string credit = "by felipejfc";
    gpuClear();
    gputDrawString(title, (gpuGetViewportWidth() - gputGetStringWidth(title, 10)) / 2, (gpuGetViewportHeight() - gputGetStringHeight(title, 10))/2, 10, 10, 0, 0, 0);
    gputDrawString(credit, (gpuGetViewportWidth() - gputGetStringWidth(credit, 8)) / 2, (gpuGetViewportHeight() - gputGetStringHeight(credit, 8))/2 - 50, 8, 8, 0, 0, 0);
    gputDrawString(str, (gpuGetViewportWidth() - gputGetStringWidth(str, 8)) / 2, (gpuGetViewportHeight() - gputGetStringHeight(str, 8))/2 - 100, 8, 8, 0, 0, 0);
    gpuFlush();
    gpuFlushBuffer();
    gpuSwapBuffers(true);
  }

  platformCleanup();
  return 0;
}
