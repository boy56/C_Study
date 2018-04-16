# Please set to begin at "main"
.globl main
.data
    str_0: .asciiz "define test passed  "
    str_1: .asciiz "operator test passed  "
    str_10: .asciiz "error 1  "
    str_11: .asciiz "error 2  "
    str_12: .asciiz "condition test passed  "
    str_13: .asciiz "miscellaneous test passed  "
    str_14: .asciiz "Fibonaci: Please input a unsigned positive integer ?  "
    str_15: .asciiz "Input error, try again:  "
    str_16: .asciiz "The fibonaci's answer is   "
    str_17: .asciiz "GCD: Please input two unsigned positive integer ?  "
    str_18: .asciiz "Input error, try Again:   "
    str_19: .asciiz "The gcd's answer is   "
    str_2: .asciiz "ABCDEFGHIJKLMNOPQRSTUVWXYZ abcdefghijklmnopqrstuvwxyz  "
    str_20: .asciiz "1  "
    str_21: .asciiz "*  "
    str_22: .asciiz "*  "
    str_23: .asciiz "prime_factorization: Please input a unsigned positive integer(<2^31-1) ?  "
    str_24: .asciiz "Input error, try again:  "
    str_25: .asciiz "The factor of n is   "
    str_26: .asciiz "Please input Test Type:E(Easy), (M)Middle, H(Hard):   "
    str_27: .asciiz "Try Again:   "
    str_3: .asciiz "!#$%&'()*+,-./:;<=>?@[]^_`{|}~  "
    str_4: .asciiz "dowhile test passed  "
    str_5: .asciiz "ifcondition clause error!  "
    str_6: .asciiz "ifcondition clause error  "
    str_7: .asciiz "switchcodintion clause  error!  "
    str_8: .asciiz "switchcodintion clause  error!  "
    str_9: .asciiz "error 0  "
.text

const_define_begin_:
    li  $v0, 1
    li  $a0, 1
    syscall
    li  $v0, 11
    li  $a0, 10
    syscall
    li  $v0, 11
    li  $a0, 47
    syscall
    li  $v0, 11
    li  $a0, 10
    syscall
    jr  $ra

const_define_test_begin_:
    sw  $ra, 0($fp)
    sw  $fp, -4($sp)
    addi  $sp, $sp, -12
    subi  $fp, $sp, -12
    addi  $sp, $sp, 0
    jal  const_define_begin_

call_9_:
    move  $sp, $fp
    lw  $fp, -4($fp)
    lw  $ra, 0($fp)
    jr  $ra

var_define_begin_:
    jr  $ra

var_define_test_begin_:
    sw  $ra, 0($fp)
    sw  $fp, -4($sp)
    addi  $sp, $sp, -12
    subi  $fp, $sp, -12
    addi  $sp, $sp, -80
    jal  var_define_begin_

call_18_:
    move  $sp, $fp
    lw  $fp, -4($fp)
    lw  $ra, 0($fp)
    jr  $ra

define_begin_:
    jr  $ra

define_test_begin_:
    sw  $ra, 0($fp)
    sw  $fp, -4($sp)
    addi  $sp, $sp, -12
    subi  $fp, $sp, -12
    addi  $sp, $sp, 0
    jal  var_define_test_begin_

call_27_:
    move  $sp, $fp
    lw  $fp, -4($fp)
    lw  $ra, 0($fp)
    sw  $ra, 0($fp)
    sw  $fp, -4($sp)
    addi  $sp, $sp, -12
    subi  $fp, $sp, -12
    addi  $sp, $sp, 0
    jal  const_define_test_begin_

call_30_:
    move  $sp, $fp
    lw  $fp, -4($fp)
    lw  $ra, 0($fp)
    sw  $ra, 0($fp)
    sw  $fp, -4($sp)
    addi  $sp, $sp, -12
    subi  $fp, $sp, -12
    addi  $sp, $sp, -12
    jal  define_begin_

call_33_:
    move  $sp, $fp
    lw  $fp, -4($fp)
    lw  $ra, 0($fp)
    li  $v0, 4
    la  $a0, str_0
    syscall
    li  $v0, 11
    li  $a0, 10
    syscall
    jr  $ra

operator_begin_:
    lw  $8, -12($fp)
    lw  $9, -16($fp)
    lw  $10, -20($fp)
    lw  $11, -24($fp)
    lw  $12, -28($fp)
    lw  $13, -32($fp)
    move  $14, $8
    move  $15, $9
    move  $16, $12
    li  $13, 6
    li  $14, 1
    div  $17, $14, $14
    li  $18, 1
    li  $16, 5
    mul  $19, $16, $13
    li  $11, -4
    div  $20, $11, 4
    li  $10, 3
    li  $21, 3
    mul  $22, $21, 3
    add  $23, $22, $10
    sub  $24, $23, $20
    add  $8, $24, $17
    mul  $25, $8, 3
    sw  $14, -120($fp)
    addi  $14, $8, 0
    sw  $15, -100($fp)
    add  $15, $14, $25
    sw  $16, -68($fp)
    add  $16, $15, $10
    sw  $17, -64($fp)
    sub  $17, $16, $20
    add  $9, $17, $19
    sw  $18, -116($fp)
    mul  $18, $9, 0
    sw  $19, -96($fp)
    sw  $20, -88($fp)
    li  $20, 0
    sub  $19, $20, $8
    add  $20, $19, $18
    sw  $21, -36($fp)
    lw  $21, -116($fp)
    sub  $12, $20, $21
    jr  $ra

