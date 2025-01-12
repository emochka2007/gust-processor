        ORG     0
START   LOAD    =1
        STORE   LSTFIB
        STORE   FIB
        LOAD    =2
        SUB     N
        BRGE    DONE
LOOP    STORE   1
        LOAD    FIB
        STORE   TEMP
        ADD     LSTFIB
        STORE   FIB
        LOAD    TEMP
        STORE   LSTFIB
        LOAD    I
        ADD     =1
        BRLT    LOOP
DONE    HALT
LSTFIB  DATA    1
FIB     DATA    1
TEMP    DATA    1
I       DATA    1
N       DATA    5
        END     START