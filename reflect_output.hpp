#ifndef REFLECT_OUTPUT_HPP
#define REFLECT_OUTPUT_HPP

#include "debug.hpp"
#include "reflect_class.hpp"

#include <llvm/Support/raw_ostream.h>

#include <string>

namespace reflector {

// @class reflect_output
class reflect_output
{
public:
	explicit reflect_output(llvm::raw_fd_ostream& o)
		: m_out(o)
	{
	}

	void dump(const reflected_class::reflected_collection& reflected)
	{
		dump_comments();
		dump_include_guards_begin();
		dump_includes();
		dump_details();
		dump_reflect_manager(reflected);
		dump_reflect_class(reflected);
		dump_include_guards_end();
	}
private:
	void dump_comments()
	{
		m_out << "/*\n";
		m_out << " ********************************************************\n";
		m_out << " * This code is licensed see LICENSE.txt for details.\n *\n";
		m_out << " * This file was generated by 'greflect' tool.\n";
		m_out << " * Send comments to antonyan_v@outlook.com.\n";
		m_out << " ********************************************************\n";
		m_out << "*/\n\n";
	}

	void dump_include_guards_begin()
	{
		m_out << "#ifndef REFLECT_HPP\n";
		m_out << "#define REFLECT_HPP\n\n";
	}

	void dump_include_guards_end()
	{
		m_out << "#endif // REFLECT_HPP\n";
	}

	void dump_includes()
	{
		m_out << "#include <map>\n";
		m_out << "#include <set>\n";
		m_out << "#include <string>\n";
		m_out << "#include <tuple>\n";
		m_out << "#include <typeinfo>\n";
		m_out << "#include <utility>\n";
		m_out << "\n";
	}

	void dump_details()
	{
		m_out << "namespace detail {\n\n";
		m_out << "template <size_t N>\n";
		m_out << "struct lookup_impl\n{\n";
		m_out << "\ttemplate <typename T>\n";
		m_out << "\tstatic auto lookup(T&& t, size_t n)\n\t{\n";
		m_out << "\t\tif (N - 1 == n) {\n";
		m_out << "\t\t\treturn std::get<N - 1>(std::forward<T>(t));\n\t\t}\n";
		m_out << "\t\tlookup_impl<N - 1>::lookup(std::forward<T>(t), n);\n\t}\n};\n\n";
		m_out << "template <>\nstruct lookup_impl<0>\n{\n";
		m_out << "\ttemplate <typename T>\n";
		m_out << "\tstatic void lookup(T, size_t)\n\t{\n";
		m_out << "\t\t//assert(\"Tuple index out of range\" && false);\n\t}\n};\n\n";
		m_out << "template <typename... Ts>\n";
		m_out << "auto lookup_function(std::tuple<Ts...>& t, size_t idx)\n{\n";
		m_out << "\treturn lookup_impl<sizeof...(Ts)>::lookup(std::move(t), idx);\n}\n\n";
		m_out << "} // namespace detail\n\n";
	}

	void dump_reflect_class_as_template()
	{
		m_out << "// @class reflect\n";
		m_out << "template <typename T>\n";
		m_out << "class reflect\n{\nprivate:\n";
		m_out << "\treflect();\n\n";
		m_out << "\t~reflect();\n\n";
		m_out << "\treflect(const reflect&);\n\n";
		m_out << "\treflect& operator =(const reflect&);\n\n\n";
		m_out << "}; // template class reflect \n\n";
	}

	void dump_reflect_class(const reflected_class::reflected_collection& reflected)
	{
		dump_reflect_class_as_template();
		for (auto i : reflected) {
			i->dump(m_out);
		}
	}

	void dump_reflect_manager(const reflected_class::reflected_collection& reflected)
	{
		m_out << "// @class reflect_manager\nclass reflect_manager\n{\n";
		m_out << "public:\n\ttemplate <typename T>\n";
		m_out << "\tstatic bool is_reflected(const T& o)\n\t{\n";
		m_out << "\t\tstatic std::set<std::string> reflected_classes;\n";
		m_out << "\t\tif (reflected_classes.empty()) {\n";
		for (auto i : reflected) {
			m_out << "\t\t\treflected_classes.insert(\"class " << i->get_name() << "\")\n";
		}
		m_out << "\t\t}\n\t\treturn reflected_classes.find(typeid(o).name()) != reflected_classes.end();\n\t}\n\n";
		m_out << "}; // class reflect_manager\n\n";
	}

private:
	llvm::raw_fd_ostream& m_out;
}; // class reflect_output

} // namespace reflector

#endif //REFLECT_OUTPUT_HPP