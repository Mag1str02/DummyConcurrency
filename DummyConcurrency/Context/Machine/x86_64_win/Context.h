#pragma once

void* SetupMachineContext(void* stack_top, void* stack_bottom, void* trampoline, void* arg);

void SwitchMachineContext(void** from_rsp, void** to_rsp);