operator_test_begin_:
    sw  $ra, 0($fp)
    sw  $fp, -4($sp)
    addi  $sp, $sp, -12
    subi  $fp, $sp, -12
    addi  $sp, $sp, -112
    jal  operator_begin_

call_74_:
    move  $sp, $fp
    lw  $fp, -4($fp)
    lw  $ra, 0($fp)
    li  $v0, 4
    la  $a0, str_1
    syscall
    li  $v0, 11
    li  $a0, 10
    syscall
    jr  $ra

string_begin_:
    li  $v0, 4
    la  $a0, str_2
    syscall
    li  $v0, 11
    li  $a0, 10
    syscall
    li  $v0, 4
    la  $a0, str_3
    syscall
    li  $v0, 11
    li  $a0, 10
    syscall
    jr  $ra

string_test_begin_:
    sw  $ra, 0($fp)
    sw  $fp, -4($sp)
    addi  $sp, $sp, -12
    subi  $fp, $sp, -12
    addi  $sp, $sp, 0
    jal  string_begin_

call_86_:
    move  $sp, $fp
    lw  $fp, -4($fp)
    lw  $ra, 0($fp)
    jr  $ra

dowhile_begin_:
    lw  $8, -12($fp)
    li  $8, 100

_91_label_:
    blt  $8, 50, _95_label_
    move  $9, $8
    div  $8, $9, 2
    sw  $9, -28($fp)

_95_label_:
    bge  $8, 50, _100_label_
    move  $9, $8
    addi  $10, $9, 10
    div  $8, $10, 10
    sw  $9, -24($fp)
    sw  $10, -20($fp)

_100_label_:
    bge  $8, 10, _91_label_

_102_label_:
    move  $9, $8
    subi  $8, $9, 1
    li  $v0, 4
    la  $a0, str_4
    syscall
    li  $v0, 11
    li  $a0, 10
    syscall
    li  $10, 0
    sw  $9, -16($fp)
    bgt  $10, 0, _102_label_
    jr  $ra

dowhile_test_begin_:
    sw  $ra, 0($fp)
    sw  $fp, -4($sp)
    addi  $sp, $sp, -12
    subi  $fp, $sp, -12
    addi  $sp, $sp, -20
    jal  dowhile_begin_

call_112_:
    move  $sp, $fp
    lw  $fp, -4($fp)
    lw  $ra, 0($fp)
    jr  $ra

ifcondition_begin_:
    lw  $8, -12($fp)
    lw  $9, -16($fp)
    li  $9, 0
    li  $8, 1
    bne  $8, 1, _121_label_
    move  $10, $9
    addi  $9, $10, 1
    sw  $10, -40($fp)

_121_label_:
    li  $8, 2
    ble  $8, 1, _126_label_
    move  $10, $9
    addi  $9, $10, 1
    sw  $10, -36($fp)

_126_label_:
    li  $8, 0
    bge  $8, 1, _131_label_
    move  $10, $9
    addi  $9, $10, 1
    sw  $10, -32($fp)

_131_label_:
    beq  $8, 1, _135_label_
    move  $10, $9
    addi  $9, $10, 1
    sw  $10, -28($fp)

_135_label_:
    blt  $8, 0, _139_label_
    move  $10, $9
    addi  $9, $10, 1
    sw  $10, -24($fp)

_139_label_:
    bgt  $8, 0, _143_label_
    move  $10, $9
    addi  $9, $10, 1
    sw  $10, -20($fp)

_143_label_:
    li  $10, 3
    li  $11, 2
    addi  $12, $11, 1
    add  $13, $12, $10
    sw  $10, -52($fp)
    sw  $11, -44($fp)
    sw  $12, -48($fp)
    sw  $13, -56($fp)
    beq  $9, $13, _150_label_
    li  $v0, 4
    la  $a0, str_5
    syscall
    li  $v0, 11
    li  $a0, 10
    syscall

_150_label_:
    beq  $9, $9, _153_label_
    li  $v0, 4
    la  $a0, str_6
    syscall
    li  $v0, 11
    li  $a0, 10
    syscall

_153_label_:
    jr  $ra

switchcodintionans_begin_:
    lw  $8, -12($fp)
    bne  $8, 1, _161_label_
    li  $v1, 2
    jr  $ra
    j  switch_158_end_

