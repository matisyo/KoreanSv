/*
 * StringHelper.h
 *
 *  Created on: Apr 2, 2016
 *      Author: gonzalo
 */

#ifndef SRC_SERVER_STRINGHELPER_H_
#define SRC_SERVER_STRINGHELPER_H_

#include <string>
#include <cstring>
#include <stdexcept>

class StringHelper
{
public:
	static bool validateInt(const std::string& stringToValidate)
	{
		for (uint i = 0 ; i < stringToValidate.length(); i++)
		{
			if (!std::isdigit(stringToValidate[i]))
				return false;
		}
		try {
		  int convertedNumber = stoi(stringToValidate);
		}
		catch(std::invalid_argument& e){
		  return false;
		}
		catch(std::out_of_range& e){
			return false;
		}

		return true;
	}

	static bool validateChar(const std::string& stringToValidate)
	{
		if (stringToValidate.length() > 1)
			return false;
		return true;
	}

	static bool validateDouble(const std::string& stringToValidate)
	{
		bool pointFound = false;
		for (uint i = 0 ; i < stringToValidate.length(); i++)
		{
			if (stringToValidate[i] == '.')
			{
				if (pointFound)
					return false;
				pointFound = true;
			}
			if (!std::isdigit(stringToValidate[i]) && (stringToValidate[i] != '.'))
				return false;
		}
		try {
		  double convertedNumber = stod(stringToValidate);
		}
		catch(std::invalid_argument& e){
		  return false;
		}
		catch(std::out_of_range& e){
			return false;
		}

		return true;
	}
};



#endif /* SRC_SERVER_STRINGHELPER_H_ */
