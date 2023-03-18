#include "Shell.hpp"


int main(int argc, const char * argv[]) {

    Ash::Shell::SharedShell().run(argc, argv);

    return 0;
}