_161_label_:
    bne  $8, 2, _165_label_
    li  $v1, 3
    jr  $ra
    j  switch_158_end_

_165_label_:
    bne  $8, 3, _169_label_
    li  $v1, 4
    jr  $ra
    j  switch_158_end_

_169_label_:

switch_158_end_:
    li  $v1, 5
    jr  $ra

switchcodintion_begin_:
    lw  $8, -12($fp)
    lw  $9, -16($fp)
    li  $9, 0
    li  $8, 1

_176_label_:
    addi  $10, $8, 1
    sw  $ra, 0($fp)
    sw  $fp, -4($sp)
    addi  $sp, $sp, -12
    sw  $8, 0($sp)
    addi  $sp, $sp, -4
    sw  $8, -12($fp)
    sw  $9, -16($fp)
    sw  $10, -20($fp)
    subi  $fp, $sp, -16
    addi  $sp, $sp, 0
    jal  switchcodintionans_begin_

call_181_:
    move  $sp, $fp
    lw  $fp, -4($fp)
    lw  $ra, 0($fp)
    lw  $8, -12($fp)
    lw  $9, -16($fp)
    move  $10, $v1
    lw  $11, -20($fp)
    sw  $10, -24($fp)
    sw  $11, -20($fp)
    bne  $11, $10, _186_label_
    move  $10, $9
    addi  $9, $10, 1
    sw  $10, -32($fp)

_186_label_:
    move  $10, $8
    addi  $8, $10, 1
    sw  $10, -28($fp)
    blt  $8, 5, _176_label_
    beq  $9, 4, _192_label_
    li  $v0, 4
    la  $a0, str_7
    syscall
    li  $v0, 11
    li  $a0, 10
    syscall

_192_label_:
    ble  $9, 0, _195_label_
    move  $v1, $8
    jr  $ra

_195_label_:
    li  $v0, 4
    la  $a0, str_8
    syscall
    li  $v0, 11
    li  $a0, 10
    syscall
    jr  $ra

condition_begin_:
    lw  $8, -12($fp)
    li  $8, 0

_201_label_:
    bne  $8, 0, _207_label_
    beq  $8, 0, _205_label_
    li  $v0, 4
    la  $a0, str_9
    syscall
    li  $v0, 11
    li  $a0, 10
    syscall

_205_label_:
    j  switch_202_end_

_207_label_:
    bne  $8, 1, _213_label_
    beq  $8, 1, _211_label_
    li  $v0, 4
    la  $a0, str_10
    syscall
    li  $v0, 11
    li  $a0, 10
    syscall

_211_label_:
    j  switch_202_end_

_213_label_:
    bne  $8, 2, _219_label_
    beq  $8, 2, _217_label_
    li  $v0, 4
    la  $a0, str_11
    syscall
    li  $v0, 11
    li  $a0, 10
    syscall

_217_label_:
    j  switch_202_end_

_219_label_:

switch_202_end_:
    move  $9, $8
    addi  $8, $9, 1
    sw  $9, -16($fp)
    blt  $8, 5, _201_label_
    jr  $ra

condition_test_begin_:
    sw  $ra, 0($fp)
    sw  $fp, -4($sp)
    addi  $sp, $sp, -12
    subi  $fp, $sp, -12
    addi  $sp, $sp, -48
    jal  ifcondition_begin_

call_229_:
    move  $sp, $fp
    lw  $fp, -4($fp)
    lw  $ra, 0($fp)
    sw  $ra, 0($fp)
    sw  $fp, -4($sp)
    addi  $sp, $sp, -12
    subi  $fp, $sp, -12
    addi  $sp, $sp, -24
    jal  switchcodintion_begin_

call_232_:
    move  $sp, $fp
    lw  $fp, -4($fp)
    lw  $ra, 0($fp)
    move  $8, $v1
    sw  $ra, 0($fp)
    sw  $fp, -4($sp)
    addi  $sp, $sp, -12
    sw  $8, -12($fp)
    subi  $fp, $sp, -12
    addi  $sp, $sp, -8
    jal  condition_begin_

call_236_:
    move  $sp, $fp
    lw  $fp, -4($fp)
    lw  $ra, 0($fp)
    li  $v0, 4
    la  $a0, str_12
    syscall
    li  $v0, 11
    li  $a0, 10
    syscall
    jr  $ra

miscellaneous_begin_:
    lw  $8, -12($fp)
    lw  $9, -16($fp)
    lw  $10, -120($fp)
    lw  $11, -116($fp)
    lw  $12, -112($fp)
    li  $9, 2
    li  $8, 1

_243_label_:
    addi  $13, $12, 5
    mul  $13, $13, -4
    add  $13, $13, $fp
    sw  $12, 0($13)
    move  $13, $12
    addi  $12, $13, 1
    sw  $13, -1180($fp)
    blt  $12, 23, _243_label_
    li  $11, 127
    move  $12, $11
    li  $10, 97
    li  $8, 0

