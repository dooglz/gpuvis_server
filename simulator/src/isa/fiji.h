#pragma once
#include "isa.h"

enum ISAe {
  NOP,
  s_buffer_load_dwordx2,
  s_bfe_u32,
  s_mov_b32,
  s_add_u32,
  s_addc_u32,
  v_mov_b32,
  flat_store_byte,
  s_waitcnt,
  s_endpgm
};

const operation ISA[] = {
    OP(NOP, SCALER, false, 1, NOP, REGS(), REGS()),
    OP(s_buffer_load_dwordx2, SCALER, false, 1, Load 2 dwords from ro memory via constant cache, REGS(2), REGS(0, 1)),
    OP(s_bfe_u32, SCALER, false, 1, Bit Field Extract, REGS(1, 2), REGS(0)),
    OP(s_mov_b32, SCALER, false, 1, generic 32bit move, REGS(1), REGS(0)),
    OP(s_add_u32, SCALER, false, 1, generic 32bit add, REGS(1, 2), REGS(0)),
    OP(s_addc_u32, SCALER, false, 1, generic 32bit add + SCC, REGS(1, 2), REGS(0)),
    OP(v_mov_b32, VECTOR, false, 1, generic 32bit move, REGS(1), REGS(0)),
    OP(flat_store_byte, FLAT, false, 1, write(1) to somewhere(0), REGS(1), REGS()),
    OP(s_waitcnt, SCALER, false, 1, , REGS(), REGS()),
    OP(s_endpgm, SCALER, false, 1, , REGS(), REGS())};
