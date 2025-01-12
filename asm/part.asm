         ORG 0
START    LOAD   =1 // START = 0
         STORE  LSTFIB //110 
         STORE  FIB //111
         LOAD   =2
         SUB    N //113
         BRGE   DONE //6 
DONE     HALT
         ORG 110
LSTFIB   DATA   0
FIB      DATA   0
N        DATA   3
         END    START