_252_label_:
    li  $13, 2
    mul  $14, $8, 1
    sub  $15, $14, $8
    subi  $16, $15, 1
    add  $17, $16, $13
    subi  $18, $17, 1
    div  $19, $18, 2
    li  $20, 97
    addi  $21, $19, 31
    mul  $21, $21, -4
    add  $21, $21, $fp
    sw  $20, 0($21)
    move  $21, $8
    addi  $8, $21, 1
    sw  $13, -1164($fp)
    sw  $14, -1152($fp)
    sw  $15, -1156($fp)
    sw  $16, -1160($fp)
    sw  $17, -1168($fp)
    sw  $18, -1172($fp)
    sw  $19, -1176($fp)
    sw  $21, -1148($fp)
    blt  $8, $11, _252_label_
    lw  $13, -124($fp)
    sw  $13, -1184($fp)
    bne  $13, 97, _267_label_
    li  $v0, 4
    la  $a0, str_13
    syscall
    li  $v0, 11
    li  $a0, 10
    syscall

_267_label_:
    jr  $ra

miscellaneous_test_begin_:
    sw  $ra, 0($fp)
    sw  $fp, -4($sp)
    addi  $sp, $sp, -12
    subi  $fp, $sp, -12
    addi  $sp, $sp, -1176
    jal  miscellaneous_begin_

call_273_:
    move  $sp, $fp
    lw  $fp, -4($fp)
    lw  $ra, 0($fp)
    jr  $ra

fibonaci_begin_:
    lw  $8, -12($fp)
    bne  $8, 0, _281_label_
    li  $v1, 0
    jr  $ra
    j  switch_278_end_

_281_label_:
    bne  $8, 1, _285_label_
    li  $v1, 1
    jr  $ra
    j  switch_278_end_

_285_label_:
    bne  $8, 2, _289_label_
    li  $v1, 1
    jr  $ra
    j  switch_278_end_

_289_label_:

switch_278_end_:
    sw  $ra, 0($fp)
    sw  $fp, -4($sp)
    addi  $sp, $sp, -12
    subi  $9, $8, 1
    sw  $9, 0($sp)
    addi  $sp, $sp, -4
    sw  $8, -12($fp)
    sw  $9, -16($fp)
    subi  $fp, $sp, -16
    addi  $sp, $sp, -20
    jal  fibonaci_begin_

call_295_:
    move  $sp, $fp
    lw  $fp, -4($fp)
    lw  $ra, 0($fp)
    lw  $8, -12($fp)
    move  $9, $v1
    sw  $ra, 0($fp)
    sw  $fp, -4($sp)
    addi  $sp, $sp, -12
    subi  $10, $8, 2
    sw  $10, 0($sp)
    addi  $sp, $sp, -4
    sw  $8, -12($fp)
    sw  $9, -20($fp)
    sw  $10, -24($fp)
    subi  $fp, $sp, -16
    addi  $sp, $sp, -20
    jal  fibonaci_begin_

call_301_:
    move  $sp, $fp
    lw  $fp, -4($fp)
    lw  $ra, 0($fp)
    lw  $8, -12($fp)
    move  $9, $v1
    lw  $11, -20($fp)
    add  $10, $11, $9
    move  $v1, $10
    jr  $ra

fibonaci_test_begin_:
    lw  $8, -12($fp)
    li  $v0, 4
    la  $a0, str_14
    syscall
    li  $v0, 11
    li  $a0, 10
    syscall
    li  $v0, 5
    syscall
    move  $8, $v0

_309_label_:
    bge  $8, 0, _313_label_
    li  $v0, 4
    la  $a0, str_15
    syscall
    li  $v0, 11
    li  $a0, 10
    syscall
    li  $v0, 5
    syscall
    move  $8, $v0

_313_label_:
    blt  $8, 0, _321_label_
    sw  $ra, 0($fp)
    sw  $fp, -4($sp)
    addi  $sp, $sp, -12
    sw  $8, 0($sp)
    addi  $sp, $sp, -4
    sw  $8, -12($fp)
    subi  $fp, $sp, -16
    addi  $sp, $sp, -20
    jal  fibonaci_begin_

call_318_:
    move  $sp, $fp
    lw  $fp, -4($fp)
    lw  $ra, 0($fp)
    lw  $8, -12($fp)
    move  $9, $v1
    li  $v0, 4
    la  $a0, str_16
    syscall
    li  $v0, 1
    move  $a0, $9
    syscall
    li  $v0, 11
    li  $a0, 10
    syscall
    sw  $9, -16($fp)

_321_label_:
    blt  $8, 0, _309_label_
    jr  $ra

mod_begin_:
    lw  $8, -16($fp)
    lw  $9, -12($fp)
    div  $10, $9, $8
    mul  $11, $10, $8
    sub  $12, $9, $11
    move  $v1, $12
    jr  $ra

