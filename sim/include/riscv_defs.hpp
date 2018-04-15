#pragma once

#include <cstdint>

// Using directives to set data widths for different CPU data types

// Data types used for registers
using reg_data_t = uint32_t;
using signed_reg_data_t = int32_t;

// Data types for instructions
using instr_t = uint32_t;

// Memory data types
using mem_addr_t = uint32_t;
using mem_offset_t = int32_t;
using pc_offset_t = int32_t;

// Immediate types
using imm_t = int32_t;
using s_imm_t = int32_t;

using word_t = uint32_t;
using signed_word_t = int32_t;
