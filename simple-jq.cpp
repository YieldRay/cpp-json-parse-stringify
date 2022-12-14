/**
 * This application is just for test
 */
#include "src/json.cpp"
#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>

using std::string, std::stringstream, std::cin, std::cout, std::vector, std::getline, std::stoi, std::to_string;

string read_from_stdin()
{
	string s;
	char ch;
	while ((ch = cin.get()) != EOF)
		s += ch;
	return s;
}

string read_first_argv(int argc, char **argv, int index = 0)
{
	string s = "";
	if (argc >= 2)
		s = string(argv[index + 1]);

	return s;
}

vector<string> string_split(const string &str, const char delim = '.')
{
	vector<string> vec;
	if (str.length() == 0)
		return vec;

	stringstream ss;
	if (str.at(0) == delim)
		ss << str.substr(1);
	else
		ss << str;

	for (string item; getline(ss, item, delim);)
		vec.push_back(string(item));

	return vec;
}

std::string print_vec_string(std::vector<string> vec)
{
	std::string s = "";
	for (int i = 0; i < vec.size(); i++)
	{
		s += "(" + std::to_string(i) + ") " + vec.at(i) + "\n";
	}
	return s;
}

class Error : public std::runtime_error
{
	using std::runtime_error::runtime_error;
};

JSON::Value get_final_value(JSON::Value json, vector<string> props)
{
	if (props.size() == 0)
		return json;

	JSON::Value final = json;
	for (auto prop : props)
	{
		if (final.getType() == JSON::Type::object)
		{
			try
			{
				final = final.getObject().at(prop);
				continue;
			}
			catch (std::out_of_range err)
			{
				throw Error("Key `" + prop + "` does not exist\n\tOn " + JSON::stringify(final));
			}
		}

		if (final.getType() == JSON::Type::array)
		{
			for (char ch : prop)
			{
				if (ch > '9' || ch < '0')
					throw Error("Key `" + prop + "` can not used for indexing array\n\tOn " + JSON::stringify(final));
			}
			int index = stoi(prop);
			try
			{
				final = final.getArray().at(index);
				continue;
			}
			catch (std::out_of_range err)
			{
				throw Error("Index `" + to_string(index) + "` is out of range, max index is " + to_string(final.getArray().size() - 1) + "\n\tOn " + JSON::stringify(final));
			}
		}

		throw Error("Key `" + prop + "` does not exist\n\tOn " + JSON::stringify(final));
	}
	return final;
}

int main(int argc, char **argv)
{
	try
	{
		auto cmd = read_first_argv(argc, argv);
		auto jq = string_split(cmd);
		auto input = read_from_stdin();
		JSON::Value json = JSON::parse(input);
		auto value = get_final_value(json, jq);
		cout << ((value.getType() == JSON::Type::string) ? value.getString() : JSON::stringify(value, 4));
	}
	catch (std::exception e)
	{
		cout << std::endl
			 << e.what();
		return 1;
	}
	return 0;
}