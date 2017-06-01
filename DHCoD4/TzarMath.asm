section .code


%define TVec_t float80_t
%define TVec tword

NASMX_STRUC TVec3_t
    NASMX_RESERVE x, TVec_t, 1
    NASMX_RESERVE y, TVec_t, 1
    NASMX_RESERVE z, TVec_t, 1
NASMX_ENDSTRUC
%define TVec3_t_size (sizeof(TVec_t)*3)


;---------------------------------------------------------------------------
;precision conversion routines
;---------------------------------------------------------------------------
proto  FVec3ToTVec3, ptrdiff_t pfVec3, ptrdiff_t ptVec3
proto  TVec3ToFVec3, ptrdiff_t ptVec3, ptrdiff_t pfVec3
proto  TVec3Copy, ptrdiff_t pfVec3, ptrdiff_t ptVec3
;---------------------------------------------------------------------------
;arithmetic routines
;---------------------------------------------------------------------------
proto  TVec3Multiply, ptrdiff_t pfVec3, ptrdiff_t ptVec3
proto  TVec3MultiplyT, ptrdiff_t pfVec3, ptrdiff_t tS
proto  TVec3Subtract, ptrdiff_t pfVec3, ptrdiff_t ptVec3
proto  TVec3Add, ptrdiff_t pfVec3, ptrdiff_t ptVec3
proto TVec3Lerp, ptrdiff_t pfVec3, ptrdiff_t ptVec3, ptrdiff_t ptA
proto TVec3Length, ptrdiff_t pVec3
proto TVec3Distance, ptrdiff_t pfVec3, ptrdiff_t ptVec3
proto TVec3Normalize, ptrdiff_t pVec3
proto TVec3Null, ptrdiff_t pVec3
global TRadToDeg ; converts st0 from radians to degrees
proto  TVec3Angles, ptrdiff_t pX, ptrdiff_t pY, ptrdiff_t pZ
global TDegToRadEx ; converts st0 from degrees(0-360) to radians
proto  TVec3AngleVectors, ptrdiff_t tV, ptrdiff_t forward, ptrdiff_t right, ptrdiff_t up


proc TVec3Operation, ptrdiff_t pVec1, ptrdiff_t pVec2, ptrdiff_t pOp
locals none
    ; calls *pOp for each entry in an array of 3 vectors
    ;uses ebx, esi, eax, edx
    mov esi, sizeof(TVec_t)
    mov eax, [argv(.pVec1)]
    mov edx, [argv(.pVec2)]
    mov ebx, [argv(.pOp)]
    
    call ebx
    add eax, esi
    add edx, esi
    
    call ebx
    add eax, esi
    add edx, esi
    
    call ebx
    add eax, esi
    add edx, esi
endproc

;---------------------------------------------------------------------------
;precision conversion routines
;---------------------------------------------------------------------------
proc  FVec3ToTVec3, ptrdiff_t pfVec3, ptrdiff_t ptVec3
locals none
    mov eax, [argv(.pfVec3)]
    mov edx, [argv(.ptVec3)]
    
    ;*ptX = *pfX
    fld dword [eax]
    fstp TVec [edx]
    
    ;*ptY = *pfY
    fld dword [eax+4]
    fstp TVec [edx+sizeof(TVec_t)]
    
    ;*ptZ = *pfZ
    fld dword [eax+8]
    fstp TVec [edx+sizeof(TVec_t)*2]
endproc

;---------------------------------------------------------------------------

proc  TVec3ToFVec3, ptrdiff_t ptVec3, ptrdiff_t pfVec3
locals none
    mov eax, [argv(.ptVec3)]
    mov edx, [argv(.pfVec3)]
    
    ;*pfX = *ptX
    fld TVec [eax]
    fstp dword [edx]
    
    ;*pfY = *ptY
    fld TVec [eax+sizeof(TVec_t)]
    fstp dword [edx+4]
    
    ;*pfZ = *ptZ
    fld TVec [eax+sizeof(TVec_t)*2]
    fstp dword [edx+8]
endproc

;---------------------------------------------------------------------------

proc  TVec3Copy, ptrdiff_t pfVec3, ptrdiff_t ptVec3
locals none
    ;memcpy would be more efficient
    invoke TVec3Operation, [argv(.pfVec3)], [argv(.ptVec3)], CopyCallback
endproc
CopyCallback:
    fld TVec [eax]
    fstp TVec [ebx]
    ret

;---------------------------------------------------------------------------
;arithmetic routines
;---------------------------------------------------------------------------
proc  TVec3Multiply, ptrdiff_t pfVec3, ptrdiff_t ptVec3
locals none
    invoke TVec3Operation, [argv(.pfVec3)], [argv(.ptVec3)], MultiplyCallback
endproc
MultiplyCallback:
    fld TVec [eax]
    fld TVec [ebx]
    fmul
    fstp TVec [eax]
    ret

