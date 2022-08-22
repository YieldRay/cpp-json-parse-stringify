#include <string>
#include <cstring>
#include <map>
#include <vector>
#include <exception>
#include <sstream>
#include <cctype>
#include <cstddef>

#define Number double
#define String std::string
#define Array std::vector<Value>
#define Object std::map<std::string, Value> // key of number is not allowed! only string is valid here
#define Null std::nullptr_t
#define Boolean bool

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
        Type type = Type::null;

    public:
        String *string;
        Number *number;
        Boolean *boolean;
        Null *null;
        Array *array;
        Object *object;

        // getters
        inline String getString() { return *this->string; }
        inline Number getNumber() { return *this->number; }
        inline Boolean getBoolean() { return *this->boolean; }
        inline Null getNull() { return *this->null; }
        inline Array getArray() { return *this->array; }
        inline Object getObject() { return *this->object; }

        // setters
        inline void setString(String *_string)
        {
            this->string = _string;
            this->type = Type::string;
        }
        inline void setNumber(Number *_number)
        {
            this->number = _number;
            this->type = Type::number;
        }
        inline void setBoolean(Boolean *_boolean)
        {
            this->boolean = _boolean;
            this->type = Type::boolean;
        }
        inline void setNull(Null *_null)
        {
            this->null = _null;
            this->type = Type::null;
        }
        inline void setArray(Array *_array)
        {
            this->array = _array;
            this->type = Type::array;
        }
        inline void setObject(Object *_object)
        {
            this->object = _object;
            this->type = Type::object;
        }

        // type
        inline Type getType() { return this->type; }
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
    };

    // using Number = double;
    // using String = std::string;
    // using Object = std::map<std::string, Value>;
    // using Array = std::vector<Value>;
    // using Null = void *;
    // using Boolean = bool;

    // ! JSON::parse
    Value parse(std::string str);
    void skipBlank(std::string &str, size_t &i);
    Value parseValue(std::string &str, size_t &i);
    String *parseString(std::string &str, size_t &i);
    Null *parseNull(std::string &str, size_t &i);
    Number *parseNumber(std::string &str, size_t &i);
    Boolean *parseTrue(std::string &str, size_t &i);
    Boolean *parseFalse(std::string &str, size_t &i);
    Object *parseObject(std::string &str, size_t &i);
    Array *parseArray(std::string &str, size_t &i);
    // ! JSON::stringify
    String stringify(Value value, unsigned int indent = 0);
    String &replace_all_distinct(String &str, const String &old_value, const String &new_value)
    {
        for (String::size_type pos(0); pos != String::npos; pos += new_value.length())
            if ((pos = str.find(old_value, pos)) != String::npos)
                str.replace(pos, old_value.length(), new_value);
            else
                break;
        return str;
    }
    String stringifyArray(Array value, unsigned int indent = 0);
    String stringifyObject(Object value, unsigned int indent = 0);
    inline String stringifyBoolean(Boolean value) { return value ? "true" : "false"; }
    inline String stringifyNull(Null value) { return "null"; }
    inline String stringifyNumber(Number value) { return std::to_string(value); }
    inline String stringifyString(String value) { return "\"" + replace_all_distinct(value, "\"", "\\\"") + "\""; }

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
