; ====================================================================
; AsmMathLib - Assembly Math Library
; Copyright (C) 2021 Manolo789 -- see doc/LICENSE.TXT
;
; ====================================================================

; --------------------------------------------------------------------
; addsub -- Fixed point addition or subtraction
; IN: AX = number_1
; IN: BX = number_2
; IN: CX = scale factor of number_1
; IN: DX = scale factor of number_2
; IN: SI = sign(0 = + OR 1 = -) of number_1
; IN: DI = sign(0 = + OR 1 = -) of number_2
; OUT: AX = add or sub result
; OUT: CX = final scale factor
; OUT: SI = final sign(0 = + OR 1 = -)

addsub:
	; ---INPUT---
	pusha
	mov word [.temp_1], ax		
	mov word [.temp_2], bx		
	mov ax, cx			
	mov bx, dx			
	; ---CALC SCALE FACTOR---
	mov word [.temp_3], ax		
	mov word [.temp_4], bx		
	mul word [.temp_2]			; AX * .temp_2 = AX
	mov word [.temp_2], ax		
	mov ax, bx			
	mul word [.temp_1]			; AX * .temp_1 = AX
	mov word [.temp_1], ax		
	mov word ax, [.temp_3]		
	mov word bx, [.temp_4]		
	mul bx						; AX * BX = AX(resulting scale factor)
	mov word [.temp_3], ax		
								; .temp_1 = scale factor of number_2 * number_1
								; .temp_2 = scale factor of number_1 * number_2
								; .temp_3 = scale factor of number_1 * scale factor of number_2
	; ---VERIFY IF IT IS ADDITION OR SUBTRACT---
	xor dx, dx					; clean DX
	cmp si, di
	jz .addition				; If SI equals DI skip to .addition
	jmp .subtraction			; but if not skip to .subtraction

.addition:
	; ---VERIFY IF NUMBER_1 IS GREATER, SMALLER OR EQUAL TO NUMBER_2---
	mov word ax, [.temp_1]		
	mov word bx, [.temp_2]		
	cmp ax, bx
	jle .addlebx				; If AX is less than or equal to BX skip to .addlebx
	jmp .addgbx					; but if not skip to .addgbx

.addlebx:
	; ---CALC ADDITION---
	add bx, ax					; BX add AX = BX(addition result)
	mov word [.temp_1], bx		
	jmp .result					; jump for .result
	
.addgbx:
	; ---CALC ADDITION---
	add ax, bx					; AX add BX = AX(addition result)
	mov word [.temp_1], ax		

.result:
	; ---RESULT---
	mov word [.temp_2], si		
	popa
	mov word ax, [.temp_1]		
	mov word si, [.temp_2]		
	mov word cx, [.temp_3]		
	ret

.subtraction:
	; ---VERIFY IF NUMBER_1 IS GREATER, SMALLER OR EQUAL TO NUMBER_2---
	mov word ax, [.temp_1]		
	mov word bx, [.temp_2]		
	cmp ax, bx
	jle .sublebx				; If AX is less than or equal to BX skip to .sublebx
	jmp .subgbx					; but if not skip to .subgbx

.sublebx:
	; ---CALC SUBTRACTION---
	sub bx, ax					; BX - AX = BX(subtraction result)
	mov word [.temp_1], bx		
	; ---RESULT---
	mov word [.temp_2], di		
	popa
	mov word ax, [.temp_1]		
	mov word si, [.temp_2]		
	mov word cx, [.temp_3]		
	ret
	
.subgbx:
	; ---CALC SUBTRACTION---
	sub ax, bx					; AX - BX = AX(subtraction result)
	mov word [.temp_1], ax		
	; ---RESULT---
	mov word [.temp_2], si		
	popa
	mov word ax, [.temp_1]		
	mov word si, [.temp_2]		
	mov word cx, [.temp_3]		
	ret
	
	.temp_1   dw 0
	.temp_2   dw 0
	.temp_3   dw 0
	.temp_4   dw 0 



; --------------------------------------------------------------------
; pmul -- Fixed point multiplication
; IN: AX = number_1
; IN: BX = number_2
; IN: CX = scale factor of number_1
; IN: DX = scale factor of number_2
; IN: SI = sign(0 = + OR 1 = -) of number_1
; IN: DI = sign(0 = + OR 1 = -) of number_2
; OUT: AX = multiplication result
; OUT: CX = final scale factor
; OUT: SI = final sign(0 = + OR 1 = -)

pmul:
	; ---INPUT---
	pusha
	mov word [.temp_1], ax		
	mov word [.temp_2], bx		
	mov ax, cx			
	mov bx, dx			
	; ---CALC SCALE FACTOR---
	mul bx						; AX * BX = AX(resulting scale factor)
	mov word [.temp_3], ax		
	; ---CALC MULTIPLICATION---
	mov word ax, [.temp_1]		
	mov word bx, [.temp_2]		
	mul bx						; AX mul BX = AX(multiplication result)
	mov word [.temp_1], ax		
	; ---RESULT---
	popa
	xor di, si					; calc signal
	mov si, di			 
	mov word ax, [.temp_1]		
	mov word cx, [.temp_3]		
	ret

	.temp_1   dw 0
	.temp_2   dw 0
	.temp_3   dw 0



