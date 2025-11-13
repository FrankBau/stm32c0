redirect printf to uart

caveats:

- depending on your terminal prog settings, `printf("Hello, world.\r\n");` is needded to properly align the ouput
- printf rather unexpectedly uses the heap (malloc) for allocating a line buffer.
  This can be avoided by adding `setbuf(stdout, NULL);` which removes the need for a line buffer and the call to malloc (check with a breakpoint in `_sbrk`)
- (observed on STM32L4) printf rather unexpectedly uses the FPU even if no floating point parameter is given.
  This is not an issue on Arm Cortex-M0+ which does not feature a FPU. Otherwise, FPU inititalization must be done early. 