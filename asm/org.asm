    ORG 0
SQ  DATA 0
    LOAD X
    MUL X
    STORE X
    BR  @SQ
X   DATA 3
    ORG 10
    CALL SQ
    HALT
    END 10
