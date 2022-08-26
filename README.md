# C++ 实现 JSON 编码与解码

递归实现，仅作练习。  
尚未完全实现 [JSON 规范](https://www.json.org/json-zh.html)。

支持以下数据类型的映射 （暂不支持 Unicode）

```cpp
using Number = double;
using String = std::string;
using Object = std::map<std::string, JSON::Value>;
using Array = std::vector<JSON::Value>;
using Null = std::nullptr_t;
using Boolean = bool;
```

构造 JSON::Value 对象 （注意：构造函数需传入右值）

```cpp
JSON::Value value = JSON::Value(123.4);
JSON::Value value = JSON::Value("string");
JSON::Value value = JSON::Value({
    {"hello", JSON::Value("world")},
    {"key", JSON::Value("value")},
    {"abc", JSON::Value(12.3)}
});
JSON::Value value = JSON::Value({JSON::Value(true), JSON::Value("abc"), JSON::Value(JSON::Array{JSON::Value(12.3)})});

// 获取 JSON 值对象中的数据类型
cout << value.getTypeString();
if(value.getType() == JSON::Type::number) {/**/}
// 获取 JSON 值对象中的数据，如果预期类型与实际类型不符，将抛出 JSON::TypeError 异常
cout << value.getNumber();
```

主要的两个函数

```cpp
JSON::Value JSON::parse(std::string str);
std::string JSON::stringify(JSON::Value value, unsigned int indent = 0);

// 通过字符串解析 JSON 值对象，如果解析失败，将抛出 JSON::SyntaxError 异常
JSON::Value value = JSON::parse("[{\"abc\":123}]");
// 格式化为文本，4 空格缩进
cout << JSON::Stringify(value, 4);
```
