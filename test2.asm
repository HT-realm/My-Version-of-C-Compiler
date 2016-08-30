ASSUME CS:code segment,DS: data segment
datasg segment
    A DB '?'
    tmpB DB '?'
datasg ends
codesg segment
  start:
    mov AX,datasg
    mov DS,AX
    mov AL,A
    CMP AL,1
    JG stepB
    JMP stepC
  stepB:
    mov BL,A
    add BL,1
    mov tmpB,BL
    mov BL,tmpB
    mov A,BL
    JMP stepC
  stepC:
    mov ax,4C00H
    int 21H
codesg ends
  end start
