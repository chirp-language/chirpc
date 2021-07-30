// This manages to pretty much interface all components with eachothers
#include "cmd.hpp"
#include "color.hpp"
#include "lexer/lexer.hpp"
#include "parser/parser.hpp"
#include "codegen/codegen.hpp"
#include "frontend/frontend.hpp"
#include "ast/ast_dumper.hpp"
#include "seman/analyser.hpp"
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
    std::ifstream f(options.filename);

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

    f.close();

    // Preprocessing & Lexing
    diagnostic_manager diagnostics(std::cerr, options.has_color);
    diagnostics.current_source = &content;

    lexer lex(content, options.filename, diagnostics);
    auto raw = lex.lex_raw();
    auto proccesed = lex.preprocess(raw);
    auto tkns = lex.lex(proccesed);

    if (options.dump_tkns)
    {
        std::cout << "Tokens:\n";

        for (token &t : tkns)
        {
            std::cout << t.util_dump() << '\n';
        }
    }
    // Parsing
    parser p(diagnostics);
    p.load_tokens(options.filename, std::move(tkns));
    diagnostics.loc_prov = &p;
    p.parse_top_level();

    // Semantic analysis
    analyser seman(p.get_ast(), diagnostics);
    seman.analyse();

    if (options.dump_ast)
    {
        if (options.dump_tkns)
        {
            std::cout << "--------------------" << '\n';
        }

        text_ast_dumper dumper(options.has_color, options.show_expr_types, &p);

        dumper.dump_ast(p.get_ast());
        std::cout << '\n';
    }

    if (diagnostics.error)
    {
        std::cerr << "Compilation aborted\n";
        return -1;
    }

    frontend frontend;

    if (!frontend.find_compiler())
    {
        if (options.has_color)
        {
            std::cerr << apply_color("[TOOL MISSING] ", color::red);
        }
        else
        {
            std::cerr << "[TOOL MISSING] ";
        }

        std::cerr << "Couldn't find supported C compiler on this machine.\n";
        std::cerr << "Supported compilers are clang and gcc\n";
        std::cerr << "To specify C compiler use option -compiler-path, and then the path to the compiler.\n";

        return -1;
    }

    // Code Generation
    codegen generator(diagnostics);

    auto t = std::make_unique<tracker>(diagnostics);

    generator.set_tree(&p.get_ast(), options.filename);
    generator.gen();

    if (generator.errored)
    {
        std::cerr << "Compilation aborted, because of error in codegen\n";
        return -1;
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
