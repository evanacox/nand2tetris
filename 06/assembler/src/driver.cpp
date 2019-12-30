#include "driver.hpp"
#include "core/assembler.hpp"
#include "core/parser.hpp"
#include <experimental/filesystem>
#include <fstream>
#include <utility>

namespace fs = std::filesystem;

namespace hack
{

void Driver::start(int argc, char **argv)
{
    if (argc <= 1)
    {
        log_fatal("Expected more than 1 argument!");
        exit(100);
    }

    // start at index 1 of args, 0 is the program name
    for (auto i = 1; i < argc; ++i)
    {
        try
        {
            log_info("Loading file: \"" + std::string(argv[i]) + "\"");

            // parse the file
            auto parsed = parse(argv[i]);
            log_info("Assembling file \"" + std::string(argv[i]) + "\"");

            // assemble the file
            auto assembled = assemble(std::move(parsed));

            // write file
            write(m_files[m_files.size() - 1], assembled);

            log_ok("Built file \"" + std::string(argv[i]) + "\"");
        }
        catch (std::exception &ex)
        {
            log_error(ex.what());
            log_fatal("An error occurred. Aborting...");
        }
    }
}

std::vector<std::shared_ptr<Instruction>> Driver::parse(std::string file)
{
    Parser parser{};

    // find the file
    auto path = parser.get_file(std::move(file));

    // add file to the driver's field
    m_files.push_back(path);

    return parser.parse(path);
}

std::vector<std::bitset<16>> Driver::assemble(std::vector<std::shared_ptr<Instruction>> vec)
{
    Assembler assembler;

    // assembles the file
    auto output = assembler.assemble(std::move(vec));

    return output;
}

void Driver::write(const fs::path &file, const std::vector<std::bitset<16>> &bits)
{
    auto original_name = file.stem().string();

    std::ofstream writer(original_name + ".hack");

    for (const auto &bit : bits)
    {
        writer << bit.to_string() << "\n";
    }

    writer.close();
}

} // namespace hack