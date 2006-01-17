/* i386 assembly routines for inner loop fraction routines in Metafont
   and MetaPost. Included from texmfmp.c. By Wayne Sullivan
   <wgs@maths.ucd.ie>.  */

/*
  Converted to inline assembler for Visual C++ [45].xx
  by Fabrice Popineau <Fabrice.Popineau@supelec.fr> */

__declspec(naked) fraction __stdcall ztakefraction(integer p, integer q) {
__asm {
        push ebp
	mov ebp,esp
        xor ecx,ecx
        mov eax, p
;        mov eax, [ebp+8]
        cmp eax, 0x80000000
        jz LL5
        imul q
        or edx,edx
        jns LL2
        neg edx
        neg eax
        sbb edx, ecx
        inc ecx
LL2:
        add eax, 0x08000000
        adc edx, 0
        cmp edx, 0x07ffffff
        ja LL3
        shrd eax,edx,28
LL1:
	jecxz LL4
        neg eax
LL4:
        mov esp,ebp
        pop ebp
	ret 8
LL5:     
	inc ecx
LL3:     
	mov eax, 0x7fffffff
        mov aritherror, 1
        jmp LL1
}
}

__declspec(naked) integer __stdcall ztakescaled(integer p, scaled q) {
__asm {
        push ebp
	mov ebp,esp
;        mov eax, [ebp+8]
        xor ecx,ecx
	mov eax, p
        cmp eax, 0x80000000
        jz LL5
        imul q
        or edx,edx
        jns LL12
        neg edx
        neg eax
        sbb edx,ecx
        inc ecx
LL12:
        add eax, 0x00008000
        adc edx, 0x0
        cmp edx, 0x00007fff
        ja LL3
        shrd eax,edx,16
LL1:
        jecxz LL4
        neg eax
LL4:
        mov esp,ebp
        pop ebp
	ret 8
LL5:     
	inc ecx
LL3:     
	mov eax, 0x7fffffff
        mov aritherror, 1
        jmp LL1
	  }
}

__declspec(naked) scaled __stdcall zmakescaled(integer p, integer q) {
  __asm {
        mov cx, 16
        push ebp
        mov ebp,esp
        push ebx
;        mov edx, [ebp+8]
	mov edx, p
        xor eax,eax
        or edx,edx
        jns LL32
        inc ch
        neg edx
LL32:
;        mov ebx, [ebp+12]
        mov ebx,q
        or ebx,ebx
        jns LL33
        dec ch
        neg ebx
        or ebx,ebx
        js LL34
LL33:
        or edx,edx
        js LL34
        shrd eax,edx,cl
        shr edx,cl
        cmp edx,ebx
        jae LL34
        div ebx
        add edx,edx
        inc edx
        sub ebx,edx
        adc eax, 0
        jc LL34
        cmp eax, 0x7fffffff
        ja LL34
LL31:    or ch,ch
        jz LL35
        neg eax
LL35:
        pop ebx
        mov esp, ebp
        pop ebp
        ret 8
LL34:    
        mov eax, 0x7fffffff
	mov aritherror, 1
        jmp LL31
  }
}

__declspec(naked) fraction __stdcall zmakefraction(integer p, integer q) {
  __asm {
        mov cx, 4
        push ebp
        mov ebp,esp
        push ebx
	mov edx, p
;        mov [ebp+8],edx
        xor eax,eax
        or edx,edx
        jns LL32
        inc ch
        neg edx
LL32:
;        mov [ebp+12],ebx
        mov ebx, q
        or ebx,ebx
        jns LL33
        dec ch
        neg ebx
        or ebx,ebx
        js LL34
LL33:
        or edx,edx
        js LL34
        shrd eax,edx, cl
        shr edx,cl
        cmp edx,ebx
        jae LL34
        div ebx
        add edx,edx
        inc edx
        sub ebx,edx
        adc eax, 0
        jc LL34
        cmp eax, 0x7fffffff
        ja LL34
LL31:    or ch,ch
        jz LL35
        neg eax
LL35:
        pop ebx
        mov esp, ebp
        pop ebp
        ret 8
LL34:    
        mov eax, 0x7fffffff
        mov aritherror, 1
        jmp LL31
  }
}


