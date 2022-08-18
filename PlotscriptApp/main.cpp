#include "interpreter.h"
#include "interrupt_handler.h"


int eval_from_stream(std::istream& stream, Interpreter& interp) {

    if (!interp.parseStream(stream)) {
        std::cerr << "Invalid Program. Could not parse.\n";
        return EXIT_FAILURE;
    }
    try {
        std::cout << interp.evaluate() << "\n";
    }
    catch (SemanticError& e) {
        std::cerr << e.what();
    }

    return EXIT_SUCCESS;
}

int eval_from_file(const std::string& filename, Interpreter& interp) {

    std::ifstream ifs(filename);

    if (!ifs) {
        std::cerr << "Could not open file for reading.\n";
        return EXIT_FAILURE;
    }

    return eval_from_stream(ifs, interp);
}

int eval_from_command(const std::string& arg_exp, Interpreter& interp) {
    std::istringstream expression(arg_exp);
    return eval_from_stream(expression, interp);
}

void repl(Interpreter& interp) {
    std::string line;

    while (!std::cin.eof() && global_status_flag == 0) {

        std::cout << "plotscript> ";
        std::cout.flush();

        std::getline(std::cin, line);
        if (std::cin.fail() || std::cin.eof()) {
            std::cin.clear();
            line.clear();
        }
        if (line.empty()) continue;


        if (line == "quit")
            exit(EXIT_SUCCESS);

        if (!interp.interpret(line)) {
            std::cout << "Invalid Expression. Could not parse.";
        }
        else {
            try {
                std::cout << interp.evaluate();
            }
            catch (SemanticError& e) {
                 std::cout << e.what();
            }
        }
        std::cout << std::endl;
    }
}

int main(int argc, char* argv[]) {
    Interpreter start;

    if (argc == 1) {
        repl(start);
    }
    if (argc == 2) {
        return eval_from_command(argv[1], start);
    }
    else if (argc == 3) {
        if (std::string(argv[1]) == "-e") {
            return eval_from_command(argv[2], start);
        }
        else if (std::string(argv[1]) == "-f") {
            return eval_from_file(argv[2], start);
        }
    }
    else {
        std::cerr << "Enter a filename to evaluate, or -e <expression>, or use no args for a repl.\n";
        return EXIT_FAILURE;
    }
}



