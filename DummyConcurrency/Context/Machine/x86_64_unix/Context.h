#pragma once

void* SetupMachineContext(void* stack, void* trampoline, void* arg);

void SwitchMachineContext(void** from_rsp, void** to_rsp);
