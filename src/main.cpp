// This manages to pretty much interface all components with eachothers
#include "cmd.hpp"
#include "color.hpp"
#include "lexer/lexer.hpp"
#include "lexer/preproc.hpp"
#include "parser/parser.hpp"
#include "codegen/codegen.hpp"
#include "frontend/frontend.hpp"
#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <string>

// Returns 0 if everything goes normal
// Returns -1 if bad code
// Returns -2 if error(i.e. can't open file)
int main(int argc, char** argv)
{
    cmd options = parse_cmd(argc, argv);

    if (options.version)
    {
        options.write_version();
        return 0;
    }

    if (options.help)
    {
        options.write_help();
        return 0;
    }

    if (options.error)
    {
        std::cout << "Error in provided arguments\n";
        return -1;
    }

    // Lets do the reading here cuz why the f not
    // Also kinda like very inefficient
    std::fstream f(options.filename);

    if (!f)
    {
        std::cout << "Can't open file: \"" << options.filename << "\"\n";
        return -1;
    }

    //Doesn't need to use HackySTL to be hacky :^)
    std::vector<std::string> content;
    std::string line;

    while (std::getline(f, line))
    {
        content.push_back(line);
    }

    // Preprocessing & Lexing
    auto proccesed = preprocess(options.filename, content);
    auto tkns = lexe(proccesed, content);

    if (options.dump_tkns)
    {
        std::cout << "Tokens:\n";

        for (token &t : tkns)
        {
            std::cout << t.util_dump() << '\n';
        }
    }
    // Parsing
    parser p;
    p.load_tokens(options.filename, tkns);
    p.parse();
    std::vector<helper> phelpers = p.get_helpers();
    bool ok = true;

    for (helper &h : phelpers)
    {
        // Always copying the file content is like
        // really really really bad & inneficient
        std::cout << h.write_helper(content, options) << '\n';

        if (
            h.type == helper_type::global_err ||
            h.type == helper_type::line_err ||
            h.type == helper_type::location_err)
        {
            ok = false;
        }
    }

    if (options.dump_ast)
    {
        if (options.dump_tkns)
        {
            std::cout << "--------------------" << '\n';
        }

        std::cout << p.get_ast().dump() << '\n';
    }

    if (!ok)
    {
        return -1;
    }

    frontend frontend;

    if (!frontend.find_compiler())
    {
        if (options.has_color)
        {
            std::cout << write_color("[TOOL MISSING]", color::red);
        }
        else
        {
            std::cout << "[TOOL MISSING] ";
        }

        std::cout << "Couldn't find supported C compiler on this machine.\n";
        std::cout << "Supported compilers are clang and gcc\n";
        std::cout << "To specify C compiler use option -compiler-path, and then the path to the compiler.\n";

        return -1;
    }

    // Code Generation
    codegen generator;

    auto t = std::make_unique<tracker>();
    t->init();

    generator.set_tree(p.get_ast(), options.filename);
    generator.set_tracker(t.get());
    generator.gen();

    if (generator.errored)
    {
        for (helper& h : generator.helpers)
        {
            std::cout << h.write_helper(content, options) << '\n';
        }
    }

    frontend.make_tmp_folder();

    frontend.write_out("dump", generator.get_result());

    // Tooling
    // (use the compiler)

    // Cleanup
    if (!options.keep_tmp)
    {
        frontend.remove_tmp_folder();
    }

    return 0;
}