#pragma once

#ifdef _DEBUG
	#define WARNING_LOG(...) Utility::log(Utility::LogType::Warning, __VA_ARGS__)
	#define ERROR_LOG(...) Utility::log(Utility::LogType::Error, __VA_ARGS__)
	#define INFO_LOG(...) Utility::log(Utility::LogType::Info, __VA_ARGS__)
#else
	#define WARNING_LOG(...)
	#define ERROR_LOG(...)
	#define INFO_LOG(...)
#endif

/* TODO:

    - Structure the includes and macros definitions (pch's, platform)
    - Add write to file option (for all platforms)
    - Add colors (for all platforms)
    - Add seperate thread option
	- Add more type support
	* Add advanced formatting options
*/

#define INVALID_INDEX -1
#define MAX_PADDING 15

#include <cstdio>
#include <iostream>
#include <tuple>
#include <exception>

namespace Utility {

	enum class LogType { Warning = 0, Error, Info };

	LogType lastLog = LogType::Info;

	template<typename... Args>
	static inline void log(LogType logType, const char* format, Args... args)
	{
		if (lastLog != logType)
		{
			switch (logType)
			{
				case LogType::Warning:
					system("color 0E");
					break;
				case LogType::Error:
					system("color C");
					break;
				case LogType::Info:
					system("color 7");
					break;
			}

			lastLog = logType;
		}

		std::printf(parseFormat(logType, format, args...), args...);
	}

	namespace {

		template<typename... Args>
		constexpr const char* parseFormat(LogType logType, const char* format, Args... args)
		{
			char* parsed = new char[std::strlen(format) + MAX_PADDING];
			std::size_t end = 0;
			int argIndex = INVALID_INDEX;

			switch (logType)
			{
			case LogType::Warning:
				strncpy(parsed, "Warning: ", strlen("Warning: "));
				end += strlen("Warning: ");
				break;
			case LogType::Error:
				strncpy(parsed, "Error: ", strlen("Error: "));
				end += strlen("Error: ");
				break;
			case LogType::Info:
				strncpy(parsed, "Info: ", strlen("Error: "));
				end += strlen("Info: ");
				break;
			}

			while (*format != '\0')
			{
				argIndex = getIndex(format, true);

				if (argIndex != INVALID_INDEX)
				{
					parsed[end] = '%';
					parsed[end + 1] = getSpecifier(args..., argIndex);
					end += 2;
				}
				else
				{
					parsed[end] = *format;
					format++;
					end++;
				}
			}

			parsed[end] = '\0';
			return parsed;
		}

		constexpr int getIndex(const char*& format, bool advance = false)
		{
			int index = 0;
			int formatIndex = 1;

			if (*format != '{')
				return INVALID_INDEX;

			while (*(format + formatIndex) != '}')
			{
				if (*(format + formatIndex) == '\0')
					throw std::exception("Argument brackets were not closed");
				if (*(format + formatIndex) < '0' || *(format + formatIndex) > '9')
					throw std::exception("Argument index was not an integer");

				index *= 10;
				index += *(format + formatIndex) - '0';

				if (advance)
					format++;
				else
					formatIndex++;
			}

			if (advance)
				format += 2;

			return index;
		}

		template<typename F, typename... Args>
		constexpr char getSpecifier(F first, Args... args)
		{
			if (getArgumentByIndex<sizeof...(Args)-1>(args...) == 0)
			{
				if (std::is_same<F, int>::value)
					return 'd';
				else if (std::is_same<F, double>::value)
					return 'f';
				else if (std::is_same<F, float>::value)
					return 'f';
				else if (std::is_same<F, char>::value)
					return 'c';
				else if (std::is_same<F, char*>::value)
					return 's';
				else if (std::is_same<F, const char*>::value)
					return 's';
				else if (std::is_same<F, unsigned int>::value)
					return 'i';
				else
					throw std::exception("Unsupported argument type");
			}

			return getSpecifier(args..., getArgumentByIndex<sizeof...(Args)-1>(args...) - 1);
		}

		template <std::size_t I, class... Args>
		decltype(auto) getArgumentByIndex(Args&&... args)
		{
			return std::get<I>(std::forward_as_tuple(args...));
		}

	}

}

#undef INVALID_INDEX
#undef MAX_PADDING