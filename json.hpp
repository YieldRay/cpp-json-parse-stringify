#include <string>
#include <cstring>
#include <map>
#include <vector>
#include <exception>
#include <sstream>
#include <cctype>
#include <cstddef>
#include <utility>

namespace JSON
{

    enum Type
    {
        string,
        number,
        boolean,
        null,
        array,
        object,
    };

    class Value
    {
    private:
        using String = std::string;
        using Number = double;
        using Boolean = bool;
        using Null = std::nullptr_t;
        using Array = std::vector<Value>;
        using Object = std::map<std::string, Value>;
        union ValueUnion
        {
            String *string;
            Number *number;
            Boolean *boolean;
            Null *null;
            Array *array;
            Object *object;
        };
        Type type = Type::null;
        ValueUnion data;

    public:
        Value()
        {
            type = Type::null;
            data.null = nullptr;
        };
        explicit Value(const char *_string) { setString(_string); };
        explicit Value(String _string) { setString(std::move(_string)); };
        explicit Value(Number _number) { setNumber(std::move(_number)); };
        explicit Value(Boolean _boolean) { setBoolean(std::move(_boolean)); };
        explicit Value(Null _null) { setNull(std::move(_null)); };
        explicit Value(Array _array) { setArray(std::move(_array)); };
        explicit Value(Object _object) { setObject(std::move(_object)); };

        // getters
        inline String getString() const { return *data.string; }
        inline Number getNumber() const { return *data.number; }
        inline Boolean getBoolean() const { return *data.boolean; }
        inline Null getNull() const { return *data.null; }
        inline Array getArray() const { return *data.array; }
        inline Object getObject() const { return *data.object; }

        // setters
        inline void setString(const char *_string)
        {
            this->data.string = new String(_string);
            this->type = Type::string;
        }
        inline void setString(String &&_string)
        {
            this->data.string = new String(_string);
            this->type = Type::string;
        }
        inline void setNumber(Number &&_number)
        {
            this->data.number = new Number(_number);
            this->type = Type::number;
        }
        inline void setBoolean(Boolean &&_boolean)
        {
            this->data.boolean = new Boolean(_boolean);
            this->type = Type::boolean;
        }
        inline void setNull(Null &&_null)
        {
            this->data.null = new Null(_null);
            this->type = Type::null;
        }
        inline void setArray(Array &&_array)
        {
            this->data.array = new Array(_array);
            this->type = Type::array;
        }
        inline void setObject(Object &&_object)
        {
            this->data.object = new Object(_object);
            this->type = Type::object;
        }

        // type
        inline Type getType() const { return this->type; }
        inline std::string getTypeString()
        {
            switch (this->type)
            {
            case Type::string:
                return "string";
            case Type::number:
                return "number";
            case Type::boolean:
                return "boolean";
            case Type::null:
                return "null";
            case Type::array:
                return "array";
            case Type::object:
                return "object";
            default:
                return "null";
            }
        }

        // free memory
        void free()
        {
            switch (type)
            {
            case Type::string:
                delete (data.string);
            case Type::number:
                delete (data.number);
            case Type::boolean:
                delete (data.boolean);
            case Type::null:
                delete (data.null);
            case Type::array:
                delete (data.array);
            case Type::object:
                delete (data.object);

                setNull(nullptr);
            }
        };
    };

    using Number = double;
    using String = std::string;
    using Object = std::map<std::string, Value>;
    using Array = std::vector<Value>;
    using Null = std::nullptr_t;
    using Boolean = bool;

    // ! JSON::parse
    Value parse(const std::string &str);
    void skipBlank(const std::string &str, size_t &i);
    Value parseValue(const std::string &str, size_t &i);
    String parseString(const std::string &str, size_t &i);
    Null parseNull(const std::string &str, size_t &i);
    Number parseNumber(const std::string &str, size_t &i);
    Boolean parseTrue(const std::string &str, size_t &i);
    Boolean parseFalse(const std::string &str, size_t &i);
    Object parseObject(const std::string &str, size_t &i);
    Array parseArray(const std::string &str, size_t &i);
    // ! JSON::stringify
    String stringify(const Value &value, unsigned int indent = 0);
    String replace_all_distinct(const String &str, const String &old_value, const String &new_value)
    {
        auto cloned = std::string(str);
        for (String::size_type pos(0); pos != String::npos; pos += new_value.length())
            if ((pos = cloned.find(old_value, pos)) != String::npos)
                cloned.replace(pos, old_value.length(), new_value);
            else
                break;
        return cloned;
    }
    String stringifyArray(const Array &value, unsigned int indent = 0);
    String stringifyObject(const Object &value, unsigned int indent = 0);
    inline String stringifyBoolean(const Boolean &value) { return value ? "true" : "false"; }
    inline String stringifyNull(const Null &value) { return "null"; }
    inline String stringifyNumber(const Number &value) { return std::to_string(value); }
    inline String stringifyString(const String &value) { return "\"" + replace_all_distinct(value, "\"", "\\\"") + "\""; }

    // utils
    std::string joinToString() { return ""; }
    std::string joinToString(std::string val) { return val; }
    template <typename T, typename... Ts>
    std::string joinToString(T arg1, Ts... arg_left)
    {
        std::stringstream ss;
        ss << arg1 << joinToString(arg_left...);
        return ss.str();
    }
    class SyntaxError : std::exception
    {
        std::string _reason;

    public:
        SyntaxError(std::string reason) : _reason(reason) {}
        const char *what() const throw()
        {
            return _reason.c_str();
        }
    };
}