;---------------------------------------------------------------------------

proc  TVec3MultiplyT, ptrdiff_t pfVec3, ptrdiff_t tS
locals none
    mov edi, [argv(.tS)]
    invoke TVec3Operation, [argv(.pfVec3)], ptrdiff_t 0, MultiplyTCallback
endproc
MultiplyTCallback:
    fld TVec [eax]
    fld TVec [edi]
    fmul
    fstp TVec [eax]
    ret

;---------------------------------------------------------------------------

proc  TVec3Subtract, ptrdiff_t pfVec3, ptrdiff_t ptVec3
locals none
    invoke TVec3Operation, [argv(.pfVec3)], [argv(.ptVec3)], SubtractCallback
endproc
SubtractCallback:
    fld TVec [eax]
    fld TVec [ebx]
    fsub
    fstp TVec [eax]
    ret

;---------------------------------------------------------------------------

proc  TVec3Add, ptrdiff_t pfVec3, ptrdiff_t ptVec3
locals none
    invoke TVec3Operation, [argv(.pfVec3)], [argv(.ptVec3)], AddCallback
endproc
AddCallback:
    fld TVec [eax]
    fld TVec [edx]
    fadd
    fstp TVec [eax]
    ret

;---------------------------------------------------------------------------

proc TVec3Lerp, ptrdiff_t pfVec3, ptrdiff_t ptVec3, ptrdiff_t ptA
locals none
    ;lerps From To by *ptA(amount: 0.3 would lerp pFrom 30% to pt)
    ;*pf = (*pf * (1.0 - *ptA)) + (*pt * *ptA)
    
    ;this formula would be more efficient and possibly more accurate due to less calculations:
    ;from + frac * (to - from)
    mov edi, [argv(.ptA)]
    invoke TVec3Operation, [argv(.pfVec3)], [argv(.ptVec3)], LerpCallback
endproc
LerpCallback:
    fld1
    fld TVec [edi]
    fsub
    fld TVec [eax]
    fmul
    fld TVec [edi]
    fld TVec [edx]
    fmul
    fadd
    fstp TVec [eax]
    ret

;---------------------------------------------------------------------------

proc TVec3Length, ptrdiff_t pVec3
locals none
    fldz
    invoke TVec3Operation, [argv(.pVec3)], ptrdiff_t 0, LengthCallback
    fsqrt
endproc
LengthCallback:
    fld TVec [eax]
    fld TVec [eax]
    fmul
    fadd
    ret

;---------------------------------------------------------------------------

proc TVec3Distance, ptrdiff_t pfVec3, ptrdiff_t ptVec3
locals
    local fwd, TVec3_t
endlocals
    lea edi, [var(.fwd)]
    invoke TVec3Copy, [argv(.pfVec3)], edi
    invoke TVec3Subtract, edi, [argv(.ptVec3)]
    invoke TVec3Length, edi
endproc

;---------------------------------------------------------------------------

proc TVec3Normalize, ptrdiff_t pVec3
locals
    local denominator, TVec_t
endlocals
    fld1
    invoke TVec3Length, [argv(.pVec3)]
    fdiv
    fstp TVec [var(.denominator)]
    invoke TVec3MultiplyT, [argv(.pVec3)], var(.denominator)
endproc

;---------------------------------------------------------------------------

proc TVec3Null, ptrdiff_t pVec3
locals none
    invoke TVec3Operation, [argv(.pVec3)], ptrdiff_t 0, NullCallback
endproc
NullCallback:
  fldz
  fstp TVec [eax]
  fldz
  fstp TVec [edx]
    ret


;---------------------------------------------------------------------------

TRadToDeg: ; converts st0 from radians to degrees
    fld TVec [t180]
    fldpi
    fdivp
    ; TODO: just load a RadToDeg constant, but be sure to calculate it yourself accurately
    fmulp
    ret

;---------------------------------------------------------------------------
    
proc  TVec3Angles, ptrdiff_t pX, ptrdiff_t pY, ptrdiff_t pZ
locals
    local yaw, TVec_t
    local pitch, TVec_t
    local forward, TVec_t
endlocals
    mov edx, [argv(.pX)]
    mov ebx, [argv(.pY)]
    fld TVec [edx]
    fldz
    fucompp
    fnstsw ax
    sahf
    jne .notz
    fld TVec [ebx]
    fldz
    fucompp
    fnstsw ax
    sahf
    jne .notz
    fldz
    fstp TVec [var(.yaw)]
    fldz
    mov eax, [argv(.pZ)]
    fld TVec [eax]
    fcompp
    fnstsw ax
    sahf
    jl .lesser
    fld TVec [t90]
    fstp TVec [var(.pitch)]
    jmp .setoutput
