#include "parser.hpp"
#include <algorithm>
#include <fstream>
#include <string>

namespace fs = std::filesystem;

static inline bool is_number(const std::string &s)
{
    std::string::const_iterator it = s.begin();

    while (it != s.end() && std::isdigit(*it))
        ++it;

    return !s.empty() && it == s.end();
}

static std::vector<std::string> split(const std::string s, const std::string delimiter)
{
    std::size_t pos_start = 0, pos_end, delim_len = delimiter.length();
    std::string token;
    std::vector<std::string> res;

    while ((pos_end = s.find(delimiter, pos_start)) != std::string::npos)
    {
        token = s.substr(pos_start, pos_end - pos_start);
        pos_start = pos_end + delim_len;
        res.push_back(token);
    }

    res.push_back(s.substr(pos_start));
    return res;
}

static inline std::vector<std::string> sanitize(const std::string data)
{
    auto lns = split(data, "\n");

    std::for_each(lns.begin(), lns.end(), [](std::string &str) {
        std::string sanitized(str);

        sanitized.erase(std::remove(sanitized.begin(), sanitized.end(), '\r'), sanitized.end());
        sanitized.erase(std::remove(sanitized.begin(), sanitized.end(), '\n'), sanitized.end());
        sanitized.erase(std::remove(sanitized.begin(), sanitized.end(), '\t'), sanitized.end());
        sanitized.erase(std::remove(sanitized.begin(), sanitized.end(), ' '), sanitized.end());

        str = sanitized.substr(0, sanitized.find("//"));
    });

    std::vector<std::string> lines(lns.size());

    // removes all the empty lines
    std::copy_if(lns.begin(), lns.end(), lines.begin(), [](std::string &l) { return !l.empty(); });

    return lines;
}

namespace hack
{

std::vector<std::shared_ptr<Instruction>> Parser::parse(const fs::path &f)
{
    auto data = read_file(f);
    auto lines = sanitize(data);

    std::vector<std::shared_ptr<Instruction>> instructions;

    for (const auto &line : lines)
    {
        ++m_line;

        try
        {
            switch (line_type(line))
            {
                case LineType::COMMENT:
                case LineType::EMPTY_LINE:
                    break;
                case LineType::A_INSTR:
                    instructions.push_back(create_a_instr(line));
                    break;
                case LineType::C_INSTR:
                    instructions.push_back(create_c_instr(line));
                    break;
                case LineType::LABEL:
                    instructions.push_back(create_label(line));
                    break;
                case LineType::UNKNOWN:
                    throw std::runtime_error("Line #" + std::to_string(m_line) + ": unknown type!");
            }
        }
        catch (std::exception &ex)
        {
            throw std::runtime_error(std::string(ex.what()) + " on line #" +
                                     std::to_string(m_line));
        }
    }

    return instructions;
}

std::shared_ptr<AInstruction> Parser::create_a_instr(const std::string &line) const
{
    if (line[1] == '\0')
        throw std::runtime_error("A instruction has no constant!");

    auto constant = line.substr(1);

    AInstruction instr;
    instr.constant = std::string(constant);
    instr.type = InstructionType::A_INSTR;
    instr.raw = std::string(line);
    instr.is_named = !is_number(constant);

    return std::make_shared<AInstruction>(instr);
}

std::shared_ptr<CInstruction> Parser::create_c_instr(const std::string &line) const
{
    CInstruction instr;

    instr.raw = line;
    instr.type = InstructionType::C_INSTR;
    instr.dest = (line.find('=') != std::string::npos)
        ? std::string(line.begin(), line.begin() + (line.find('=')))
        : "nil";

    auto begin_pos = (instr.dest == "nil") ? line.begin() : line.begin() + line.find('=') + 1;

    instr.computation = (line.find(';') != std::string::npos)
        ? std::string(begin_pos, line.begin() + (line.find(';')))
        : std::string(begin_pos, line.end());

    instr.a_enabled = (instr.computation.find('M') != std::string::npos);

    if (instr.computation.empty())
        throw std::runtime_error("Computation isn't specified!");

    instr.jmp = (line.find(';') != std::string::npos)
        ? std::string(line.begin() + line.find(';') + 1, line.end())
        : "nil";

    return std::make_shared<CInstruction>(instr);
}

std::shared_ptr<Label> Parser::create_label(const std::string &line) const
{
    Label instr;

    instr.raw = line;
    instr.type = InstructionType::LABEL;

    if (line[line.size() - 1] != ')')
        throw std::runtime_error("Label is missing a closing paren!");

    instr.label = std::string(line.begin() + 1, line.end() - 1);

    return std::make_shared<Label>(instr);
}

Parser::LineType Parser::line_type(const std::string &line) const noexcept
{
    std::size_t n_of_spaces = 0;

    for (auto c : line)
    {
        if (!std::isspace(c))
        {
            if (c == '@')
                return LineType::A_INSTR;
            if (c == 'D' || c == 'A' || c == 'M')
                return LineType::C_INSTR;
            if (c == '(')
                return LineType::LABEL;
            if (c == '/')
                return LineType::COMMENT;
        }

        ++n_of_spaces;
    }

    if (n_of_spaces == line.size())
        return LineType::EMPTY_LINE;

    return LineType::UNKNOWN;
}

fs::path Parser::get_file(const std::string &name) const
{
    fs::path file(name);

    if (fs::exists(file))
    {
        return file;
    }

    throw std::runtime_error("File \"" + file.string() + "\" cannot be found!");
}

std::string Parser::read_file(const fs::path &f) const
{
    std::ifstream reader(f);

    if (reader.is_open())
    {
        std::stringstream buffer;
        buffer << reader.rdbuf();
        reader.close();
        return buffer.str();
    }

    throw std::runtime_error("Unable to open file \"" + f.string() + "\".");
}

} // namespace hack