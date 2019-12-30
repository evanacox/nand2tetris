#include "assembler.hpp"
#include "instructions.hpp"
#include <algorithm>
#include <cctype>
#include <cstdio>
#include <stdexcept>
#include <unordered_map>

/**
 * Lookup table for when the `a` bit is 0, meaning A is being used over M
 */
static std::unordered_map<std::string, std::string> using_a_map = {
    {"0", "101010"},   {"1", "111111"},   {"-1", "111010"},  {"D", "001100"},   {"A", "110000"},
    {"!D", "001101"},  {"!A", "110001"},  {"-D", "001111"},  {"-A", "110011"},  {"D+1", "011111"},
    {"A+1", "110111"}, {"D-1", "001110"}, {"A-1", "110010"}, {"D+A", "000010"}, {"D-A", "010011"},
    {"A-D", "000111"}, {"D&A", "000000"}, {"D|A", "010101"},
};

/**
 * Lookup table for when the `a` bit is 1, meaning M is being used over A
 */
static std::unordered_map<std::string, std::string> using_m_map = {
    {"M", "110000"},   {"!M", "110001"},  {"-M", "110011"},  {"M+1", "110111"}, {"M-1", "110010"},
    {"D+M", "000010"}, {"D-M", "010011"}, {"M-D", "000111"}, {"D&M", "000000"}, {"D|M", "010101"},
};

/**
 * Lookup table for jumps
 */
static std::unordered_map<std::string, std::string> jump_map = {
    {"nil", "000"}, {"JGT", "001"}, {"JEQ", "010"}, {"JGE", "011"},
    {"JLT", "100"}, {"JNE", "101"}, {"JLE", "110"}, {"JMP", "111"},
};

/**
 * Lookup table for destinations
 */
static std::unordered_map<std::string, std::string> dest_map = {
    {"nil", "000"}, {"M", "001"},  {"D", "010"},  {"MD", "011"},
    {"A", "100"},   {"AM", "101"}, {"AD", "110"}, {"AMD", "111"},
};

namespace hack
{

Assembler::Assembler() : m_line(0)
{
    m_symbol_table["SP"] = 0;
    m_symbol_table["LCL"] = 1;
    m_symbol_table["ARG"] = 2;
    m_symbol_table["THIS"] = 3;
    m_symbol_table["THAT"] = 4;
    m_symbol_table["SCREEN"] = 16384;
    m_symbol_table["KBD"] = 24576;

    for (auto i = 0; i < 16; ++i)
    {
        m_symbol_table["R" + std::to_string(i)] = i;
    }
}

std::vector<std::bitset<16>> Assembler::assemble(std::vector<std::shared_ptr<Instruction>> data)
{
    get_symbols(data);

    std::vector<std::bitset<16>> bits;

    for (auto &ptr : data)
    {
        switch (ptr->type)
        {
            case hack::InstructionType::A_INSTR:
                bits.push_back(a_instruction(ptr));
                break;
            case hack::InstructionType::C_INSTR:
                bits.push_back(c_instruction(ptr));
                break;
            case hack::InstructionType::LABEL:
                break;
        }
    }

    return bits;
}

void Assembler::get_symbols(std::vector<std::shared_ptr<Instruction>> &instructions) noexcept
{
    int16_t current_address =
        16; // pre-defined symbols cover 0-15, so 16 is the first "usable" address
    size_t line = 0;

    for (const auto &instruction : instructions)
    {
        if (instruction->type == hack::InstructionType::LABEL)
        {
            auto instr = reinterpret_cast<Label *>(instruction.get());

            // the ROM addresses start at 0, and line is incremented for every non-label line
            // due to this, the line is equal to the label line (and thus, the next instruction
            // for the rom)
            m_symbol_table[instr->label] = line;

            continue;
        }

        ++line; // line is only incremented for non-labels

        if (instruction->type == hack::InstructionType::A_INSTR)
        {
            auto instr = reinterpret_cast<AInstruction *>(instruction.get());

            // if it's unnamed, its just a number being loaded. if it's found,
            // it's predefined or it's been defined already
            if (!instr->is_named || m_symbol_table.find(instr->constant) != m_symbol_table.end())
                continue;

            m_symbol_table[instr->constant] = current_address++; // increment after assignment
        }
    }
}

std::bitset<16> Assembler::a_instruction(std::shared_ptr<Instruction> instruction) const noexcept
{
    auto instr = reinterpret_cast<AInstruction *>(instruction.get());

    if (instr->is_named)
    {
        return std::bitset<16>(m_symbol_table.at(instr->constant));
    }

    return std::bitset<16>(std::stoi(instr->constant));
}

std::bitset<16> Assembler::c_instruction(std::shared_ptr<Instruction> instruction) const
{
    auto instr = reinterpret_cast<CInstruction *>(instruction.get());

    std::string bits("111");

    bits += (instr->a_enabled) ? "1" : "0"; // a bit, whether 'M' is used in comp

    try
    {
        // if `a` is enabled, M is being used. if not, A is being used
        bits += (instr->a_enabled) ? using_m_map.at(instr->computation)
                                   : using_a_map.at(instr->computation);
    }
    catch (std::out_of_range &ex)
    {
        throw std::runtime_error("Computation not found!");
    }

    bits += dest_map.at(instr->dest); // every possible dest the parser can give is covered
    bits += jump_map.at(instr->jmp);  // see above

    return std::bitset<16>(bits);
}

} // namespace hack