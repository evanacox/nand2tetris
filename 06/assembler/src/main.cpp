#include "driver.hpp"
#include <filesystem>

namespace fs = std::filesystem;

int main(int argc, char **argv)
{
    hack::Driver driver;
    driver.start(argc, argv);

    return 0;
}