gcd_begin_:
    lw  $8, -16($fp)
    lw  $9, -12($fp)
    lw  $10, -20($fp)
    li  $10, 0
    bne  $9, 0, _339_label_
    move  $v1, $8
    jr  $ra

_339_label_:
    bne  $8, 0, _342_label_
    move  $v1, $9
    jr  $ra

_342_label_:
    sw  $ra, 0($fp)
    sw  $fp, -4($sp)
    addi  $sp, $sp, -12
    sw  $8, 0($sp)
    addi  $sp, $sp, -4
    sw  $ra, 0($fp)
    sw  $fp, -4($sp)
    addi  $sp, $sp, -12
    sw  $9, 0($sp)
    addi  $sp, $sp, -4
    sw  $8, 0($sp)
    addi  $sp, $sp, -4
    sw  $8, -16($fp)
    sw  $9, -12($fp)
    sw  $10, -20($fp)
    subi  $fp, $sp, -20
    addi  $sp, $sp, -12
    jal  mod_begin_

call_349_:
    move  $sp, $fp
    lw  $fp, -4($fp)
    lw  $ra, 0($fp)
    lw  $8, -16($fp)
    lw  $9, -12($fp)
    lw  $10, -20($fp)
    move  $11, $v1
    sw  $11, 0($sp)
    addi  $sp, $sp, -4
    sw  $8, -16($fp)
    sw  $9, -12($fp)
    sw  $10, -20($fp)
    sw  $11, -24($fp)
    subi  $fp, $sp, -20
    addi  $sp, $sp, -12
    jal  gcd_begin_

call_353_:
    move  $sp, $fp
    lw  $fp, -4($fp)
    lw  $ra, 0($fp)
    lw  $8, -16($fp)
    lw  $9, -12($fp)
    lw  $10, -20($fp)
    move  $11, $v1
    move  $v1, $11
    jr  $ra

iswronggcd_begin_:
    lw  $8, -16($fp)
    lw  $9, -12($fp)
    bgt  $9, 0, _362_label_
    li  $v1, 1
    jr  $ra

_362_label_:
    bgt  $8, 0, _365_label_
    li  $v1, 1
    jr  $ra

_365_label_:
    li  $v1, 0
    jr  $ra

gcd_test_begin_:
    lw  $8, -16($fp)
    lw  $9, -12($fp)
    li  $v0, 4
    la  $a0, str_17
    syscall
    li  $v0, 11
    li  $a0, 10
    syscall
    li  $v0, 5
    syscall
    move  $9, $v0
    li  $v0, 5
    syscall
    move  $8, $v0
    sw  $ra, 0($fp)
    sw  $fp, -4($sp)
    addi  $sp, $sp, -12
    sw  $9, 0($sp)
    addi  $sp, $sp, -4
    sw  $8, 0($sp)
    addi  $sp, $sp, -4
    sw  $8, -16($fp)
    sw  $9, -12($fp)
    subi  $fp, $sp, -20
    addi  $sp, $sp, 0
    jal  iswronggcd_begin_

call_376_:
    move  $sp, $fp
    lw  $fp, -4($fp)
    lw  $ra, 0($fp)
    lw  $8, -16($fp)
    lw  $9, -12($fp)
    move  $10, $v1
    sw  $10, -20($fp)
    ble  $10, 0, _390_label_

_379_label_:
    li  $v0, 4
    la  $a0, str_18
    syscall
    li  $v0, 11
    li  $a0, 10
    syscall
    li  $v0, 5
    syscall
    move  $9, $v0
    li  $v0, 5
    syscall
    move  $8, $v0
    sw  $ra, 0($fp)
    sw  $fp, -4($sp)
    addi  $sp, $sp, -12
    sw  $9, 0($sp)
    addi  $sp, $sp, -4
    sw  $8, 0($sp)
    addi  $sp, $sp, -4
    sw  $8, -16($fp)
    sw  $9, -12($fp)
    subi  $fp, $sp, -20
    addi  $sp, $sp, 0
    jal  iswronggcd_begin_

call_387_:
    move  $sp, $fp
    lw  $fp, -4($fp)
    lw  $ra, 0($fp)
    lw  $8, -16($fp)
    lw  $9, -12($fp)
    move  $10, $v1
    sw  $10, -24($fp)
    bgt  $10, 0, _379_label_

_390_label_:
    sw  $ra, 0($fp)
    sw  $fp, -4($sp)
    addi  $sp, $sp, -12
    sw  $9, 0($sp)
    addi  $sp, $sp, -4
    sw  $8, 0($sp)
    addi  $sp, $sp, -4
    sw  $8, -16($fp)
    sw  $9, -12($fp)
    subi  $fp, $sp, -20
    addi  $sp, $sp, -12
    jal  gcd_begin_

