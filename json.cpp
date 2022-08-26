#include "json.hpp"

namespace JSON
{

    Value parse(const std::string &str)
    {
        size_t i = 0;
        auto rst = parseValue(str, i);
        skipBlank(str, i);
        if (i != str.length())
            throw SyntaxError("Unexpected end of JSON input");
        return rst;
    };

    void skipBlank(const std::string &str, size_t &i)
    {
        while (str[i] == ' ' || str[i] == '\n' || str[i] == '\r' || str[i] == '\t')
            i++;
    }
    Value parseValue(const std::string &str, size_t &i)
    {
        if (i >= str.length())
            throw SyntaxError("Unexpected end of JSON input");
        Value value;
        skipBlank(str, i);
        if (str[i] == '{')
            value.setObject(parseObject(str, i));
        else if (str[i] == '[')
            value.setArray(parseArray(str, i));
        else if (str[i] == 'n')
            value.setNull(parseNull(str, i));
        else if (str[i] == 't')
            value.setBoolean(parseTrue(str, i));
        else if (str[i] == 'f')
            value.setBoolean(parseFalse(str, i));
        else if (str[i] == '"')
            value.setString(parseString(str, i));
        else if (isdigit(str[i]) || str[i] == '-')
            value.setNumber(parseNumber(str, i));
        else
            throw SyntaxError(joinToString("Unexpected token ", str[i], " in JSON at position ", i));
        return value;
    }
    String parseString(const std::string &str, size_t &i)
    {
        String result;
        i++; // skip left colon
        while (str[i] != '"')
        {
            if (str[i] == '\n')
                throw SyntaxError("Bad control character in string literal in JSON at position " + i);
            if (str[i] == '\\')
            {
                i++; // skip '\'
                switch (str[i])
                {
                case '"':
                    result += '"';
                    break;
                case '\\':
                    result += "\\";
                    break;
                case '/':
                    result += "/";
                    break;
                case 'b':
                    result += "\b";
                    break;
                case 'f':
                    result += "\f";
                    break;
                case 'n':
                    result += "\n";
                    break;
                case 'r':
                    result += "\r";
                    break;
                case 't':
                    result += "\t";
                    break;
                case 'u':
                    // TODO
                    throw SyntaxError("Unicode is unimplemented yet");

                default:
                    throw SyntaxError(joinToString("Unexpected token ", str[i], " in JSON at position ", i));
                }
                i++; // skip common '\'
            }
            else
            {
                result += str[i];
                i++;
            }
        }
        i++; // skip right colon
        return result;
    }

    Number parseNumber(const std::string &str, size_t &i)
    {
        std::string numStr = "";
        if (str[i] == '-')
        {
            numStr += '-';
            i++;
        }
        while (isdigit(str[i]))
        {
            numStr += str[i];
            i++;
        }
        if (str[i] == '.') // push '.' and push rest number
        {
            numStr += '.';
            i++;
            if (i >= str.length())
                throw SyntaxError("Unexpected end of JSON input");
            if (!isdigit(str[i]))
                throw SyntaxError(joinToString("Unexpected token ", str[i], " in JSON at position ", i));
            while (isdigit(str[i]))
            {
                numStr += str[i];
                i++;
            }
        }
        if (str[i] == 'e' || str[i] == 'E') // parse Scientific notation
        {
            numStr += 'e';
            i++; // skip e/E
            if (i >= str.length())
                throw SyntaxError("Unexpected end of JSON input");
            if (str[i] == '-' || str[i] == '+')
            {
                numStr += str[i];
                i++;
            }
            if (i >= str.length())
                throw SyntaxError("Unexpected end of JSON input");
            if (!isdigit(str[i]))
                throw SyntaxError(joinToString("Unexpected token ", str[i], " in JSON at position ", i));
            while (isdigit(str[i]))
            {
                numStr += str[i];
                i++;
            }
        }

        return std::stod(numStr);
    }

