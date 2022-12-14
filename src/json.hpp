#include <string>
#include <sstream>
#include <cstring>
#include <map>
#include <vector>
#include <exception>
#include <cctype>
#include <cstddef>
#include <utility>
#include <stdexcept>
#include "./json-utils.cpp"

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

		//! init data
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
		Value(const Value &val) noexcept
		{
			this->type = val.type;
			this->data_number = val.data_number;
			this->data_boolean = val.data_boolean;
			this->data_null = val.data_null;
			this->data_string = val.data_string;
			this->data_array = val.data_array;
			this->data_object = val.data_object;
		}

		//! getters
		inline Number getNumber() const
		{
			if (type != Type::number)
				throw TypeError(String("Expect number, got ") + getTypeString());
			return data_number;
		}
		inline Boolean getBoolean() const
		{
			if (type != Type::boolean)
				throw TypeError(String("Expect boolean, got ") + getTypeString());
			return data_boolean;
		}
		inline Null getNull() const
		{
			if (type != Type::null)
				throw TypeError(String("Expect null, got ") + getTypeString());
			return data_null;
		}
		inline String getString() const
		{
			if (type != Type::string)
				throw TypeError(String("Expect string, got ") + getTypeString());
			return data_string;
		}
		inline Array getArray() const
		{
			if (type != Type::array)
				throw TypeError(String("Expect array, got ") + getTypeString());
			return data_array;
		}
		inline Object getObject() const
		{
			if (type != Type::object)
				throw TypeError(String("Expect object, got ") + getTypeString());
			return data_object;
		}

		//! setters
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

		//! type
		inline Type getType() const noexcept { return this->type; }
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

		//! free memory
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

	//! Type bindings
	using Number = double;
	using Boolean = bool;
	using Null = std::nullptr_t;
	using String = std::string;
	using Array = std::vector<Value>;
	using Object = std::map<std::string, Value>;

	//! JSON::parse
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

	//! JSON::stringify
	String stringify(const Value &value, unsigned int indent = 0);
	String stringifyArray(const Array &value, unsigned int indent = 0);
	String stringifyObject(const Object &value, unsigned int indent = 0);
	inline String stringifyBoolean(const Boolean &value) { return value ? String("true") : String("false"); }
	inline String stringifyNull(const Null &value) { return String("null"); }
	inline String stringifyNumber(const Number &value) { return std::to_string(value); }
	inline String stringifyString(const String &value) { return String("\"") + replaceAllDistinct(value, "\"", "\\\"") + String("\""); }
}
