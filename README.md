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
JSON::Value value = JSON::Value(makr);

```
