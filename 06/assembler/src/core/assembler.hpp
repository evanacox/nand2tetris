#pragma once

#include "instructions.hpp"
#include <bitset>
#include <cstdint>
#include <memory>
#include <unordered_map>
#include <vector>

namespace hack
{

class Assembler
{
public:
    Assembler();

    /**
     * Takes an input string of Hack Assembly, and outputs a vector of
     * bitsets, each one being a Hack machine language instruction
     * @returns list of bitset<16>s
     */
    std::vector<std::bitset<16>> assemble(std::vector<std::shared_ptr<Instruction>> data);

private:
    /**
     * Represents the Assembly symbol table
     */
    std::unordered_map<std::string, int16_t> m_symbol_table;

    /**
     * The current line number
     */
    size_t m_line;

    /**
     * Puts all symbols into the symbol table in one pass
     */
    void get_symbols(std::vector<std::shared_ptr<Instruction>> &) noexcept;

    /**
     * Outputs an instruction for a particular line of assembly
     * @returns An std::bitset<16>, representing an A instruction
     */
    [[nodiscard]] std::bitset<16> a_instruction(std::shared_ptr<Instruction>) const noexcept;

    /**
     * Outputs an instruction for a C instruction line of assembly
     * @returns an std::bitset<16> representing a C instruction
     */
    [[nodiscard]] std::bitset<16> c_instruction(std::shared_ptr<Instruction>) const;

    /**
     * Resolves a symbol, returning the address for the symbol
     * @return An integer representing an address
     */
    int32_t resolve_symbol(std::string) const;
};

} // namespace hack
