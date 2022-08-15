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
    catch (SemanticError& e) {
        std::cerr << "Evaluation error: " << e.what();
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

    while (!std::cin.eof() && global_status_flag == 0) {

        std::cout << "plotscript> ";
        std::string line = readline();

        if (line.empty()) continue;

        if (line == "quit")
            exit(EXIT_SUCCESS);

        if (!interp.interpret(line)) {
            std::cerr << ("Invalid Expression. Could not parse.") << std::endl;
        }
        else {
            try {
                std::cout << interp.evaluate();
            }
            catch (SemanticError& e) {
                 std::cerr << "REPL Error: " << e.what() << "\n";
            }
            std::cout << std::endl;
        }
    }
}

int main(int argc, char* argv[]) {
    Interpreter start;

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
    else if (argc > 3) {
        std::cerr << "Enter a filename to evaluate, or -e <expression>, or use no args for a repl.\n";
        return EXIT_FAILURE;
    }
    else {
        repl(start);
    }
}



