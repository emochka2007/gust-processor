#ifndef CONFIG_H
#define CONFIG_H
// Array lengths
#define MEMORY_LENGTH 1024
#define INSTRUCTION_LENGTH 17
#define ADDRESS_LENGTH 11
#define COMMAND_LENGTH 5
#define ADDRESS_MODE_LENGTH 3
// MODES
extern char DIRECT_MODE[ADDRESS_MODE_LENGTH];
extern char IMMEDIATE_MODE[ADDRESS_MODE_LENGTH];
extern char INDEXED_MODE[ADDRESS_MODE_LENGTH];
extern char INDIRECT_MODE[ADDRESS_MODE_LENGTH];
extern char *BIN_MODES[4];
extern char *MNEMO_MODES[4];
// MEMORY
extern char memory[MEMORY_LENGTH][INSTRUCTION_LENGTH];
extern char IR[INSTRUCTION_LENGTH];
extern char MBR[INSTRUCTION_LENGTH];
extern char PC[ADDRESS_LENGTH];
extern char MAR[ADDRESS_LENGTH];
extern char XR[ADDRESS_LENGTH];
extern char AC[INSTRUCTION_LENGTH];

// COMMANDS
extern char HALT[COMMAND_LENGTH];
extern char LOAD[COMMAND_LENGTH];
extern char STORE[COMMAND_LENGTH];
extern char CALL[COMMAND_LENGTH];
extern char BR[COMMAND_LENGTH];
extern char BREQ[COMMAND_LENGTH];
extern char BRGE[COMMAND_LENGTH];
extern char BRLT[COMMAND_LENGTH];
extern char ADD[COMMAND_LENGTH];
extern char SUB[COMMAND_LENGTH];
extern char MUL[COMMAND_LENGTH];
extern char DIV[COMMAND_LENGTH];
// Parallel arrays
#define COMMANDS_COUNT 12
char *COMMANDS_STR[COMMANDS_COUNT];
char *BIN_COMMANDS_STR[COMMANDS_COUNT];

#endif
void instruction_cycle(void);

void get_bits_from_instruction(int start, int end, char result[]);