; --------------------------------------------------------------------
; pdiv -- Fixed point division
; IN: AX = number_1
; IN: BX = number_2
; IN: CX = scale factor of number_1
; IN: DX = scale factor of number_2
; IN: SI = sign(0 = + OR 1 = -) of number_1
; IN: DI = sign(0 = + OR 1 = -) of number_2
; OUT: AX = quotient (with integer and fractional part)
; OUT: BX = remainder
; OUT: CX = final scale factor
; OUT: DX = error indicator for division by zero (1 = error, 0 = not error)
; OUT: SI = final sign(0 = + OR 1 = -)

pdiv:
	; ---INPUT---
	pusha
	cmp bx, 0					; check if the denominator is equal to zero
	jz .error_division_by_zero	; if so skip to .error_division_by_zero, if not skip to .continue
	jmp .continue

.error_division_by_zero:
	popa
	mov dx, 1					; ERROR: division by zero	
	ret

.continue:
	mov word [.temp_1], ax		
	mov word [.temp_2], bx		
	mov ax, cx			
	mov bx, dx			
	; ---PREPARING FOR THE DIVISION---
	; DIVIDEND
	mov cx, ax			
	mov ax, bx			
	mul word [.temp_1]			; AX * .temp_1 = AX(number_1 * scale factor of number_2)
	mov word [.temp_1], ax		
	; DIVISOR
	mov ax, cx			
	mul word [.temp_2]			; AX * .temp_2 = AX(number_2 * scale factor of number_1)
	mov word [.temp_2], ax		
	; ---MULTIPLY 1000 BY DIVIDEND---
	xor dx, dx
	mov word ax, [.temp_1]		
	mov word bx, 1000			; move 1000 (CONSTANT ScaleFactor) to BX
	mul bx						; AX * BX = AX:DX(dividend * CONSTANT ScaleFactor)
	mov word [.temp_1], ax		
	mov word [.temp_3], dx		
	; ---CALC DIVISION---
	mov word ax, [.temp_1]		
	mov word bx, [.temp_2]		
	mov word dx, [.temp_3]		
	div bx						; DX:AX / BX = AX(quotient), DX(remainder)
	mov word [.temp_1], ax		
	mov word [.temp_2], dx		
	; ---RESULT---
	popa
	xor di, si					; calc signal
	mov si, di			 
	mov word ax, [.temp_1]		
	mov word bx, [.temp_2]		
	mov cx, 1000			
	mov dx, 0					; move 0 to DX
	ret
	
	.temp_1   dw 0
	.temp_2   dw 0
	.temp_3   dw 0



; --------------------------------------------------------------------
; sin -- Sine in Degrees
; IN: AX = angle in Degrees(0-360)
; OUT: AX = sine / 1000
; OUT: SI = signal(0 = + OR 1 = -)

sin:
	pusha
	xor dx, dx					; clean DX
	mov cx, 360			
	div cx						; AX / CX = AX(quotient), DX(remainder)
	mov ax, dx					; AX = AX % 360 
	mov si, 0					; signal (positive)
	mov dx, 180			
	cmp ax, dx					; compares if AX is less than or equal to DX
	jle .positive_sine			; if it is smaller, skip to .positive_sine
	jmp .negative_sine			; if not, skip to .negative_sine

.negative_sine:
	mov si, 1					; set the signal to 1
	mov dx, 180			
	sub ax, dx					; AX = AX - DX

.positive_sine:
	mov cx, 2			
	mul cx						; AX = AX * CX
	mov bx, ax			
	mov cx, [bx + TrigTable]	; move [BX + TrigTable] to CX
	mov word [.temp_1], cx		
	mov word [.temp_2], si		
	; ---RESULT---
	popa
	mov word ax, [.temp_1]		
	mov word si, [.temp_2]		
	ret
	
	.temp_1   dw 0
	.temp_2   dw 0



; --------------------------------------------------------------------
; cos -- Cosine in Degrees
; IN: AX = angle in Degrees(0-360)
; OUT: AX = cosine / 1000
; OUT: SI = signal(0 = + OR 1 = -)

cos:
	pusha
	xor dx, dx					; clean DX
	mov cx, 360			
	div cx						; AX / CX = AX(quotient), DX(remainder)
	mov ax, dx					; AX = AX % 360 
	mov dx, 180			
	cmp ax, dx					; compares if AX is less than or equal to DX
	jle .lessthan180			; if it is smaller, skip to .lessthan180
	jmp .greaterthan180 		; if not, skip to .greaterthan180

.lessthan180:
	mov dx, 90			
	cmp ax, dx					; compares if AX is less than or equal to DX
	jle .lpositive_cosine		; if it is smaller, skip to .lpositive_cosine
	jmp .lnegative_cosine		; if not, skip to .lnegative_cosine

.lpositive_cosine:
	sub dx, ax					; DX = DX - AX
	mov ax, dx
	mov si, 0			
	jmp .next					; jump to .next