call_395_:
    move  $sp, $fp
    lw  $fp, -4($fp)
    lw  $ra, 0($fp)
    lw  $8, -16($fp)
    lw  $9, -12($fp)
    move  $10, $v1
    li  $v0, 4
    la  $a0, str_19
    syscall
    li  $v0, 1
    move  $a0, $10
    syscall
    li  $v0, 11
    li  $a0, 10
    syscall
    jr  $ra

is_prime_begin_:
    lw  $8, -16($fp)
    lw  $9, -12($fp)
    li  $8, 2
    bne  $9, 2, _405_label_
    li  $v1, 1
    jr  $ra

_405_label_:

_406_label_:
    sw  $ra, 0($fp)
    sw  $fp, -4($sp)
    addi  $sp, $sp, -12
    sw  $9, 0($sp)
    addi  $sp, $sp, -4
    sw  $8, 0($sp)
    addi  $sp, $sp, -4
    sw  $8, -16($fp)
    sw  $9, -12($fp)
    subi  $fp, $sp, -20
    addi  $sp, $sp, -12
    jal  mod_begin_

call_411_:
    move  $sp, $fp
    lw  $fp, -4($fp)
    lw  $ra, 0($fp)
    lw  $8, -16($fp)
    lw  $9, -12($fp)
    move  $10, $v1
    sw  $10, -20($fp)
    bne  $10, 0, _415_label_
    li  $v1, 0
    jr  $ra

_415_label_:
    move  $10, $8
    addi  $8, $10, 1
    mul  $11, $8, $8
    sw  $10, -24($fp)
    sw  $11, -28($fp)
    blt  $11, $9, _406_label_
    li  $v1, 1
    jr  $ra

prime_factorization_begin_:
    lw  $8, -20($fp)
    lw  $9, -16($fp)
    lw  $10, -12($fp)
    li  $9, 2
    li  $8, 0
    bne  $10, 1, _429_label_
    li  $v0, 4
    la  $a0, str_20
    syscall
    li  $v0, 11
    li  $a0, 10
    syscall
    jr  $ra

_429_label_:

_430_label_:
    sw  $ra, 0($fp)
    sw  $fp, -4($sp)
    addi  $sp, $sp, -12
    sw  $9, 0($sp)
    addi  $sp, $sp, -4
    sw  $8, -20($fp)
    sw  $9, -16($fp)
    sw  $10, -12($fp)
    subi  $fp, $sp, -16
    addi  $sp, $sp, -16
    jal  is_prime_begin_

call_434_:
    move  $sp, $fp
    lw  $fp, -4($fp)
    lw  $ra, 0($fp)
    lw  $8, -20($fp)
    lw  $9, -16($fp)
    lw  $10, -12($fp)
    move  $11, $v1
    sw  $11, -24($fp)
    bne  $11, 1, _460_label_
    sw  $ra, 0($fp)
    sw  $fp, -4($sp)
    addi  $sp, $sp, -12
    sw  $10, 0($sp)
    addi  $sp, $sp, -4
    sw  $9, 0($sp)
    addi  $sp, $sp, -4
    sw  $8, -20($fp)
    sw  $9, -16($fp)
    sw  $10, -12($fp)
    subi  $fp, $sp, -20
    addi  $sp, $sp, -12
    jal  mod_begin_

call_441_:
    move  $sp, $fp
    lw  $fp, -4($fp)
    lw  $ra, 0($fp)
    lw  $8, -20($fp)
    lw  $9, -16($fp)
    lw  $10, -12($fp)
    move  $11, $v1
    sw  $11, -28($fp)
    bne  $11, 0, _459_label_

_444_label_:
    move  $11, $10
    div  $10, $11, $9
    sw  $11, -40($fp)
    bne  $8, 1, _449_label_
    li  $v0, 4
    la  $a0, str_21
    syscall
    li  $v0, 11
    li  $a0, 10
    syscall

_449_label_:
    li  $8, 1
    li  $v0, 1
    move  $a0, $9
    syscall
    li  $v0, 11
    li  $a0, 10
    syscall
    sw  $ra, 0($fp)
    sw  $fp, -4($sp)
    addi  $sp, $sp, -12
    sw  $10, 0($sp)
    addi  $sp, $sp, -4
    sw  $9, 0($sp)
    addi  $sp, $sp, -4
    sw  $8, -20($fp)
    sw  $9, -16($fp)
    sw  $10, -12($fp)
    subi  $fp, $sp, -20
    addi  $sp, $sp, -12
    jal  mod_begin_

call_456_:
    move  $sp, $fp
    lw  $fp, -4($fp)
    lw  $ra, 0($fp)
    lw  $8, -20($fp)
    lw  $9, -16($fp)
    lw  $10, -12($fp)
    move  $11, $v1
    sw  $11, -36($fp)
    beq  $11, 0, _444_label_

_459_label_:

_460_label_:
    move  $11, $9
    addi  $9, $11, 1
    mul  $12, $9, $9
    sw  $11, -32($fp)
    sw  $12, -44($fp)
    blt  $12, $10, _430_label_
    beq  $10, 1, _470_label_
    bne  $8, 1, _468_label_
    li  $v0, 4
    la  $a0, str_22
    syscall
    li  $v0, 11
    li  $a0, 10
    syscall

