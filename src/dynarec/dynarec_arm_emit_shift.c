#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <pthread.h>
#include <errno.h>

#include "debug.h"
#include "box86context.h"
#include "dynarec.h"
#include "emu/x86emu_private.h"
#include "emu/x86run_private.h"
#include "x86run.h"
#include "x86emu.h"
#include "box86stack.h"
#include "callback.h"
#include "emu/x86run_private.h"
#include "x86trace.h"
#include "dynarec_arm.h"
#include "dynarec_arm_private.h"
#include "arm_printer.h"
#include "../tools/bridge_private.h"

#include "dynarec_arm_functions.h"
#include "dynarec_arm_helper.h"

// emit SHL32 instruction, from s1 , constant c, store result in s1 using s3 and s4 as scratch
void emit_shl32c(dynarec_arm_t* dyn, int ninst, int s1, int32_t c, int s3, int s4)
{
    IFX(X_PEND) {
        MOVW(s3, c);
        STR_IMM9(s1, xEmu, offsetof(x86emu_t, op1));
        STR_IMM9(s3, xEmu, offsetof(x86emu_t, op2));
        MOVW(s4, d_shl32);
    } else IFX(X_ALL) {
        MOVW(s4, d_none);
    }
    IFX(X_PEND|X_ALL) {
        STR_IMM9(s4, xEmu, offsetof(x86emu_t, df));
    }
    if(c==0) {
        IFX(F_OF) {
            BIC_IMM8(xFlags, xFlags, 0b10, 0x0b);
        }
        IFX(X_PEND) {
            STR_IMM9(s1, xEmu, offsetof(x86emu_t, res));
        }
        return;
    }
    IFX(X_CF) {
        MOV_REG_LSR_IMM5(s3, s1, 32-c);
        BFI(xFlags, s3, F_CF, 1);
    }
    IFX(X_ZF|X_OF) {
        MOVS_REG_LSL_IMM5(s1, s1, c);
    } else {
        MOV_REG_LSL_IMM5(s1, s1, c);
    }
    IFX(X_PEND) {
        STR_IMM9(s1, xEmu, offsetof(x86emu_t, res));
    }
    IFX(X_ZF) {
        ORR_IMM8_COND(cEQ, xFlags, xFlags, 1<<F_ZF, 0);
        BIC_IMM8_COND(cNE, xFlags, xFlags, 1<<F_ZF, 0);
    }
    IFX(X_SF) {
        MOV_REG_LSR_IMM5(s3, s1, 31);
        BFI(xFlags, s3, F_SF, 1);
    }
    IFX(X_OF) {
        if(c==1) {
            IFX(X_SF) {} else {MOV_REG_LSR_IMM5(s3, s1, 31);}
            XOR_IMM8_COND(cCS, s3, s3, 1);
            BFI(xFlags, s3, F_OF, 1);
        } else {
            BFC(xFlags, F_OF, 1);
        }
    }
    IFX(X_PF) {
        // PF: (((emu->x86emu_parity_tab[(res) / 32] >> ((res) % 32)) & 1) == 0)
        AND_IMM8(s3, s1, 0xE0); // lsr 5 masking pre-applied
        LDR_IMM9(s4, xEmu, offsetof(x86emu_t, x86emu_parity_tab));
        LDR_REG_LSR_IMM5(s4, s4, s3, 5-2);   // x/32 and then *4 because array is integer
        AND_IMM8(s3, s1, 31);
        MVN_REG_LSR_REG(s4, s4, s3);
        BFI(xFlags, s4, F_PF, 1);
    }
}

