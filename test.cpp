#include "json.cpp"
#include <iostream>

int main()
{
    system("chcp 65001");
    try
    {
        std::cout << JSON::parse("[{\"abc\":123}]").getTypeString() << std::endl;
        std::cout << JSON::stringify(JSON::parse("{\"a\":1,\"b\":{\"c\":3}}"), 4);
        std::cout << JSON::parse("'").getTypeString() << std::endl; // throw error
    }
    catch (JSON::SyntaxError e)
    {
        std::cout << std::endl
                  << "错误：";
        std::cout << e.what();
    }
}