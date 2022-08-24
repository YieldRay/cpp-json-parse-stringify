#include "json.cpp"
#include <iostream>
using namespace std;
int main()
{
    try
    {
        JSON::Value arr = JSON::parse("[{\"abc\":123}]");
        arr.getArray().push_back(JSON::Value("hallow!"));   // this has no effect to the real vector, it just push data to a copy
        cout << JSON::stringify(arr.getArray()[1]) << endl; // cannot get "hallow!" but get null
        auto copied_arr = arr.getArray();
        copied_arr.push_back(JSON::Value("OK!"));
        arr.setArray(std::move(copied_arr)); // keep it in mind that you should move it!!!
        cout << arr.getTypeString() << endl; // array
        cout << JSON::stringify(arr) << endl; // [{"abc":123.000000},"OK!"]


        JSON::Value num = JSON::Value(123.4);
        cout << num.getTypeString() << endl; // number
        cout << JSON::stringify(num) << endl;

        auto obj = JSON::parse("{\"a\":1,\"b\":{\"c\":3}}");
        cout << obj.getTypeString() << endl;
        cout << obj.getObject()["a"].getNumber() << endl;
        obj.getObject().insert(std::make_pair(JSON::String("123"), JSON::Value(123.0))); // constructors are explicit, so use double rather than int
        cout << JSON::stringify(obj, 4) << endl;

        cout << JSON::parse("'").getTypeString() << endl; // throw error
    }
    catch (JSON::SyntaxError e)
    {
        cout << "Error: ";
        cout << e.what();
    }
    return 0;
}