// emit SHR32 instruction, from s1 , constant c, store result in s1 using s3 and s4 as scratch
void emit_shr32c(dynarec_arm_t* dyn, int ninst, int s1, int32_t c, int s3, int s4)
{
    IFX(X_PEND) {
        MOVW(s3, c);
        STR_IMM9(s1, xEmu, offsetof(x86emu_t, op1));
        STR_IMM9(s3, xEmu, offsetof(x86emu_t, op2));
        MOVW(s4, d_shr32);
    } else IFX(X_ALL) {
        MOVW(s4, d_none);
    }
    IFX(X_PEND|X_ALL) {
        STR_IMM9(s4, xEmu, offsetof(x86emu_t, df));
    }
    if(!c) {
        IFX(X_PEND) {
            STR_IMM9(s1, xEmu, offsetof(x86emu_t, res));
        }
        return;
    }
    IFX(X_ZF|X_CF) {
        MOVS_REG_LSR_IMM5(s1, s1, c);
    } else {
        MOV_REG_LSR_IMM5(s1, s1, c);
    }
    IFX(X_PEND) {
        STR_IMM9(s1, xEmu, offsetof(x86emu_t, res));
    }
    IFX(X_ZF) {
        ORR_IMM8_COND(cEQ, xFlags, xFlags, 1<<F_ZF, 0);
        BIC_IMM8_COND(cNE, xFlags, xFlags, 1<<F_ZF, 0);
    }
    IFX(X_CF) {
        ORR_IMM8_COND(cCS, xFlags, xFlags, 1<<F_CF, 0);
        BIC_IMM8_COND(cCC, xFlags, xFlags, 1<<F_CF, 0);
    }
    IFX(X_SF) {
        MOV_REG_LSR_IMM5(s3, s1, 31);
        BFI(xFlags, s3, F_SF, 1);
    }
    IFX(X_OF) {
        if(c==1) {
            MOV_REG_LSR_IMM5(s4, s1, 30);
            XOR_REG_LSR_IMM8(s4, s4, s4, 1);
            BFI(xFlags, s4, F_OF, 1);
        }
    }
    IFX(X_PF) {
        // PF: (((emu->x86emu_parity_tab[(res) / 32] >> ((res) % 32)) & 1) == 0)
        AND_IMM8(s3, s1, 0xE0); // lsr 5 masking pre-applied
        LDR_IMM9(s4, xEmu, offsetof(x86emu_t, x86emu_parity_tab));
        LDR_REG_LSR_IMM5(s4, s4, s3, 5-2);   // x/32 and then *4 because array is integer
        AND_IMM8(s3, s1, 31);
        MVN_REG_LSR_REG(s4, s4, s3);
        BFI(xFlags, s4, F_PF, 1);
    }
}

// emit SAR32 instruction, from s1 , constant c, store result in s1 using s3 and s4 as scratch
void emit_sar32c(dynarec_arm_t* dyn, int ninst, int s1, int32_t c, int s3, int s4)
{
    IFX(X_PEND) {
        MOVW(s3, c);
        STR_IMM9(s1, xEmu, offsetof(x86emu_t, op1));
        STR_IMM9(s3, xEmu, offsetof(x86emu_t, op2));
        MOVW(s4, d_sar32);
    } else IFX(X_ALL) {
        MOVW(s4, d_none);
    }
    IFX(X_PEND|X_ALL) {
        STR_IMM9(s4, xEmu, offsetof(x86emu_t, df));
    }
    if(!c) {
        IFX(X_PEND) {
            STR_IMM9(s1, xEmu, offsetof(x86emu_t, res));
        }
        return;
    }
    IFX(X_ZF|X_CF) {
        MOVS_REG_ASR_IMM5(s1, s1, c);
    } else {
        MOV_REG_ASR_IMM5(s1, s1, c);
    }
    IFX(X_PEND) {
        STR_IMM9(s1, xEmu, offsetof(x86emu_t, res));
    }
    IFX(X_ZF) {
        ORR_IMM8_COND(cEQ, xFlags, xFlags, 1<<F_ZF, 0);
        BIC_IMM8_COND(cNE, xFlags, xFlags, 1<<F_ZF, 0);
    }
    IFX(X_CF) {
        ORR_IMM8_COND(cCS, xFlags, xFlags, 1<<F_CF, 0);
        BIC_IMM8_COND(cCC, xFlags, xFlags, 1<<F_CF, 0);
    }
    IFX(X_SF) {
        MOV_REG_LSR_IMM5(s3, s1, 31);
        BFI(xFlags, s3, F_SF, 1);
    }
    IFX(X_PF) {
        // PF: (((emu->x86emu_parity_tab[(res) / 32] >> ((res) % 32)) & 1) == 0)
        AND_IMM8(s3, s1, 0xE0); // lsr 5 masking pre-applied
        LDR_IMM9(s4, xEmu, offsetof(x86emu_t, x86emu_parity_tab));
        LDR_REG_LSR_IMM5(s4, s4, s3, 5-2);   // x/32 and then *4 because array is integer
        AND_IMM8(s3, s1, 31);
        MVN_REG_LSR_REG(s4, s4, s3);
        BFI(xFlags, s4, F_PF, 1);
    }
}

