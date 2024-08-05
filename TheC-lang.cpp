#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <cctype>
#include <stack>
#include <cmath>
#include <fstream>

enum TokenType {
    NUMBER,
    OPERATOR,
    FUNCTION
};

struct Token {
    TokenType type;
    std::string value;
};

std::vector<Token> tokenize(const std::string& expression) {
    std::vector<Token> tokens;
    std::istringstream stream(expression);
    char ch;
    while (stream >> ch) {
        if (std::isdigit(ch) || ch == '.') {
            stream.putback(ch);
            double number;
            stream >> number;
            tokens.push_back({NUMBER, std::to_string(number)});
        } else if (ch == '+' || ch == '-' || ch == '*' || ch == '/') {
            tokens.push_back({OPERATOR, std::string(1, ch)});
        } else if (std::isalpha(ch)) {
            stream.putback(ch);
            std::string func;
            stream >> func;
            if (func == "log" || func == "sin" || func == "abs" || func == "acos" ||
                func == "asin" || func == "ceil" || func == "cos" || func == "deg" ||
                func == "exp" || func == "floor" || func == "modf" || func == "rad" ||
                func == "sqrt" || func == "tan") {
                tokens.push_back({FUNCTION, func});
            }
        }
    }
    return tokens;
}

double evaluate(const std::vector<Token>& tokens) {
    std::stack<double> values;
    std::stack<char> ops;
    std::stack<Token> funcs;

    auto applyOperator = [](char op, double b, double a) -> double {
        switch (op) {
            case '+': return a + b;
            case '-': return a - b;
            case '*': return a * b;
            case '/': return a / b;
            default: return 0;
        }
    };

    auto applyFunction = [](const std::string& func, double a) -> double {
        if (func == "log") return std::log(a);
        if (func == "sin") return std::sin(a);
        if (func == "abs") return std::abs(a);
        if (func == "acos") return std::acos(a);
        if (func == "asin") return std::asin(a);
        if (func == "ceil") return std::ceil(a);
        if (func == "cos") return std::cos(a);
        if (func == "exp") return std::exp(a);
        if (func == "floor") return std::floor(a);
        if (func == "modf") {
            double intpart;
            return std::modf(a, &intpart);
        }
        if (func == "sqrt") return std::sqrt(a);
        if (func == "tan") return std::tan(a);
        return 0;
    };

    for (const auto& token : tokens) {
        if (token.type == NUMBER) {
            values.push(std::stod(token.value));
        } else if (token.type == OPERATOR) {
            while (!ops.empty() && ((token.value[0] == '+' || token.value[0] == '-') ||
                                    (ops.top() == '*' || ops.top() == '/'))) {
                double val2 = values.top(); values.pop();
                double val1 = values.top(); values.pop();
                char op = ops.top(); ops.pop();
                values.push(applyOperator(op, val2, val1));
            }
            ops.push(token.value[0]);
        } else if (token.type == FUNCTION) {
            funcs.push(token);
        }
    }

    while (!ops.empty()) {
        double val2 = values.top(); values.pop();
        double val1 = values.top(); values.pop();
        char op = ops.top(); ops.pop();
        values.push(applyOperator(op, val2, val1));
    }

    while (!funcs.empty()) {
        double val = values.top(); values.pop();
        Token func = funcs.top(); funcs.pop();
        values.push(applyFunction(func.value, val));
    }

    return values.top();
}

bool hasExtension(const std::string& filename, const std::string& extension) {
    if (filename.size() >= extension.size() &&
        filename.compare(filename.size() - extension.size(), extension.size(), extension) == 0) {
        return true;
    }
    return false;
}

int main() {
    std::string inputFileName, outputFileName;

    std::cout << "Enter the input file name (must end with .math): ";
    std::getline(std::cin, inputFileName);
    if (!hasExtension(inputFileName, ".math")) {
        std::cerr << "Error: Input file must end with .math" << std::endl;
        return 1;
    }

    std::cout << "Enter the output file name (must end with .resu): ";
    std::getline(std::cin, outputFileName);
    if (!hasExtension(outputFileName, ".resu")) {
        std::cerr << "Error: Output file must end with .resu" << std::endl;
        return 1;
    }

    std::ifstream inputFile(inputFileName);
    std::ofstream outputFile(outputFileName);

    if (!inputFile.is_open()) {
        std::cerr << "Error: Could not open input file." << std::endl;
        return 1;
    }
    if (!outputFile.is_open()) {
        std::cerr << "Error: Could not open output file." << std::endl;
        return 1;
    }

    std::string expression;
    while (std::getline(inputFile, expression)) {
        std::vector<Token> tokens = tokenize(expression);
        double result = evaluate(tokens);
        outputFile << result << std::endl;
    }

    inputFile.close();
    outputFile.close();

    std::cout << "Processing complete. Results have been written to " << outputFileName << std::endl;

    return 0;
}