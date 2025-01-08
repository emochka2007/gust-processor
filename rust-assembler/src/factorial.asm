ORG 0
 LOAD 20
  BREQ H
  MUL Y
  STORE Y
  LOAD 20
  SUB =1
  STORE 20
  BRGE 0
H HALT
 ORG 20
DATA 5
Y DATA 1
 END 0
[("0", "LOAD", "=", "1"), ("", "STORE", "", "3"), ("0", "HALT", "", "")]
[[("0", "LOAD", "=", "1"), ("", "STORE", "", "3"), ("0", "HALT", "", "")]]