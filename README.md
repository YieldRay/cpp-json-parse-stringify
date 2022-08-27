# C++ 实现 JSON 编码与解码

递归实现，仅作练习。  
尚未完全实现 [JSON 规范](https://www.json.org/json-zh.html)。（暂不支持 Unicode）

支持以下数据类型的映射

```cpp
namespace JSON {
    using Number = double;
    using Boolean = bool;
    using Null = std::nullptr_t;
    using String = std::string;
    using Array = std::vector<Value>;
    using Object = std::map<std::string, Value>;

    enum Type { number, boolean, null, string, array, object };

    class Value {
        public:
        explicit Value(){};
        explicit Value(Number &&_number);
        explicit Value(Boolean &&_boolean);
        explicit Value(Null &&_null);
        explicit Value(const char *_string);
        explicit Value(String &&_string);
        explicit Value(Array &&_array);
        explicit Value(Object &&_object);
        // getters
        inline Number getNumber() const;
        inline Boolean getBoolean() const;
        inline Null getNull() const;
        inline String getString() const;
        inline Array getArray() const;
        inline Object getObject() const;
        // setters
        inline void setNumber(Number &&_number) noexcept;
        inline void setBoolean(Boolean &&_boolean) noexcept;
        inline void setNull(Null &&_null) noexcept;
        inline void setString(const char *_string) noexcept;
        inline void setString(String &&_string) noexcept;
        inline void setArray(Array &&_array) noexcept;
        inline void setObject(Object &&_object) noexcept;
        // type
        inline Type getType() const noexcept;
        inline std::string getTypeString() const noexcept;
        // clear
        void clear() noexcept;
    }

}
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

// 获取 JSON::Value 对象中的数据类型
cout << value.getTypeString();
if(value.getType() == JSON::Type::number) {/**/}
// 获取 JSON::Value 对象中的数据，如果预期类型与实际类型不符，将抛出 JSON::TypeError 异常
cout << value.getNumber();
```

主要的两个函数

```cpp
JSON::Value JSON::parse(std::string str);
std::string JSON::stringify(JSON::Value value, unsigned int indent = 0);

// 从字符串中解析 JSON::Value 对象，如果解析失败，将抛出 JSON::SyntaxError 异常
JSON::Value value = JSON::parse("[{\"abc\":123}]");
// 格式化为文本
cout << JSON::stringify(value);
// 格式化为文本，4 空格缩进
cout << JSON::stringify(value, 4);
```