_468_label_:
    li  $v0, 1
    move  $a0, $10
    syscall
    li  $v0, 11
    li  $a0, 10
    syscall

_470_label_:
    jr  $ra

prime_factorization_test_begin_:
    lw  $8, -12($fp)
    li  $v0, 4
    la  $a0, str_23
    syscall
    li  $v0, 11
    li  $a0, 10
    syscall
    li  $v0, 5
    syscall
    move  $8, $v0

_476_label_:
    bge  $8, 0, _480_label_
    li  $v0, 4
    la  $a0, str_24
    syscall
    li  $v0, 11
    li  $a0, 10
    syscall
    li  $v0, 5
    syscall
    move  $8, $v0

_480_label_:
    blt  $8, 0, _476_label_
    li  $v0, 4
    la  $a0, str_25
    syscall
    li  $v0, 11
    li  $a0, 10
    syscall
    sw  $ra, 0($fp)
    sw  $fp, -4($sp)
    addi  $sp, $sp, -12
    sw  $8, 0($sp)
    addi  $sp, $sp, -4
    sw  $8, -12($fp)
    subi  $fp, $sp, -16
    addi  $sp, $sp, -32
    jal  prime_factorization_begin_

call_486_:
    move  $sp, $fp
    lw  $fp, -4($fp)
    lw  $ra, 0($fp)
    lw  $8, -12($fp)
    jr  $ra

iswrong_begin_:
    lw  $8, -12($fp)
    li  $9, 69
    bne  $9, $8, _493_label_
    li  $v1, 0
    jr  $ra

_493_label_:
    li  $9, 77
    bne  $9, $8, _496_label_
    li  $v1, 0
    jr  $ra

_496_label_:
    li  $9, 72
    bne  $9, $8, _499_label_
    li  $v1, 0
    jr  $ra

_499_label_:
    li  $v1, 1
    jr  $ra

main:
    la  $ra, exit
    move  $fp, $sp
    addi  $sp, $sp, -24
    lw  $8, -12($fp)
    li  $v0, 4
    la  $a0, str_26
    syscall
    li  $v0, 11
    li  $a0, 10
    syscall
    li  $v0, 12
    syscall
    move  $8, $v0
    sw  $ra, 0($fp)
    sw  $fp, -4($sp)
    addi  $sp, $sp, -12
    sw  $8, 0($sp)
    addi  $sp, $sp, -4
    sw  $8, -12($fp)
    subi  $fp, $sp, -16
    addi  $sp, $sp, 0
    jal  iswrong_begin_

call_508_:
    move  $sp, $fp
    lw  $fp, -4($fp)
    lw  $ra, 0($fp)
    lw  $8, -12($fp)
    move  $9, $v1
    sw  $9, -16($fp)
    ble  $9, 0, _520_label_

_511_label_:
    li  $v0, 4
    la  $a0, str_27
    syscall
    li  $v0, 11
    li  $a0, 10
    syscall
    li  $v0, 12
    syscall
    move  $8, $v0
    sw  $ra, 0($fp)
    sw  $fp, -4($sp)
    addi  $sp, $sp, -12
    sw  $8, 0($sp)
    addi  $sp, $sp, -4
    sw  $8, -12($fp)
    subi  $fp, $sp, -16
    addi  $sp, $sp, 0
    jal  iswrong_begin_

call_517_:
    move  $sp, $fp
    lw  $fp, -4($fp)
    lw  $ra, 0($fp)
    lw  $8, -12($fp)
    move  $9, $v1
    sw  $9, -20($fp)
    bgt  $9, 0, _511_label_

_520_label_:
    bne  $8, 69, _532_label_
    sw  $ra, 0($fp)
    sw  $fp, -4($sp)
    addi  $sp, $sp, -12
    sw  $8, -12($fp)
    subi  $fp, $sp, -12
    addi  $sp, $sp, 0
    jal  define_test_begin_

call_524_:
    move  $sp, $fp
    lw  $fp, -4($fp)
    lw  $ra, 0($fp)
    lw  $8, -12($fp)
    sw  $ra, 0($fp)
    sw  $fp, -4($sp)
    addi  $sp, $sp, -12
    sw  $8, -12($fp)
    subi  $fp, $sp, -12
    addi  $sp, $sp, 0
    jal  operator_test_begin_

call_527_:
    move  $sp, $fp
    lw  $fp, -4($fp)
    lw  $ra, 0($fp)
    lw  $8, -12($fp)
    sw  $ra, 0($fp)
    sw  $fp, -4($sp)
    addi  $sp, $sp, -12
    sw  $8, -12($fp)
    subi  $fp, $sp, -12
    addi  $sp, $sp, -8
    jal  fibonaci_test_begin_

