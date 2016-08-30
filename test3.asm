ASSUME CS:code segment,DS: data segment
datasg segment
    x DB '?'
    y DB '?'
    z DB '?'
    a DB '?'
    tmpB DB '?'
    tmpC DB '?'
    tmpD DB '?'
    tmpE DB 'x=$'
    tmpF DB ' y=$'
    tmpG DB ' z=$'
    tmpH DB ' a=$'
    tmpI DB '?'
    tmpJ DB ' d=$'
datasg ends
codesg segment
  start:
    mov AX,datasg
    mov DS,AX
    mov BL,1
    mov x,BL
    mov BL,x
    add BL,4
    mov tmpB,BL
    mov BL,tmpB
    mov y,BL
    mov AL,x
    mov BH,y
    mul BH
    mov BL,1
    div BL
    mov tmpC,AL
    mov BL,tmpC
    mov a,BL
    mov BL,y
    sub BL,2
    mov tmpD,BL
    mov BL,tmpD
    mov z,BL
    LEA DX,tmpE
    mov AH,09
    int 21H
    mov AL,x
    CBW
    mov BL,10
    DIV BL
    mov BH,AH
    add BH,30H
    add AL,30H
    CMP AL,48
    JE stepC
  stepB:
    mov DL,AL
    mov AH,2
    int 21H
  stepC:
    mov DL,BH
    mov AH,2
    int 21H
    LEA DX,tmpF
    mov AH,09
    int 21H
    mov AL,y
    CBW
    mov BL,10
    DIV BL
    mov BH,AH
    add BH,30H
    add AL,30H
    CMP AL,48
    JE stepE
  stepD:
    mov DL,AL
    mov AH,2
    int 21H
  stepE:
    mov DL,BH
    mov AH,2
    int 21H
    LEA DX,tmpG
    mov AH,09
    int 21H
    mov AL,z
    CBW
    mov BL,10
    DIV BL
    mov BH,AH
    add BH,30H
    add AL,30H
    CMP AL,48
    JE stepG
  stepF:
    mov DL,AL
    mov AH,2
    int 21H
  stepG:
    mov DL,BH
    mov AH,2
    int 21H
    LEA DX,tmpH
    mov AH,09
    int 21H
    mov AL,a
    CBW
    mov BL,10
    DIV BL
    mov BH,AH
    add BH,30H
    add AL,30H
    CMP AL,48
    JE stepI
  stepH:
    mov DL,AL
    mov AH,2
    int 21H
  stepI:
    mov DL,BH
    mov AH,2
    int 21H
    mov AL,y
    CBW
    mov BL,5
    div BL
    mov tmpI,AL
    mov BL,tmpI
    mov a,BL
    LEA DX,tmpJ
    mov AH,09
    int 21H
    mov AL,d
    CBW
    mov BL,10
    DIV BL
    mov BH,AH
    add BH,30H
    add AL,30H
    CMP AL,48
    JE stepK
  stepJ:
    mov DL,AL
    mov AH,2
    int 21H
  stepK:
    mov DL,BH
    mov AH,2
    int 21H
    mov ax,4C00H
    int 21H
codesg ends
  end start