    Null parseNull(const std::string &str, size_t &i)
    {
        if (str.substr(i, 4) == "null")
        {
            i += 4;
            return nullptr;
        }
        throw SyntaxError(joinToString("Unexpected token ", str[i], " in JSON at position ", i));
    }

    Boolean parseTrue(const std::string &str, size_t &i)
    {
        if (str.substr(i, 4) == "true")
        {
            i += 4;
            return (true);
        }
        throw SyntaxError(joinToString("Unexpected token ", str[i], " in JSON at position ", i));
    }

    Boolean parseFalse(const std::string &str, size_t &i)
    {
        if (str.substr(i, 5) == "false")
        {
            i += 5;
            return (false);
        }
        throw SyntaxError(joinToString("Unexpected token ", str[i], " in JSON at position ", i));
    }

    Object parseObject(const std::string &str, size_t &i)
    {
        i++; // skip '{'
        Object result;
        while (str[i] != '}')
        {
            skipBlank(str, i); // skip blank to find key
            std::string key = parseString(str, i);
            skipBlank(str, i); // skip blank to find colon
            if (str[i] == ':')
                i++;
            else
                throw SyntaxError(joinToString("Expect ':' at ", i, " but found ", str[i]));
            auto value = parseValue(str, i);
            auto entry = std::make_pair<String, Value>(std::move(key), std::move(value));
            result.insert(std::move(entry));
            skipBlank(str, i);
            if (str[i] == ',')
                i++; // skip sep_comma
            else
                skipBlank(str, i); // skip blank to find '}'
        }
        i++; // skip '}'
        return result;
    }
    Array parseArray(const std::string &str, size_t &i)
    {
        i++;
        Array result;
        while (str[i] != ']')
        {
            result.push_back(parseValue(str, i));
            if (str[i] == ',')
                i++; // skip sep_comma
            else
                skipBlank(str, i); // skip blank to find ']'
        }
        i++; // skip ']'
        return result;
    }

    String stringify(const Value &value, unsigned int indent)
    {
        switch (value.getType())
        {
        case Type::string:
            return stringifyString(value.getString());
        case Type::number:
            return stringifyNumber(value.getNumber());
        case Type::boolean:
            return stringifyBoolean(value.getBoolean());
        case Type::null:
            return stringifyNull(value.getNull());
        case Type::array:
            return stringifyArray(value.getArray(), indent);
        case Type::object:
            return stringifyObject(value.getObject(), indent);
        default:
            return "null";
        }
    }

    String addIndent(const std::string &s, unsigned int indent = 0)
    {
        std::stringstream input, output;
        input.str(s);
        std::string item;
        while (std::getline(input, item))
        {
            for (unsigned int i = 0; i < indent; i++)
                output << ' ';
            output << item << '\n';
        }
        std::string out_str = output.str();
        out_str.erase(out_str.length() - 1, 1);
        return out_str;
    }

    String stringifyArray(const Array &value, unsigned int indent)
    {
        String rst = "[";
        for (auto item : value)
        {
            auto lines = stringify(item, indent);
            if (indent)
            {
                rst.append("\n");
                lines = addIndent(lines, indent);
            }
            rst.append(lines);
            rst.append(",");
        }
        rst.erase(rst.length() - 1, 1);
        rst.append(indent ? "\n]" : "]");
        return rst;
    }

    String stringifyObject(const Object &value, unsigned int indent)
    {
        String rst = "{";
        for (auto item : value)
        {
            auto kv_string = joinToString(stringifyString(item.first), indent ? ": " : ":", stringify(item.second, indent));
            if (indent > 0)
            {
                rst.append("\n");
                kv_string = addIndent(kv_string, indent);
            }
            rst.append(kv_string);
            rst.append(",");
        }
        rst.erase(rst.length() - 1, 1);
        rst.append(indent ? "\n}" : "}");
        return rst;
    }
}
