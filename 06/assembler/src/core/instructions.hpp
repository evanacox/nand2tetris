/*
  Compile-time lookup for the bits for various computations
  Since it's all done at compile-time, I don't need to worry about
  the find time
 */
#pragma once

#include <bitset>
#include <stdexcept>
#include <unordered_map>

namespace hack
{

/**
 * Type of instruction
 */
enum class InstructionType
{
    A_INSTR,
    C_INSTR,
    LABEL
};

/**
 * Base Instruction class
 */
struct Instruction
{
    InstructionType type;
    std::string raw;
};

/**
 * Represents an A Instruction
 */
struct AInstruction : public Instruction
{
    std::string constant;
    int16_t integer_constant;
    bool is_named;
};

/**
 * Represents a C instruction
 */
struct CInstruction : public Instruction
{
    std::string dest;
    std::string jmp;
    std::string computation;
    bool a_enabled;
};

/**
 * Represents a label
 */
struct Label : public Instruction
{
    std::string label;
};

} // namespace hack