#pragma once
#include "isa.h"

// Global instructions are similar to Flat instructions,but the programmer must ensure
// that no threads access LDS space; thus, no LDS bandwidth is used by global
// instructions. These instructions also allow direct data movement between LDS and memory
// without going through VGPRs.

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
  s_endpgm,
  s_load_dwordx2,
  v_mov_b32_e32,
  global_store_dwordx2,
  global_store_dwordx4,
  global_store_dword,
  global_store_short,
  s_load_dword,
  s_and_b32,
  s_mul_i32,
  v_add_u32_e32,
  v_add_co_u32_e32,
  v_xor_b32_e32,
  v_cmp_gt_u32_e32,
  s_and_saveexec_b64,
  s_cbranch_execz,
  v_or_b32_e32,
  s_xor_b64,
  v_and_b32_e32,
  v_not_b32_e32,
  v_cndmask_b32_e64,
  v_cmp_lt_i32_e64,
  v_lshrrev_b64,
  s_or_saveexec_b64,
  v_cmp_ne_u64_e32,
  v_cndmask_b32_e32,
  s_lshl_b64,
  v_cmp_lt_i32_e32,
  s_add_i32,
  v_addc_co_u32_e32

};

const operation ISA[] = {
    OP(NOP, SCALER, false, 1, NOP, REGS(), REGS()),
    OP(s_buffer_load_dwordx2, SCALER, false, 1,
       Load 2 dwords from ro memory via constant cache, REGS(2), REGS(0, 1)),
    OP(s_bfe_u32, SCALER, false, 1, Bit Field Extract, REGS(1, 2), REGS(0)),
    OP(s_mov_b32, SCALER, false, 1, generic 32bit move, REGS(1), REGS(0)),
    OP(s_add_u32, SCALER, false, 1, generic 32bit add, REGS(1, 2), REGS(0)),
    OP(s_addc_u32, SCALER, false, 1, generic 32bit add + SCC, REGS(1, 2), REGS(0)),
    OP(v_mov_b32, VECTOR, false, 1, generic 32bit move, REGS(1), REGS(0)),
    OP(flat_store_byte, FLAT, false, 1, write(1) to somewhere(0), REGS(1), REGS(0)),
    OP(s_waitcnt, SCALER, false, 1, , REGS(), REGS()),
    OP(s_endpgm, SCALER, false, 1, , REGS(), REGS()),
    // technically these are gfx900
    OP(s_load_dword, SCALER, false, 1, , REGS(1), REGS(0)),         // todo check
    OP(s_load_dwordx2, SCALER, false, 1, , REGS(TODO), REGS(TODO)), // todo check
    OP(v_mov_b32_e32, VECTOR, false, 1, "generic 32bit move", REGS(1), REGS(0)),
    OP(global_store_dwordx2, GLOBAL, false, 1, , REGS(1), REGS(0)),
    OP(global_store_dword, GLOBAL, false, 1, , REGS(1), REGS(0)),
    OP(global_store_short, GLOBAL, false, 1, , REGS(1), REGS(0)),
    OP(global_store_dwordx4, GLOBAL, false, 1, , REGS(), REGS()), // todo
    //
    OP(s_and_b32, SCALER, false, 1, , REGS(1, 2), REGS(0)),
    OP(s_mul_i32, SCALER, false, 1, , REGS(1, 2), REGS(0)),
    OP(v_add_u32_e32, VECTOR, false, 1, , REGS(1, 2), REGS(0)),
    OP(v_add_co_u32_e32, VECTOR, false, 1, , REGS(1, 2), REGS(0)),
    OP(v_xor_b32_e32, VECTOR, false, 1, , REGS(1, 2), REGS(0)),
    OP(v_cmp_gt_u32_e32, VECTOR, false, 1, , REGS(1, 2), REGS(0)),
    OP(s_and_saveexec_b64, SCALER, false, 1, , REGS(1), REGS(0)), // todo check
    OP(s_cbranch_execz, SCALER, true, 1, , REGS(0), REGS(TODO)),  // todo check
    OP(s_xor_b64, SCALER, false, 1, , REGS(), REGS()),            // todo
    OP(v_or_b32_e32, VECTOR, false, 1, , REGS(), REGS()),         // todo
    OP(v_and_b32_e32, VECTOR, false, 1, , REGS(), REGS()),        // todo
    OP(v_not_b32_e32, VECTOR, false, 1, , REGS(), REGS()),        // todo
    OP(v_cndmask_b32_e64, VECTOR, false, 1, , REGS(), REGS()),    // todo
    OP(v_cmp_lt_i32_e64, VECTOR, false, 1, , REGS(), REGS()),     // todo
    OP(v_lshrrev_b64, VECTOR, false, 1, , REGS(), REGS()),        // todo

    OP(s_or_saveexec_b64, VECTOR, false, 1, , REGS(), REGS()), // todo
    OP(v_cmp_ne_u64_e32, VECTOR, false, 1, , REGS(), REGS()),  // todo
    OP(v_cndmask_b32_e32, VECTOR, false, 1, , REGS(), REGS()), // todo
    OP(s_lshl_b64, VECTOR, false, 1, , REGS(), REGS()),        // todo
    OP(v_cmp_lt_i32_e32, VECTOR, false, 1, , REGS(), REGS()),  // todo
    OP(s_add_i32, SCALER, false, 1, , REGS(), REGS()),         // todo
    OP(v_addc_co_u32_e32, VECTOR, false, 1, , REGS(), REGS()),  // todo

};
