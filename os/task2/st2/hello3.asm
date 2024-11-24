global _start
 
section .text
_start:
    ;@ mov rdi, 22     ; в RDI код статуса результата
    ;@ mov rax, 60     ; в RAX номер системной функции
    ;@ syscall         ; выполняем системную функцию
    MOV    AH,  13h             ;Номер функции 13h
    MOV    AL,  1               ;Перевести курсор в конец строки
    MOV    CX,  15              ;Длина строки
    MOV    BL,  00011110b       ;Жёлтый текст на синем фоне
    MOV    DL,  5               ;Координата Х
    MOV    DH,  2               ;Координата Y
    MOV    BP,  offset stroka   ;Адрес строки записываем в DX
    INT    10h
    RET                         ;завершение СОМ-файла	

stroka   DB	'Hello, World!!!' ;Строка для вывода	
  END    start