#include "json.hpp"

namespace JSON
{

    Value parse(std::string str)
    {
        size_t i = 0;
        auto rst = parseValue(str, i);
        skipBlank(str, i);
        if (i != str.length())
            throw SyntaxError("Unexpected end of JSON input");
        return rst;
    };

    void skipBlank(std::string &str, size_t &i)
    {
        while (str[i] == ' ' || str[i] == '\n' || str[i] == '\r' || str[i] == '\t')
            i++;
    }
    Value parseValue(std::string &str, size_t &i)
    {

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
        else if ((str[i] >= '0' && str[i] <= '9') || str[i] == '-')
            value.setNumber(parseNumber(str, i));
        else
            throw SyntaxError(joinToString("Unexpected token ", str[i], " in JSON at position ", i));
        return value;
    }
    String *parseString(std::string &str, size_t &i)
    {
        String *result = new std::string("");
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
                    (*result) += '"';
                    break;
                case '\\':
                    (*result) += "\\";
                    break;
                case '/':
                    (*result) += "/";
                    break;
                case 'b':
                    (*result) += "\b";
                    break;
                case 'f':
                    (*result) += "\f";
                    break;
                case 'n':
                    (*result) += "\n";
                    break;
                case 'r':
                    (*result) += "\r";
                    break;
                case 't':
                    (*result) += "\t";
                    break;
                case 'u':
                    // TODO
                    throw SyntaxError("Unimplemented yet");

                default:
                    throw SyntaxError(joinToString("Unexpected token ", str[i], " in JSON at position ", i));
                }
                i++; // skip common '\'
            }
            else
            {
                (*result) += str[i];
                i++;
            }
        }
        i++; // skip right colon
        return result;
    }
    Number *parseNumber(std::string &str, size_t &i)
    {
        // TODO: support e/E
        std::string numStr = "";
        if (str[i] == '-')
        {
            numStr += '-';
            i++;
        }
        while (isalnum(str[i]))
        {
            numStr += str[i];
            i++;
        }
        if (str[i] == 'e' || str[i] == 'E')
        {
            // TODO
            throw SyntaxError("Unimplemented yet");
        }
        if (str[i] == '.')
        {
            numStr += '.';
            i++;
            if (!isalnum(str[i]))
            {
                throw SyntaxError(joinToString("Unexpected token ", str[i], " in JSON at position ", i));
            }
            while (isalnum(str[i]))
            {
                numStr += str[i];
                i++;
            }
        }

        Number *val = new Number;
        *val = std::stod(numStr);
        return val;
    }
    Null *parseNull(std::string &str, size_t &i)
    {
        if (str.substr(i, 4) == "null")
        {
            i += 4;
            return nullptr;
        }
        throw SyntaxError(joinToString("Unexpected token ", str[i], " in JSON at position ", i));
    }
    Boolean *parseTrue(std::string &str, size_t &i)
    {
        if (str.substr(i, 4) == "true")
        {
            i += 4;
            return new Boolean(true);
        }
        throw SyntaxError(joinToString("Unexpected token ", str[i], " in JSON at position ", i));
    }
    Boolean *parseFalse(std::string &str, size_t &i)
    {
        if (str.substr(i, 5) == "false")
        {
            i += 5;
            return new Boolean(false);
        }
        throw SyntaxError(joinToString("Unexpected token ", str[i], " in JSON at position ", i));
    }
    Object *parseObject(std::string &str, size_t &i)
    {
        i++; // skip '{'
        Object *result = new Object();
        while (str[i] != '}')
        {
            skipBlank(str, i); // skip blank to find key
            std::string key = *parseString(str, i);
            skipBlank(str, i); // skip blank to find colon
            if (str[i] == ':')
            {
                i++;
            }
            else
            {
                throw SyntaxError(joinToString("Expect ':' at ", i, " but found ", str[i]));
            }
            auto value = parseValue(str, i);
            (*result)[key] = value;
            skipBlank(str, i);
            if (str[i] == ',')
            {
                i++; // skip sep_comma
            }
            else
            {
                skipBlank(str, i); // skip blank to find '}'
            }
        }
        i++; // skip '}'
        return result;
    }
    Array *parseArray(std::string &str, size_t &i)
    {
        i++;
        Array *result = new Array();
        while (str[i] != ']')
        {
            result->push_back(parseValue(str, i));
            if (str[i] == ',')
            {
                i++; // skip sep_comma
            }
            else
            {
                skipBlank(str, i); // skip blank to find ']'
            }
        }
        i++; // skip ']'
        return result;
    }

    String stringify(Value value, unsigned int indent)
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

    String add_indent(String lines, unsigned int indent = 0)
    {
        std::stringstream ss;
        const char *token = "\r\n";
        char *copy = new char(strlen(lines.c_str()));
        strcpy(copy, lines.c_str());
        char *p = strtok(copy, token);
        while (p)
        {
            for (unsigned int i = 0; i < indent; i++)
                ss << ' ';
            ss << p;
            ss << '\n';
            p = strtok(NULL, token);
        }
        auto rst = ss.str();
        rst.erase(rst.length() - 1, 1);
        return rst;
    }

    String stringifyArray(Array value, unsigned int indent)
    {
        String rst = "[";
        for (auto item : value)
        {
            auto lines = stringify(item, indent);
            if (indent)
            {
                rst.append("\n");
                lines = add_indent(lines, indent);
            }
            rst.append(lines);
            rst.append(",");
        }
        rst.erase(rst.length() - 1, 1);
        rst.append(indent ? "\n]" : "]");
        return rst;
    }

    String stringifyObject(Object value, unsigned int indent)
    {
        String rst = "{";
        for (auto item : value)
        {
            auto lines = joinToString(stringifyString(item.first), indent ? ": " : ":", stringify(item.second, indent));
            if (indent)
            {
                rst.append("\n");
                lines = add_indent(lines, indent);
            }
            rst.append(lines);
            rst.append(",");
        }
        rst.erase(rst.length() - 1, 1);
        rst.append(indent ? "\n}" : "}");
        return rst;
    }
}
