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
		if (i > str.length())
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
			throw SyntaxError(std::string("Unexpected token ") + (str[i]) + " in JSON at position " + std::to_string(i));
		return value;
	}
	String parseString(const std::string &str, size_t &i)
	{
		String result;
		i++; // skip left colon
		while (str[i] != '"')
		{
			if (str[i] == '\n')
				throw SyntaxError(std::string("Bad control character in string literal in JSON at position ") + std::to_string(i));

			if (str[i] == '\\')
			{
				i++; // skip first '\'

				if (strchr("/bfnrt\"", str[i]) != NULL)
				{
					// there character should escaped with double backlash, so only one backslash is not allowed
					throw SyntaxError(std::string("Unexpected token ") + (str[i]) + " in JSON at position " + std::to_string(i));
				}
				else
				{
					// check the char behind the first backlash
					// if is a double backslash (or \uXXXX), then we need to parse it
					// otherwise, simply append to result string
					if (str[i] == '\\')
					{
						// here we match the escape char behind the first backlash
						i++; // skip second '\'
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
						{
							i++; // skip 'u'
							String hexStr = str.substr(i, 4);
							if (hexStr.length() != 4)
								throw SyntaxError("Unexpected end of JSON input");
							for (auto ch : hexStr)
								if (!isHexChar(ch))
									throw SyntaxError("Invalid Unicode escape sequence");
							result += unicode_to_utf8(std::stoi(hexStr, 0, 16));
							i += 3; // skip 3 hex digits, the rest 1 degit will be skip below (*)
							break;
						}
						default:
							throw SyntaxError(std::string("Unexpected token ") + (str[i]) + " in JSON at position " + std::to_string(i));
						}
						i++; // skip one escape char (*)
					}		 // END IF
					else if (str[i] == 'u')
					{
						i++; // skip 'u'
						String hexStr = str.substr(i, 4);
						if (hexStr.length() != 4)
							throw SyntaxError("Unexpected end of JSON input");
						for (auto ch : hexStr)
							if (!isHexChar(ch))
								throw SyntaxError("Invalid Unicode escape sequence");
						result += unicode_to_utf8(std::stoi(hexStr, 0, 16));
						i += 4; // skip 4 hex digits (for parsing unicode)
					}
					else
					{
						// if is not double backlash, no need to parse, simply append the char after the first backlash to the result
						result += str[i];
						i++;
					} // END ELSE
				}	  // END ELSE
			}		  // END IF '\'
			else
			{
				// simply append to the result
				result += str[i];
				i++;
			}
		}	 // END WHILE
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
				throw SyntaxError(std::string("Unexpected token ") + (str[i]) + " in JSON at position " + std::to_string(i));
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
				throw SyntaxError(std::string("Unexpected token ") + (str[i]) + " in JSON at position " + std::to_string(i));
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
		throw SyntaxError(std::string("Unexpected token ") + (str[i]) + " in JSON at position " + std::to_string(i));
	}

	Boolean parseTrue(const std::string &str, size_t &i)
	{
		if (str.substr(i, 4) == "true")
		{
			i += 4;
			return (true);
		}
		throw SyntaxError(std::string("Unexpected token ") + (str[i]) + " in JSON at position " + std::to_string(i));
	}

	Boolean parseFalse(const std::string &str, size_t &i)
	{
		if (str.substr(i, 5) == "false")
		{
			i += 5;
			return (false);
		}
		throw SyntaxError(std::string("Unexpected token ") + (str[i]) + " in JSON at position " + std::to_string(i));
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
				throw SyntaxError("Expect ':' at " + std::to_string(i) + " but found " + (str[i]));
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
			auto kv_string = stringifyString(item.first) + (indent ? ": " : ":") + stringify(item.second, indent);
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
