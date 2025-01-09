         ORG    100
START    LOAD   =1
         STORE  LSTFIB
         STORE  FIB
         ORG    103
         LOAD   =2
         SUB    N
         BRGE   DONE
DONE     HALT
LSTFIB   1
FIB      1
N        DATA   3
         END    START