// emit ROL32 instruction, from s1 , constant c, store result in s1 using s3 and s4 as scratch
void emit_rol32c(dynarec_arm_t* dyn, int ninst, int s1, int32_t c, int s3, int s4)
{
    IFX(X_PEND) {
        MOVW(s3, c);
        STR_IMM9(s3, xEmu, offsetof(x86emu_t, op2));
        MOVW(s4, d_rol32);
    } else IFX(X_ALL) {
        MOVW(s4, d_none);
    }
    IFX(X_PEND|X_ALL) {
        STR_IMM9(s4, xEmu, offsetof(x86emu_t, df));
    }
    if(!c) {
        IFX(X_PEND) {
            STR_IMM9(s1, xEmu, offsetof(x86emu_t, res));
        }
        return;
    }
    MOV_REG_ROR_IMM5(s1, s1, 32-c);
    IFX(X_PEND) {
        STR_IMM9(s1, xEmu, offsetof(x86emu_t, res));
    }
    IFX(X_CF) {
        BFI(xFlags, s1, F_CF, 1);
    }
    IFX(X_OF) {
        if(c==1) {
            ADD_REG_LSR_IMM5(s3, s1, s1, 31);
            AND_IMM8(s3, s3, 1);
            BFI(xFlags, s3, F_OF, 1);
        }
    }
}

// emit ROR32 instruction, from s1 , constant c, store result in s1 using s3 and s4 as scratch
void emit_ror32c(dynarec_arm_t* dyn, int ninst, int s1, int32_t c, int s3, int s4)
{
    IFX(X_PEND) {
        MOVW(s3, c);
        STR_IMM9(s3, xEmu, offsetof(x86emu_t, op2));
        MOVW(s4, d_ror32);
    } else IFX(X_ALL) {
        MOVW(s4, d_none);
    }
    IFX(X_PEND|X_ALL) {
        STR_IMM9(s4, xEmu, offsetof(x86emu_t, df));
    }
    if(!c) {
        IFX(X_PEND) {
            STR_IMM9(s1, xEmu, offsetof(x86emu_t, res));
        }
        return;
    }
    MOV_REG_ROR_IMM5(s1, s1, c);
    IFX(X_PEND) {
        STR_IMM9(s1, xEmu, offsetof(x86emu_t, res));
    }
    IFX(X_CF) {
        MOV_REG_LSR_IMM5(s3, s1, 31);
        BFI(xFlags, s3, F_CF, 1);
    }
    IFX(X_OF) {
        if(c==1) {
            MOV_REG_LSR_IMM5(s4, s1, 30);
            XOR_REG_LSR_IMM8(s4, s4, s4, 1);
            BFI(xFlags, s4, F_OF, 1);
        }
    }
}

// emit SHRD32 instruction, from s1, fill s2 , constant c, store result in s1 using s3 and s4 as scratch
void emit_shrd32c(dynarec_arm_t* dyn, int ninst, int s1, int s2, int32_t c, int s3, int s4)
{
    IFX(X_PEND) {
        MOVW(s3, c);
        STR_IMM9(s1, xEmu, offsetof(x86emu_t, op1));
        STR_IMM9(s3, xEmu, offsetof(x86emu_t, op2));
        // same flags calc as shr32
        MOVW(s4, d_shr32);
    } else IFX(X_ALL) {
        MOVW(s4, d_none);
    }
    IFX(X_PEND|X_ALL) {
        STR_IMM9(s4, xEmu, offsetof(x86emu_t, df));
    }
    if(!c) {
        IFX(X_PEND) {
            STR_IMM9(s1, xEmu, offsetof(x86emu_t, res));
        }
        return;
    }
    IFX(X_CF) {
        MOVS_REG_LSR_IMM5(s1, s1, c);
    } else {
        MOV_REG_LSR_IMM5(s1, s1, c);
    }
    IFX(X_CF) {
        ORR_IMM8_COND(cCS, xFlags, xFlags, 1<<F_CF, 0);
        BIC_IMM8_COND(cCC, xFlags, xFlags, 1<<F_CF, 0);
    }
    IFX(X_ZF) {
        ORRS_REG_LSL_IMM5(s1, s1, s2, 32-c);
    } else {
        ORR_REG_LSL_IMM5(s1, s1, s2, 32-c);
    }
    IFX(X_PEND) {
        STR_IMM9(s1, xEmu, offsetof(x86emu_t, res));
    }
    IFX(X_ZF) {
        ORR_IMM8_COND(cEQ, xFlags, xFlags, 1<<F_ZF, 0);
        BIC_IMM8_COND(cNE, xFlags, xFlags, 1<<F_ZF, 0);
    }
    IFX(X_SF) {
        MOV_REG_LSR_IMM5(s3, s1, 31);
        BFI(xFlags, s3, F_SF, 1);
    }
    IFX(X_OF) {
        if(c==1) {
            MOV_REG_LSR_IMM5(s4, s1, 30);
            XOR_REG_LSR_IMM8(s4, s4, s4, 1);
            BFI(xFlags, s4, F_OF, 1);
        }
    }
    IFX(X_PF) {
        // PF: (((emu->x86emu_parity_tab[(res) / 32] >> ((res) % 32)) & 1) == 0)
        AND_IMM8(s3, s1, 0xE0); // lsr 5 masking pre-applied
        LDR_IMM9(s4, xEmu, offsetof(x86emu_t, x86emu_parity_tab));
        LDR_REG_LSR_IMM5(s4, s4, s3, 5-2);   // x/32 and then *4 because array is integer
        AND_IMM8(s3, s1, 31);
        MVN_REG_LSR_REG(s4, s4, s3);
        BFI(xFlags, s4, F_PF, 1);
    }
}