call_530_:
    move  $sp, $fp
    lw  $fp, -4($fp)
    lw  $ra, 0($fp)
    lw  $8, -12($fp)
    j  switch_521_end_

_532_label_:
    bne  $8, 77, _547_label_
    sw  $ra, 0($fp)
    sw  $fp, -4($sp)
    addi  $sp, $sp, -12
    sw  $8, -12($fp)
    subi  $fp, $sp, -12
    addi  $sp, $sp, 0
    jal  dowhile_test_begin_

call_536_:
    move  $sp, $fp
    lw  $fp, -4($fp)
    lw  $ra, 0($fp)
    lw  $8, -12($fp)
    sw  $ra, 0($fp)
    sw  $fp, -4($sp)
    addi  $sp, $sp, -12
    sw  $8, -12($fp)
    subi  $fp, $sp, -12
    addi  $sp, $sp, -4
    jal  condition_test_begin_

call_539_:
    move  $sp, $fp
    lw  $fp, -4($fp)
    lw  $ra, 0($fp)
    lw  $8, -12($fp)
    sw  $ra, 0($fp)
    sw  $fp, -4($sp)
    addi  $sp, $sp, -12
    sw  $8, -12($fp)
    subi  $fp, $sp, -12
    addi  $sp, $sp, 0
    jal  string_test_begin_

call_542_:
    move  $sp, $fp
    lw  $fp, -4($fp)
    lw  $ra, 0($fp)
    lw  $8, -12($fp)
    sw  $ra, 0($fp)
    sw  $fp, -4($sp)
    addi  $sp, $sp, -12
    sw  $8, -12($fp)
    subi  $fp, $sp, -12
    addi  $sp, $sp, -20
    jal  gcd_test_begin_

call_545_:
    move  $sp, $fp
    lw  $fp, -4($fp)
    lw  $ra, 0($fp)
    lw  $8, -12($fp)
    j  switch_521_end_

_547_label_:
    bne  $8, 72, _571_label_
    sw  $ra, 0($fp)
    sw  $fp, -4($sp)
    addi  $sp, $sp, -12
    sw  $8, -12($fp)
    subi  $fp, $sp, -12
    addi  $sp, $sp, 0
    jal  define_test_begin_

call_551_:
    move  $sp, $fp
    lw  $fp, -4($fp)
    lw  $ra, 0($fp)
    lw  $8, -12($fp)
    sw  $ra, 0($fp)
    sw  $fp, -4($sp)
    addi  $sp, $sp, -12
    sw  $8, -12($fp)
    subi  $fp, $sp, -12
    addi  $sp, $sp, 0
    jal  operator_test_begin_

call_554_:
    move  $sp, $fp
    lw  $fp, -4($fp)
    lw  $ra, 0($fp)
    lw  $8, -12($fp)
    sw  $ra, 0($fp)
    sw  $fp, -4($sp)
    addi  $sp, $sp, -12
    sw  $8, -12($fp)
    subi  $fp, $sp, -12
    addi  $sp, $sp, 0
    jal  dowhile_test_begin_

call_557_:
    move  $sp, $fp
    lw  $fp, -4($fp)
    lw  $ra, 0($fp)
    lw  $8, -12($fp)
    sw  $ra, 0($fp)
    sw  $fp, -4($sp)
    addi  $sp, $sp, -12
    sw  $8, -12($fp)
    subi  $fp, $sp, -12
    addi  $sp, $sp, -4
    jal  condition_test_begin_

call_560_:
    move  $sp, $fp
    lw  $fp, -4($fp)
    lw  $ra, 0($fp)
    lw  $8, -12($fp)
    sw  $ra, 0($fp)
    sw  $fp, -4($sp)
    addi  $sp, $sp, -12
    sw  $8, -12($fp)
    subi  $fp, $sp, -12
    addi  $sp, $sp, 0
    jal  string_test_begin_

call_563_:
    move  $sp, $fp
    lw  $fp, -4($fp)
    lw  $ra, 0($fp)
    lw  $8, -12($fp)
    sw  $ra, 0($fp)
    sw  $fp, -4($sp)
    addi  $sp, $sp, -12
    sw  $8, -12($fp)
    subi  $fp, $sp, -12
    addi  $sp, $sp, 0
    jal  miscellaneous_test_begin_

call_566_:
    move  $sp, $fp
    lw  $fp, -4($fp)
    lw  $ra, 0($fp)
    lw  $8, -12($fp)
    sw  $ra, 0($fp)
    sw  $fp, -4($sp)
    addi  $sp, $sp, -12
    sw  $8, -12($fp)
    subi  $fp, $sp, -12
    addi  $sp, $sp, -4
    jal  prime_factorization_test_begin_

call_569_:
    move  $sp, $fp
    lw  $fp, -4($fp)
    lw  $ra, 0($fp)
    lw  $8, -12($fp)
    j  switch_521_end_

_571_label_:

switch_521_end_:
    jr  $ra
exit:
