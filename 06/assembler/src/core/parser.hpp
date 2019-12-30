#pragma once

#include "instructions.hpp"
#include <filesystem>
#include <memory>
#include <vector>

namespace hack
{

class Parser
{
public:
    /**
     * Gets and parses a file, returns a list of instruction objects
     */
    std::vector<std::shared_ptr<Instruction>> parse(const std::filesystem::path &);

    /**
     * Attempts to get a file by a string, throws an error if it's unable to
     */
    std::filesystem::path get_file(const std::string &) const;

private:
    /** The current line the parser is on */
    size_t m_line;

    /**
     * The determined type of line
     */
    enum class LineType
    {
        A_INSTR,
        C_INSTR,
        LABEL,
        COMMENT,
        EMPTY_LINE,
        UNKNOWN
    };

    /**
     * Attempts to read a file into an std::string, and returns it
     */
    [[nodiscard]] std::string read_file(const std::filesystem::path &) const;

    /**
     * Gets the type of a line of assembly
     */
    [[nodiscard]] LineType line_type(const std::string &) const noexcept;

    /**
     * Creates an A instruction from a line
     */
    [[nodiscard]] std::shared_ptr<AInstruction> create_a_instr(const std::string &) const;

    /**
     * Creates a C instruction from a line
     */
    [[nodiscard]] std::shared_ptr<CInstruction> create_c_instr(const std::string &) const;

    /**
     * Creates a Label from a line
     */
    [[nodiscard]] std::shared_ptr<Label> create_label(const std::string &) const;
};

} // namespace hack