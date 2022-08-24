#include "json.cpp"
#include <iostream>
using namespace std;
int main()
{
    try
    {
        JSON::Value arr = JSON::parse("[\"wow\", {\"abc\":123}]");
        arr.getArray().push_back(JSON::Value(789.0));       // this has no effect to the real vector, it just push data to a copy
        cout << (arr.getArray()[0].getString()) << endl;    // wow
        cout << JSON::stringify(arr.getArray()[0]) << endl; // "wow"
        cout << JSON::stringify(arr) << endl;               // ["wow",{"abc":123.000000}]
        cout << arr.getTypeString() << endl;                // array

        JSON::Value num = JSON::Value(123.4);
        cout << num.getTypeString() << endl;  // number
        cout << JSON::stringify(num) << endl; // 123.400000

        auto obj = JSON::parse("{\"a\":1,\"b\":{\"c\":3}}");
        cout << obj.getTypeString() << endl;              // object
        cout << obj.getObject()["a"].getNumber() << endl; // 1
        // obj.getObject().insert(std::make_pair(JSON::String("123"), JSON::Value(123.0))); // constructors are explicit, so use double rather than int
        cout << JSON::stringify(obj.getObject()["b"], 4) << endl;
        cout << JSON::stringify(obj, 4) << endl;

        auto map = JSON::Value(
            {{"hello", JSON::Value("world")},
             {"key", JSON::Value("value")},
             {"abc", JSON::Value(12.3)}});
        cout << JSON::stringify(map, 4) << endl;

        auto vec = JSON::Value({JSON::Value(true), JSON::Value("abc"), JSON::Value(JSON::Array{JSON::Value(12.3)})});
        cout << JSON::stringify(vec, 4) << endl;

        cout << JSON::parse("'").getTypeString() << endl; // throw error
    }
    catch (JSON::SyntaxError e)
    {
        cout << "Error: ";
        cout << e.what();
    }
    return 0;
}