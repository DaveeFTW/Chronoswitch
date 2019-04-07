	.set noreorder

.global getPowerAddress
.ent    getPowerAddress
getPowerAddress:
	addiu $sp, $sp, -8
	sw $ra, 0($sp)
	move $a1, $a0
	jal sceUtilityPowerRegisterCallback
	move $a0, $0
	move $v0, $v1
	lw $ra, 0($sp)
	jr $ra
	addiu $sp, $sp, 8
.end getPowerAddress