#pragma once

#include "core/instructions.hpp"
#include <bitset>
#include <filesystem>
#include <iostream>
#include <memory>
#include <new>
#include <unordered_map>
#include <vector>

namespace hack
{

/**
 * The assembler driver
 */
class Driver
{
public:
    /**
     * Called from main, gets files out of the arguments
     */
    void start(int, char **);

    /**
     * Parses a file
     */
    std::vector<std::shared_ptr<Instruction>> parse(std::string);

    /**
     * Assembles a single file, adding it to m_hack_output
     */
    static std::vector<std::bitset<16>> assemble(std::vector<std::shared_ptr<Instruction>>);

    /**
     * Writes the hack output of a single file
     */
    static void write(const std::filesystem::path &, const std::vector<std::bitset<16>> &);

private:
    /**
     * Contains a list of every file from the arguments
     */
    std::vector<std::filesystem::path> m_files;

    /**
     * Type of logger message
     */
    enum class LoggerCase
    {
        OK,
        INFO,
        ERROR,
        FATAL
    };

    /**
     * Formats a console message
     */
    template <typename T>
    inline std::string _format(T item, LoggerCase lcase) const noexcept
    {
        std::string to_return("  hack-as ");

        switch (lcase)
        {
            case LoggerCase::OK:
                to_return += "\u001b[38;5;40mok";
                break;
            case LoggerCase::INFO:
                to_return += "\u001b[38;5;39minfo";
                break;
            case LoggerCase::ERROR:
                to_return += "\u001b[38;5;196merror";
                break;
            case LoggerCase::FATAL:
                to_return += "\u001b[38;5;124mfatal";
                break;
        }

        to_return += "\u001b[0m: ";
        to_return += item;

        return to_return;
    }

    /**
     * Logs an "ok" message to stdout
     */
    template <typename T>
    void log_ok(T item) const noexcept
    {
        std::cout << _format(item, LoggerCase::OK) << "\n";
    }

    /**
     * Logs an "error" message to stdout
     */
    template <typename T>
    void log_error(T item) const noexcept
    {
        std::cout << _format(item, LoggerCase::ERROR) << "\n";
    }

    /**
     * Logs an "info" message to stdout
     */
    template <typename T>
    void log_info(T item) const noexcept
    {
        std::cout << _format(item, LoggerCase::INFO) << "\n";
    }

    /**
     * Logs a "fatal" message to stdout
     */
    template <typename T>
    void log_fatal(T item) const noexcept
    {
        std::cout << _format(item, LoggerCase::FATAL) << "\n";
    }
};

} // namespace hack