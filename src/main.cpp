#include <iostream>

#include "symbol.hpp"
#include "object_list.hpp"
#include "parser.hpp"
#include "scanner.hpp"

using namespace nscheme;

int main()
{
    SymbolTable table;
    std::istreambuf_iterator<char> first(std::cin);
    std::istreambuf_iterator<char> last;
    ObjectList object_list;
    std::string filename("stdin");
    try {
        Scanner scanner(first, last, filename, &table);
        Parser parser(&scanner, &table, &object_list);
        ObjectRef obj = parser.Parse();
        std::cout << ObjectToString(obj) << std::endl;
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }
}
