#include "./json.cpp"
#include <iostream>
using namespace std;

int main()
{
    // system("chcp 65001");
    try
    {
        // to echo string below, ensure you are using correct encoding
        cout << JSON::parse(R"JSON( "\\u4f60\\u597d\\uff0c\\u4e16\\u754c\\uff01" )JSON").getString() << endl;
        cout << JSON::parse(R"JSON( "\a" )JSON").getString() << endl; // this is ok, but \n will cause error
        cout << JSON::parse(R"JSON( "break\\nline" )JSON").getString() << endl;

        cout << JSON::parse("-9.9E+9").getNumber() << endl;
        cout << JSON::parse("-9.9E-9").getNumber() << endl;

        JSON::Value arr = JSON::parse(R"JSON(  ["wow", {"abc":123}]  )JSON");
        arr.getArray().push_back(JSON::Value(789.0));       // this has no effect to the real vector, it just push data to a copy
        cout << (arr.getArray()[0].getString()) << endl;    // wow
        cout << JSON::stringify(arr.getArray()[0]) << endl; // "wow"
        cout << JSON::stringify(arr) << endl;               // ["wow",{"abc":123.000000}]
        cout << arr.getTypeString() << endl;                // array

        JSON::Value num = JSON::Value(123.4);
        cout << num.getTypeString() << endl;  // number
        cout << JSON::stringify(num) << endl; // 123.400000

        auto obj = JSON::parse(R"JSON(
            {
                "a": 1,
                "b": { "c": 3 }
             }
        )JSON");
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
        cout << "SyntaxError: ";
        cout << e.what();
    }
    catch (JSON::TypeError e)
    {
        cout << "TypeError: ";
        cout << e.what();
    }
    return 0;
}