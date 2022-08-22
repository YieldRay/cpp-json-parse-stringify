# C++ 实现 JSON 编码与解码

仅作练习。  
尚未完全实现 [JSON 规范](https://www.json.org/json-zh.html)。

支持以下数据类型的映射  
（目前对象仅允许字符串作为键，不支持科学计数法）

```cpp
#define Number double
#define String std::string
#define Array std::vector<Value>
#define Object std::map<std::string, Value>
#define Null std::nullptr_t
#define Boolean bool


class Value
{
public:
  String *string;
  Number *number;
  Boolean *boolean;
  Null *null;
  Array *array;
  Object *object;
}
```

主要的两个函数

```cpp
JSON::Value JSON::parse(std::string str);
std::string JSON::stringify(JSON::Value value, unsigned int indent = 0);
```
