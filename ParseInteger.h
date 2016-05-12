// ParseInteger.h

// Implements a template function to parse any kind of integer from a string





#ifndef PARSEINTEGER_H
#define PARSEINTEGER_H





template <typename T>
bool parseInteger(const char * a_Str, T & a_Num, size_t a_MaxLength = std::numeric_limits<size_t>::max())
{
	size_t i = 0;
	bool isPositive = true;
	T result = 0;
	if (a_Str[0] == '+')
	{
		i++;
	}
	else if (a_Str[0] == '-')
	{
		i++;
		isPositive = false;
	}
	if (isPositive)
	{
		for (; (i < a_MaxLength) && (a_Str[i] != 0); i++)
		{
			if ((a_Str[i] < '0') || (a_Str[i] > '9'))
			{
				// Not a digit
				return false;
			}
			if (std::numeric_limits<T>::max() / 10 < result)
			{
				// Number too large to represent using the specified type
				return false;
			}
			result *= 10;
			T digit = static_cast<T>(a_Str[i] - '0');
			if (std::numeric_limits<T>::max() - digit < result)
			{
				// Number too large to represent using the specified type
				return false;
			}
			result += digit;
		}
	}
	else
	{
		// Unsigned result cannot be signed!
		if (!std::numeric_limits<T>::is_signed)
		{
			return false;
		}

		for (; a_Str[i] != 0; i++)
		{
			if ((a_Str[i] < '0') || (a_Str[i] > '9'))
			{
				// Not a digit
				return false;
			}
			if (std::numeric_limits<T>::min() / 10 > result)
			{
				// Number too small to represent using the specified type
				return false;
			}
			result *= 10;
			T digit = static_cast<T>(a_Str[i] - '0');
			if (std::numeric_limits<T>::min() + digit > result)
			{
				// Number too small to represent using the specified type
				return false;
			}
			result -= digit;
		}
	}
	a_Num = result;
	return true;
}





#endif // PARSEINTEGER_H




