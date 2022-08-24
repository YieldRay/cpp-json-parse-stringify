# C++ 实现 JSON 编码与解码

仅作练习。  
尚未完全实现 [JSON 规范](https://www.json.org/json-zh.html)。

支持以下数据类型的映射  
（目前对象仅允许字符串作为键，不支持科学计数法）

```cpp
using Number = double;
using String = std::string;
using Object = std::map<std::string, Value>;
using Array = std::vector<Value>;
using Null = std::nullptr_t;
using Boolean = bool;
```

主要的两个函数

```cpp
JSON::Value JSON::parse(std::string str);
std::string JSON::stringify(JSON::Value value, unsigned int indent = 0);
```

构造 JSON 值对象

```cpp
JSON::Value value = JSON::Value(123.4);
JSON::Value value = JSON::Value("string");
JSON::Value value = JSON::Value({
    {"hello", JSON::Value("world")},
    {"key", JSON::Value("value")},
    {"abc", JSON::Value(12.3)}
});
JSON::Value value = JSON::Value({JSON::Value(true), JSON::Value("abc"), JSON::Value(JSON::Array{JSON::Value(12.3)})});
// 通过字符串解析 JSON 值对象
JSON::Value value = JSON::parse("[{\"abc\":123}]");
// 格式化为文本，4 空格缩进
cout << JSON::Stringify(value, 4);
// 获取 JSON 值对象中的数据类型
cout << value.getTypeString();
if(value.getType() == JSON::Type::number) {/**/}
// 获取 JSON 值对象中的数据
cout << value.getNumber();
```