.lesser:
    fld TVec [t270]
    fstp TVec [var(.pitch)]
    jmp .setoutput
.notz:
    fldz
    fld TVec [edx]
    fucompp
    fnstsw ax
    sahf
    jz .xz
    fld TVec [ebx]
    fld TVec [edx]
    fpatan
    call TRadToDeg
    fstp TVec [var(.yaw)]
    jmp .yawnormalize
.xz:
    fldz
    fld TVec [ebx]
    fucompp
    fnstsw ax
    sahf
    jl .ylesser
    fld TVec [t90]
    fstp TVec [var(.yaw)]
    jmp .yawnormalize
.ylesser:
    fld TVec [t270]
    fstp TVec [var(.yaw)]
.yawnormalize:
    fldz
    fld TVec [var(.yaw)]
    fucompp
    fnstsw ax
    sahf
    jg .zgreater
    fld TVec [var(.yaw)]
    fld TVec [t360]
    fadd
    fstp TVec [var(.yaw)]
.zgreater:
    fld TVec [edx]
    fld TVec [edx]
    fmul
    fld TVec [ebx]
    fld TVec [ebx]
    fmul
    fadd
    fsqrt
    fstp TVec [var(.forward)]
    mov eax, [argv(.pZ)]
    fld TVec [eax]
    fld TVec [var(.forward)]
    fpatan
    call TRadToDeg
    fldz
    fucomp
    fnstsw ax
    sahf
    jg .pitchpositive
    fld TVec [t360]
    fadd
.pitchpositive:
    fstp TVec [var(.pitch)]
.setoutput:
    fld TVec [var(.pitch)]
    fchs
    fstp TVec [edx]
    fld TVec [var(.yaw)]
    fstp TVec [ebx]
    fldz
    mov eax, [argv(.pZ)]
    fstp TVec [eax]
endproc

;---------------------------------------------------------------------------

TDegToRadEx: ; converts st0 from degrees(0-360) to radians
    fldpi
    fldpi
    fadd
    fld TVec [t360]
    fdiv
    ; TODO: just load a DegToRad constant, but be sure to calculate it yourself in atleast 80bit extended precision
    fmul
    ret

;---------------------------------------------------------------------------

proc  TVec3AngleVectors, ptrdiff_t tV, ptrdiff_t forward, ptrdiff_t right, ptrdiff_t up ; FIXME: it is possible that i didn't write this function correctly, because i didn't follow the left to right rule when solving expressions
locals
    local tsr, TVec_t
    local tsp, TVec_t
    local tsy, TVec_t
    local tcr, TVec_t
    local tcp, TVec_t
    local tcy, TVec_t
endlocals
    mov eax, [argv(.tV)]
    
    fld TVec [eax+sizeof(TVec_t)*2]
    call TDegToRadEx
    fsincos
    fstp TVec [var(.tcr)]
    fstp TVec [var(.tsr)]
    
    fld TVec [eax]
    call TDegToRadEx
    fsincos
    fstp TVec [var(.tcp)]
    fstp TVec [var(.tsp)]
    
    fld TVec [eax+sizeof(TVec_t)]
    call TDegToRadEx
    fsincos
    fstp TVec [var(.tcy)]
    fstp TVec [var(.tsy)]

    mov eax, [argv(.right)]
    test eax, eax
    jz .noright
.noright:
    mov eax, [argv(.forward)]
    test eax, eax
    jz .noforward
    fld TVec [var(.tcy)]
    fld TVec [var(.tcp)]
    fmul
    fstp TVec [eax]
    fld TVec [var(.tcp)]
    fld TVec [var(.tsy)]
    fmul
    fstp TVec [eax+sizeof(TVec_t)]
    fld TVec [var(.tsp)]
    fchs
    fstp TVec [eax+sizeof(TVec_t)*2]
.noforward:
    mov eax, [argv(.up)]
    test eax, eax
    jz .noup
    fld TVec [var(.tsy)]
    fchs
    fld TVec [var(.tsr)]
    fchs
    fmul
    fld TVec [var(.tcy)]
    fld TVec [var(.tsp)]
    fmul
    fld TVec [var(.tcr)]
    fmul
    fadd
    fstp TVec [eax]
    fld TVec [var(.tcy)]
    fld TVec [var(.tsr)]
    fchs
    fmul
    fld TVec [var(.tsy)]
    fld TVec [var(.tsp)]
    fmul
    fld TVec [var(.tcr)]
    fmul
    fstp TVec [eax+sizeof(TVec_t)]
    fld TVec [var(.tcp)]
    fld TVec [var(.tcr)]
    fstp TVec [eax+sizeof(TVec_t)*2]
.noup:
endproc

;---------------------------------------------------------------------------

section .data
    tm1 dt -1.0
    t90 dt 90.0
    t270 dt 270.0
    t180 dt 180.0
    t360 dt 360.0
    
