#include <string>
#include <cstring>
#include <map>
#include <vector>
#include <exception>
#include <sstream>
#include <cctype>
#include <cstddef>
#include <utility>
#include <stdexcept>

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
        Type type = Type::null;

        String data_string;
        Array data_array;
        Object data_object;
        Number data_number = 0;
        Boolean data_boolean = false;
        Null data_null = nullptr;

    public:
        explicit Value(){};
        explicit Value(const char *_string) { setString(_string); };
        explicit Value(String &&_string) { setString(std::move(_string)); };
        explicit Value(Number &&_number) { setNumber(std::move(_number)); };
        explicit Value(Boolean &&_boolean) { setBoolean(std::move(_boolean)); };
        explicit Value(Null &&_null) { setNull(std::move(_null)); };
        explicit Value(Array &&_array) { setArray(std::move(_array)); };
        explicit Value(Object &&_object) { setObject(std::move(_object)); };

        // getters
        inline Number getNumber() const { return data_number; }
        inline Boolean getBoolean() const { return data_boolean; }
        inline Null getNull() const { return data_null; }
        inline String getString() const { return data_string; }
        inline Array getArray() const { return data_array; }
        inline Object getObject() const { return data_object; }

        // setters
        inline void setString(const char *_string)
        {
            data_string = String(_string);
            type = Type::string;
        }
        inline void setString(String &&_string)
        {
            data_string = _string;
            type = Type::string;
        }
        inline void setNumber(Number &&_number)
        {

            data_number = _number;
            type = Type::number;
        }
        inline void setBoolean(Boolean &&_boolean)
        {
            data_boolean = _boolean;
            type = Type::boolean;
        }
        inline void setNull(Null &&_null)
        {
            data_null = _null;
            type = Type::null;
        }
        inline void setArray(Array &&_array)
        {
            data_array = _array;
            type = Type::array;
        }
        inline void setObject(Object &&_object)
        {
            data_object = _object;
            type = Type::object;
        }

        // type
        inline Type getType() const { return type; }
        inline std::string getTypeString()
        {
            switch (type)
            {
            case Type::number:
                return "number";
            case Type::boolean:
                return "boolean";
            case Type::null:
                return "null";
            case Type::string:
                return "string";
            case Type::array:
                return "array";
            case Type::object:
                return "object";
            default:
                throw std::runtime_error("The JSON library does not run normally");
            }
        }
        // free memory
        void clear()
        {
            switch (type)
            {
            case Type::string:
                data_string.clear();
                break;
            case Type::array:
                data_array.clear();
                break;
            case Type::object:
                data_object.clear();
                break;
            }
        }
    };

    // Type bindings
    using Number = double;
    using Boolean = bool;
    using Null = std::nullptr_t;
    using String = std::string;
    using Array = std::vector<Value>;
    using Object = std::map<std::string, Value>;

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
    String replaceAllDistinct(const String &str, const String &old_value, const String &new_value)
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
    inline String stringifyString(const String &value) { return "\"" + replaceAllDistinct(value, "\"", "\\\"") + "\""; }

    // Utils
    std::string joinToString() { return ""; }
    std::string joinToString(const std::string &val) { return val; }
    template <typename T, typename... Ts>
    std::string joinToString(const T &arg1, const Ts &...arg_left)
    {
        std::stringstream ss;
        ss << arg1 << joinToString(arg_left...);
        return ss.str();
    }
    // Error class
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
