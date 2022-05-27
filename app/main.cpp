#include "interpreter.h"
#include "interrupt_handler.h"

std::string readline() {
    std::string line;
    std::getline(std::cin, line);
    if (std::cin.fail() || std::cin.eof()) {
        std::cin.clear();
        line.clear();
    }
    return line;
}

int eval_from_stream(std::istream& stream, Interpreter& interp) {

    if (!interp.parseStream(stream)) {
        std::cerr << "Invalid Program. Could not parse.\n";
        return EXIT_FAILURE;
    }
    try {
        std::cout << interp.evaluate() << "\n";
    }
    catch (SemanticError e) {
        std::cerr << e.what();
    }

    return EXIT_SUCCESS;
}

int eval_from_file(std::string filename, Interpreter& interp) {

    std::ifstream ifs(filename);

    if (!ifs) {
        std::cerr << "Could not open file for reading.\n";
        return EXIT_FAILURE;
    }

    return eval_from_stream(ifs, interp);
}

int eval_from_command(std::string argexp, Interpreter& interp) {
    std::istringstream expression(argexp);
    return eval_from_stream(expression, interp);;
}

void repl(Interpreter& interp) {

    while (!std::cin.eof() && global_status_flag == 0) {

        std::cout << "prompt> ";
        std::string line = readline();

        if (line.empty()) continue;

        if (line == "quit")
            exit(EXIT_SUCCESS);

        std::istringstream expression(line);

        if (!interp.parseStream(expression)) {
            std::cerr << ("Invalid Expression. Could not parse.") << std::endl;;
        }
        else {
            try {
                std::cout << interp.evaluate();
            }
            catch (SemanticError e) {
                 std::cerr << e.what() << "\n";
            }
            std::cout << std::endl;
        }
    }
}

int main(int argc, char* argv[])
{
    Interpreter start;

    if (argc == 2) {
        return eval_from_file(argv[1], start);
    }
    else if (argc == 3) {
        if (std::string(argv[1]) == "-e") {
            return eval_from_command(argv[2], start);
        }
    }
    else if (argc > 3) {
        std::cerr << "Enter filename or -e \"EXPR\" to evaluate, or no args for repl.\n";
        return EXIT_FAILURE;
    }
    else {
        repl(start);
    }

}



