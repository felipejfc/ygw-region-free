#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include <3ds.h>

#include "constants.h"
#include "patches.h"
#include "kernel11.h"
#include "kobjects.h"

//-----------------------------------------------------------------------------

u32 self_pid = 0;

int PatchPid()
{
    *(u32*)(curr_kproc_addr + kproc_pid_offset) = 0;
    return 0;
}

int UnpatchPid()
{
    *(u32*)(curr_kproc_addr + kproc_pid_offset) = self_pid;
    return 0;
}

void ReinitSrv()
{
    srvExit();
    srvInit();
}

void PatchSrvAccess()
{
    svcGetProcessId(&self_pid, 0xFFFF8001);
    printf("Current process id: %lu\n", self_pid);

    printf("Patching srv access...");
    KernelBackdoor(PatchPid);
    ReinitSrv();

    u32 new_pid;
    svcGetProcessId(&new_pid, 0xFFFF8001);
    printf("%s\n", new_pid == 0 ? "succeeded!" : "failed!");

    // Cleanup; won't take effect until srv is reinitialized
    KernelBackdoor(UnpatchPid);
}

//-----------------------------------------------------------------------------

int PatchProcess()
{
    // Target title id
    static const char * title_id = "nim";
    static const u32 patch_offset = 0x0000DD28;
    static const char patch_code[] = { 0x00, 0x20, 0x08, 0x60, 0x70, 0x47 };

    KCodeSet* code_set = FindTitleCodeSet(title_id,3);
    if (code_set == nullptr)
        return 1;

    void * destination = (u64*)FindCodeOffsetKAddr(code_set, patch_offset);

    memcpy(destination, patch_code, sizeof(patch_code));
    return 0;
}
