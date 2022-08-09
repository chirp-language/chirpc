#include "sym_dumper.hpp"

constexpr color c_color_decl = color::green | color::bright | color::bold;
constexpr color c_color_identifier = color::blue | color::bright;
constexpr color c_color_address = color::red | color::green | color::bright;

static std::string indent(int x)
{
    std::string result;
    result.resize(x * 3, ' ');
    return result;
}

void text_symbol_dumper::dump_symbols()
{
	dump(*m_tracker.get_top());
	if (dump_syms_extra)
	{
		std::cout << "Anonymous syms\n";
		++depth;
		for (auto& sym : m_tracker.table)
			if (!sym->is_global)
				dump(*sym);
		--depth;
	}
}

void text_symbol_dumper::dump(tracker::symbol& sym)
{
	indent(depth);
	std::cout << "symbol";
	if (sym.has_name)
	{
		if (sym.is_global)
		{
			std::cout << " <";
			begin_color(c_color_identifier);
			int i = 0, len = sym.full_name.parts.size();
			for (auto const& id : sym.full_name.parts)
			{
				std::cout << id.name;
				if (++i != len)
				{
					std::cout << ".";
				}
			}
			end_color();
			std::cout << '>';
		}
		std::cout << ' ';
		begin_color(c_color_identifier);
		std::cout << sym.name.name;
		end_color();
	}

	// Flags
	if (sym.has_name)
		std::cout << " has_name";
	if (sym.is_global)
		std::cout << " global";
	if (sym.has_storage)
		std::cout << " has_storage";
	if (sym.is_entry)
		std::cout << " entry";
	if (sym.is_scope)
		std::cout << " scope";

	if (sym.target)
	{
		std::cout << ' ';
		begin_color(c_color_decl);
		switch (sym.target->kind)
		{
			case decl_kind::root:
				std::cout << "root";
				break;
			case decl_kind::var:
				std::cout << "var";
				break;
			case decl_kind::entry:
				std::cout << "entry";
				break;
			case decl_kind::import:
				std::cout << "import";
				break;
			case decl_kind::nspace:
				std::cout << "namespace";
				break;
			case decl_kind::fdecl:
				std::cout << "func_decl";
				break;
			case decl_kind::fdef:
				std::cout << "func_def";
				break;
			case decl_kind::external:
				std::cout << "extern";
				break;
		}
		end_color();
		std::cout << ' ';
		begin_color(c_color_address);
		std::cout << '[' << sym.target << ']';
		end_color();
	}
	std::cout << '\n';
	// Iterate over children
	if (sym.target)
	{
		auto const It = [this](decl& decl) {
			if (decl.symbol)
				return dump(*decl.symbol);
		};
		decl* d = sym.target;
		++depth;
		switch (d->kind)
		{
			case decl_kind::root:
			{
				for (auto& c : static_cast<ast_root*>(d)->top_decls)
					It(*c);
				break;
			}
			case decl_kind::nspace:
			{
				for (auto& c : static_cast<namespace_decl*>(d)->decls)
					It(*c);
				break;
			}
			case decl_kind::fdecl:
			case decl_kind::fdef:
			{
				for (auto& c : static_cast<func_decl*>(d)->params.body)
					It(*c);
				break;
			}
			case decl_kind::external:
				It(*static_cast<extern_decl*>(d)->inner_decl);
				break;
			case decl_kind::var:
			case decl_kind::entry:
			case decl_kind::import:
				break;
		}
		--depth;
	}
}
