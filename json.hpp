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
    // Error class
    class TypeError : public std::runtime_error
    {
        using std::runtime_error::runtime_error;
    };

    class SyntaxError : public std::runtime_error
    {
    public:
        using _Mybase = runtime_error;
        explicit SyntaxError(const std::string &_Message) : _Mybase(_Message.c_str()) {}
        explicit SyntaxError(const char *_Message) : _Mybase(_Message) {}
    };

    enum Type
    {
        number,
        boolean,
        null,
        string,
        array,
        object,
    };

    class Value
    {
    private:
        using Number = double;
        using Boolean = bool;
        using Null = std::nullptr_t;
        using String = std::string;
        using Array = std::vector<Value>;
        using Object = std::map<std::string, Value>;
        Type type = Type::null;
        // init data
        Number data_number = 0;
        Boolean data_boolean = false;
        Null data_null = nullptr;
        String data_string;
        Array data_array;
        Object data_object;

    public:
        explicit Value() noexcept {};
        explicit Value(Number &&_number) noexcept { setNumber(std::move(_number)); };
        explicit Value(Boolean &&_boolean) noexcept { setBoolean(std::move(_boolean)); };
        explicit Value(Null &&_null) noexcept { setNull(std::move(_null)); };
        explicit Value(const char *_string) noexcept { setString(_string); };
        explicit Value(String &&_string) noexcept { setString(std::move(_string)); };
        explicit Value(Array &&_array) noexcept { setArray(std::move(_array)); };
        explicit Value(Object &&_object) noexcept { setObject(std::move(_object)); };

        // getters
        inline Number getNumber() const
        {
            if (type != Type::number)
                throw TypeError("Expect number, got " + getType());
            return data_number;
        }
        inline Boolean getBoolean() const
        {
            if (type != Type::boolean)
                throw TypeError("Expect boolean, got " + getType());
            return data_boolean;
        }
        inline Null getNull() const
        {
            if (type != Type::null)
                throw TypeError("Expect null, got " + getType());
            return data_null;
        }
        inline String getString() const
        {
            if (type != Type::string)
                throw TypeError("Expect string, got " + getType());
            return data_string;
        }
        inline Array getArray() const
        {
            if (type != Type::array)
                throw TypeError("Expect array, got " + getType());
            return data_array;
        }
        inline Object getObject() const
        {
            if (type != Type::object)
                throw TypeError("Expect object, got " + getType());
            return data_object;
        }

        // setters
        inline void setNumber(Number &&_number) noexcept
        {
            data_number = _number;
            type = Type::number;
        }
        inline void setBoolean(Boolean &&_boolean) noexcept
        {
            data_boolean = _boolean;
            type = Type::boolean;
        }
        inline void setNull(Null &&_null) noexcept
        {
            data_null = _null;
            type = Type::null;
        }
        inline void setString(const char *_string) noexcept
        {
            data_string = String(_string);
            type = Type::string;
        }
        inline void setString(String &&_string) noexcept
        {
            data_string = _string;
            type = Type::string;
        }
        inline void setArray(Array &&_array) noexcept
        {
            data_array = _array;
            type = Type::array;
        }
        inline void setObject(Object &&_object) noexcept
        {
            data_object = _object;
            type = Type::object;
        }

        // type
        inline Type getType() const noexcept { return type; }
        inline std::string getTypeString() const
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
                throw TypeError("The JSON library does not run normally");
            }
        }
        // free memory
        void clear() noexcept
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
            setNull(nullptr);
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
    String replaceAllDistinct(const String &str, const String &old_value, const String &new_value);
    String stringifyArray(const Array &value, unsigned int indent = 0);
    String stringifyObject(const Object &value, unsigned int indent = 0);
    inline String stringifyBoolean(const Boolean &value) { return value ? "true" : "false"; }
    inline String stringifyNull(const Null &value) { return "null"; }
    inline String stringifyNumber(const Number &value) { return std::to_string(value); }
    inline String stringifyString(const String &value) { return "\"" + replaceAllDistinct(value, "\"", "\\\"") + "\""; }
    // Utils
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
    inline std::string joinToString() { return ""; }
    inline std::string joinToString(const std::string &val) { return val; }
    template <typename T, typename... Ts>
    inline std::string joinToString(const T &arg1, const Ts &...arg_left)
    {
        std::stringstream ss;
        ss << arg1 << joinToString(arg_left...);
        return ss.str();
    }
    inline bool isHexChar(char ch)
    {
        return (ch >= '0' && ch <= '9') ||
               (ch >= 'a' && ch <= 'f') ||
               (ch >= 'A' && ch <= 'F');
    }

    // code from: https://stackoverflow.com/questions/12015571
    int utf8_to_unicode(std::string utf8_code)
    {
        unsigned utf8_size = utf8_code.length();
        int unicode = 0;

        for (unsigned p = 0; p < utf8_size; ++p)
        {
            int bit_count = (p ? 6 : 8 - utf8_size - (utf8_size == 1 ? 0 : 1)),
                shift = (p < utf8_size - 1 ? (6 * (utf8_size - p - 1)) : 0);

            for (int k = 0; k < bit_count; ++k)
                unicode += ((utf8_code[p] & (1 << k)) << shift);
        }

        return unicode;
    }
    std::string unicode_to_utf8(int unicode)
    {
        std::string s;
        if (unicode >= 0 && unicode <= 0x7f) // 7F(16) = 127(10)
        {
            s = static_cast<char>(unicode);
            return s;
        }
        else if (unicode <= 0x7ff) // 7FF(16) = 2047(10)
        {
            unsigned char c1 = 192, c2 = 128;
            for (int k = 0; k < 11; ++k)
            {
                if (k < 6)
                    c2 |= (unicode % 64) & (1 << k);
                else
                    c1 |= (unicode >> 6) & (1 << (k - 6));
            }
            s = c1;
            s += c2;
            return s;
        }
        else if (unicode <= 0xffff) // FFFF(16) = 65535(10)
        {
            unsigned char c1 = 224, c2 = 128, c3 = 128;
            for (int k = 0; k < 16; ++k)
            {
                if (k < 6)
                    c3 |= (unicode % 64) & (1 << k);
                else if (k < 12)
                    c2 |= (unicode >> 6) & (1 << (k - 6));
                else
                    c1 |= (unicode >> 12) & (1 << (k - 12));
            }
            s = c1;
            s += c2;
            s += c3;
            return s;
        }
        else if (unicode <= 0x1fffff) // 1FFFFF(16) = 2097151(10)
        {
            unsigned char c1 = 240, c2 = 128, c3 = 128, c4 = 128;
            for (int k = 0; k < 21; ++k)
            {
                if (k < 6)
                    c4 |= (unicode % 64) & (1 << k);
                else if (k < 12)
                    c3 |= (unicode >> 6) & (1 << (k - 6));
                else if (k < 18)
                    c2 |= (unicode >> 12) & (1 << (k - 12));
                else
                    c1 |= (unicode >> 18) & (1 << (k - 18));
            }
            s = c1;
            s += c2;
            s += c3;
            s += c4;
            return s;
        }
        else if (unicode <= 0x3ffffff) // 3FFFFFF(16) = 67108863(10)
        {
            ; // actually, there are no 5-bytes unicodes
        }
        else if (unicode <= 0x7fffffff) // 7FFFFFFF(16) = 2147483647(10)
        {
            ; // actually, there are no 6-bytes unicodes
        }
        else
            ; // incorrect unicode (< 0 or > 2147483647)
        return "";
    }
}