void emit_shld32c(dynarec_arm_t* dyn, int ninst, int s1, int s2, int32_t c, int s3, int s4)
{
    IFX(X_PEND) {
        MOVW(s3, c);
        STR_IMM9(s1, xEmu, offsetof(x86emu_t, op1));
        STR_IMM9(s3, xEmu, offsetof(x86emu_t, op2));
        MOVW(s4, d_shl32);  // same flags computation as with shl32
    } else IFX(X_ALL) {
        MOVW(s4, d_none);
    }
    IFX(X_PEND|X_ALL) {
        STR_IMM9(s4, xEmu, offsetof(x86emu_t, df));
    }
    if(c==0) {
        IFX(F_OF) {
            BFC(xFlags, F_OF, 1);
        }
        IFX(X_PEND) {
            STR_IMM9(s1, xEmu, offsetof(x86emu_t, res));
        }
        return;
    }
    IFX(X_CF) {
        MOV_REG_LSR_IMM5(s3, s1, 32-c);
        BFI(xFlags, s3, F_CF, 1);
    }
    IFX(X_OF) {
        MOVS_REG_LSL_IMM5(s1, s1, c);
    } else {
        MOV_REG_LSL_IMM5(s1, s1, c);
    }
    IFX(X_OF) {
        if(c==1) {
            UBFX(s3, s2, 0, 1);
            XOR_IMM8_COND(cCS, s3, s3, 1);
            BFI(xFlags, s3, F_OF, 1);
        } else {
            BFC(xFlags, F_OF, 1);
        }
    }
    IFX(X_ZF) {
        ORRS_REG_LSR_IMM5(s1, s1, s2, 32-c);
    } else {
        ORR_REG_LSR_IMM5(s1, s1, s2, 32-c);
    }

    IFX(X_PEND) {
        STR_IMM9(s1, xEmu, offsetof(x86emu_t, res));
    }
    IFX(X_ZF) {
        ORR_IMM8_COND(cEQ, xFlags, xFlags, 1<<F_ZF, 0);
        BIC_IMM8_COND(cNE, xFlags, xFlags, 1<<F_ZF, 0);
    }
    IFX(X_SF) {
        MOV_REG_LSR_IMM5(s3, s1, 31);
        BFI(xFlags, s3, F_SF, 1);
    }
    IFX(X_PF) {
        // PF: (((emu->x86emu_parity_tab[(res) / 32] >> ((res) % 32)) & 1) == 0)
        AND_IMM8(s3, s1, 0xE0); // lsr 5 masking pre-applied
        LDR_IMM9(s4, xEmu, offsetof(x86emu_t, x86emu_parity_tab));
        LDR_REG_LSR_IMM5(s4, s4, s3, 5-2);   // x/32 and then *4 because array is integer
        AND_IMM8(s3, s1, 31);
        MVN_REG_LSR_REG(s4, s4, s3);
        BFI(xFlags, s4, F_PF, 1);
    }
}
