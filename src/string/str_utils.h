/**
 * @file
 * @author  yaocoder <yaocoder@gamil.com>
 * @section DESCRIPTION
 * Utilities for String(thanks for James Fancy).
 */

#ifndef str_util_h__
#define str_util_h__

#include <string>
#include <vector>
#include <algorithm>

namespace str_utils
{
	inline std::string trimLeft(const std::string& str, const std::string& token)
	{
		std::string t = str;
		t.erase(0, t.find_first_not_of(token));
		return t;
	}

	inline std::string trimRight(const std::string& str, const std::string& token)
	{
		std::string t = str;
		t.erase(t.find_last_not_of(token) + 1);
		return t;
	}

	inline std::string trim(const std::string& str, const std::string& token)
	{
		std::string t = str;
		t.erase(0, t.find_first_not_of(token));
		t.erase(t.find_last_not_of(token) + 1);
		return t;
	}

	inline std::string toLower(const std::string& str)
	{
		std::string t = str;
		transform(t.begin(), t.end(), t.begin(), tolower);
		return t;
	}

	inline std::string toUpper(const std::string& str)
	{
		std::string t = str;
		transform(t.begin(), t.end(), t.begin(), toupper);
		return t;
	}

	inline bool	startsWith(const std::string& str, const std::string& substr)
	{
		return str.find(substr) == 0;
	}

	inline bool endsWith(const std::string& str, const std::string& substr)
	{
		return str.rfind(substr) == (str.length() - substr.length());
	}

	inline bool equalsIgnoreCase(const std::string& str1, const std::string& str2) 
	{
		return toLower(str1) == toLower(str2);
	}

	template<class T> std::string toString(const T& value)
	{
		std::ostringstream oss;
		oss << value;
		return oss.str();
	}

	template<class T> void string2T(const std::string& str, T& value)
	{	
		std::istringstream iss(str);
		iss >> value;
		return ;
	}

	/*
	warn: strtok is not thread-safe;
	*/
	inline std::vector<std::string> splitByDelimiter(const char* str,const char* delimiter)
	{
		char* token = strtok(const_cast<char*>(str),delimiter);

		std::vector<std::string> result;

		while(token != NULL)
		{
			result.push_back(token);
			token = strtok(NULL,delimiter);
		}
		return result;
	}

	inline std::string joinStringByToken(const std::vector<std::string>&vec, const std::string& token)
	{
		std::string str;
		for(unsigned int i = 0; i < vec.size(); ++i)
		{
			str = str + vec.at(i) + token;
		}

		return str;
	}
	
	static inline void SplitStringStd(const std::string& str,  const std::string& delimiter, 
					  std::vector<std::string>& vec_data)
	{
		std::string s = str;
		size_t pos = 0;
		std::string token;
		while ((pos = s.find(delimiter)) != std::string::npos) 
		{
			token = s.substr(0, pos);
			vec_data.push_back(token);
			s.erase(0, pos + delimiter.length());
		}
	}

#ifdef BOOST
	inline std::vector<std::string> splitStringBoost(const std::string& str, const std::string& delimiter)
	{
		std::vector<std::string> vec_temp;
		boost::algorithm::split(vec_temp, str, boost::algorithm::is_any_of(delimiter));
		return vec_temp;
	}

	inline std::string replaceString(const std::string& str,
		const std::string& flag,
		const std::string& replaceFlag)
	{
		boost::regex expressionReplace(flag);
		std::string strTemp = boost::regex_replace(str, expressionReplace, replaceFlag);
		return strTemp;
	}
#endif

}

#endif 