.lnegative_cosine: 
	sub ax, dx					; AX = AX - DX
	mov si, 1
	jmp .next					; jump to .next

.greaterthan180:		
	mov dx, 270
	cmp ax, dx					; compares if AX is greater than or equal to DX
	jge .gpositive_cosine		; if it is greater, skip to .gpositive_cosine
	jmp .gnegative_cosine		; if not, skip to .gnegative_cosine
	
.gpositive_cosine:
	mov dx, 180			
	sub ax, dx					; AX = AX - DX
	mov dx, 90
	sub ax, dx					; AX = AX - DX
	mov si, 0
	jmp .next					; jump to .next

.gnegative_cosine:
	mov dx, 180			
	sub ax, dx					; AX = AX - DX
	mov dx, 90
	sub dx, ax					; DX = DX - AX
	mov ax, dx
	mov si, 1
	jmp .next					; jump to .next

.next:
	mov cx, 2			
	mul cx						; AX = AX * CX
	mov bx, ax			
	mov cx, [bx + TrigTable]  	; move [BX + TrigTable] to CX
	mov word [.temp_1], cx		
	mov word [.temp_2], si		
	; ---RESULT---
	popa
	mov word ax, [.temp_1]		
	mov word si, [.temp_2]		
	ret
	
	.temp_1   dw 0
	.temp_2   dw 0



; --------------------------------------------------------------------
; tan -- tangent in degrees
; IN: AX = angle in Degrees(0-360)
; OUT: AX = tangent / 1000
; OUT: SI = signal(0 = + OR 1 = -)
; OUT: If AX is equal to FFFF in hex(or 65535 in decimal), it means that the tangent AX DOES NOT EXIST
tan:
	pusha
	; ---CALC SIN AND COS---
	mov word [.temp_1], ax		
	call sin					; AX angle , resulting in CX sin and SI is signal
	mov dx, ax			
	mov word [.temp_2], si		
	mov word ax, [.temp_1]		
	call cos					; AX angle , resulting in CX cos and SI is signal
	mov cx, ax			
	mov word [.temp_3], si		
	; ---ORGANIZE---
	cmp cx, 0					; Compares if CX is equal to 1 if it is
	jz .error_not_exist			; if it is jumps to .error_not_exist if not continue
	mov ax, dx			
	mov bx, cx			
	mov cx, 1			
	mov dx, 1			
	mov word si, [.temp_2]		
	mov word di, [.temp_3]		
	; ---CALC TANGENT---
	call pdiv					; AX / BX = AX(quotient), BX(remainder), CX(final scale factor),  
								; DX(error indicator for division by zero), SI (final sign)
	mov word [.temp_1], ax		
	mov word [.temp_2], si		
	; ---RESULT---
	popa
	mov word ax, [.temp_1]		
	mov word si, [.temp_2]		
	ret

.error_not_exist:
	popa
	mov ax, 65535				; return FFFF in hex(or 65535 decimal)
	mov si, 0					; return signal equal 0
	ret
	
	.temp_1   dw 0
	.temp_2   dw 0
	.temp_3   dw 0



; --------------------------------------------------------------------
; pi -- Constant pi
; OUT: PI = constant pi (3.14)
; OUT: PI_SCALE_FACTOR = 100

pi:
	PI   dw 314			; PI constant
	PI_SCALE_FACTOR   dw 100	; scale factor of PI
	ret



; ====================================================================
; ----DATA------------------------------------------------------------
; ====================================================================
TrigTable:
    dw    0,  17,  34,  52,  69,  87, 104, 121, 139, 156
    dw  173, 190, 207, 224, 241, 258, 275, 292, 309, 325
    dw  342, 358, 374, 390, 406, 422, 438, 453, 469, 484
    dw  500, 515, 529, 544, 559, 573, 587, 601, 615, 629
    dw  642, 656, 669, 681, 694, 707, 719, 731, 743, 754
    dw  766, 777, 788, 798, 809, 819, 829, 838, 848, 857 
    dw  866, 874, 882, 891, 898, 906, 913, 920, 927, 933
    dw  939, 945, 951, 956, 961, 965, 970, 974, 978, 981
    dw  984, 987, 990, 992, 994, 996, 997, 998, 999, 999
    dw 1000, 999, 999, 998, 997, 996, 994, 992, 990, 987
    dw  984, 981, 978, 974, 970, 965, 961, 956, 951, 945 
    dw  939, 933, 927, 920, 913, 906, 898, 891, 882, 874
    dw  866, 857, 848, 838, 829, 819, 809, 798, 788, 777
    dw  766, 754, 743, 731, 719, 707, 694, 681, 669, 656
    dw  642, 629, 615, 601, 587, 573, 559, 544, 529, 515 
    dw  500, 484, 469, 453, 438, 422, 406, 390, 374, 358
    dw  342, 325, 309, 292, 275, 258, 241, 224, 207, 190
    dw  173, 156, 139, 121, 104,  87,  69,  52,  35,  17
    dw    0
