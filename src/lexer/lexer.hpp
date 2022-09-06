// Lexer, takes in the input words and create tokens from them
#pragma once

#include "token.hpp"
#include "../shared/diagnostic.hpp"
#include <vector>
#include <utility>

class lexer
{
	public:
	lexer(std::vector<std::string> const& source, std::string const& fname, diagnostic_manager& diag)
		: source(source), fname(fname), diagnostics(diag) {}

	// Cuts the input into individual words/locations -> will later become tokens
	std::vector<location> lex_raw();
	// Takes in the file content
	std::vector<location> preprocess(std::vector<location> const& raw_tokens);
	// Very very very simple lexer
	std::vector<token> lex(std::vector<location> const& prep_tokens);

	private:
	std::vector<std::string> const& source;
	std::string const& fname;
	diagnostic_manager& diagnostics;
};

extern unsigned char chirp_ctype_space[0x21];

inline bool chirp_isspace(unsigned char ch)
{
	return ch <= 0x20 and chirp_ctype_space[ch];
}
