#pragma once

#define RAM_SIZE (16*1024)
#define ROM_CORE_SIZE (24*1024)
#define ROM_FULL_SIZE (128*1024)
#define ROM_PROG_SIZE (ROM_FULL_SIZE - ROM_CORE_SIZE)
#define ROM_ADDRESS_CORE (0x08000000)
#define ROM_ADDRESS_PROG (ROM_ADDRESS_CORE + ROM_CORE_SIZE)

extern unsigned char ram[RAM_SIZE];
