#pragma once

#ifdef _DEBUG
	#define WARNING_LOG(...) Logging::log(Logging::LogType::Warning, __VA_ARGS__, 0)
	#define ERROR_LOG(...) Logging::log(Logging::LogType::Error, __VA_ARGS__, 0)
	#define INFO_LOG(...) Logging::log(Logging::LogType::Info, __VA_ARGS__, 0)
#else
	#define WARNING_LOG(...)
	#define ERROR_LOG(...)
	#define INFO_LOG(...)
#endif

/* Example of use:

	- WARNING_LOG("Hello {1} {0}", "world", "my");
	  will output: hello my world
*/

// If there are more utilities change the namespace to utils or something
/* TODO:
    - Add write to file option (for all platforms)
    - Add colors (for all platforms)
    - Add seperate thread option
	- Add more type support
	* Add advanced formatting options
*/

#define MAX_ARG_LEN 256
#define INVALID_INDEX -1

#include <cstdio>
#include <iostream>
#include <exception>
#include <vector>
#include <tuple>

namespace Logging {

	enum class LogType { Warning = 0, Error, Info };

	template<typename... Args>
	static constexpr inline void log(LogType logType, const char* format, Args... args)
	{
		setColor(logType);
		std::vprintf(evaluate(logType, format, args...), nullptr);
	}

	template<std::uint8_t>
	static inline void log(LogType logType, const char* format)
	{
		setColor(logType);
		std::vprintf(format, nullptr);
	}

	namespace {

		static char argumentBuffer[MAX_ARG_LEN] = { 0 };

		static inline void setColor(LogType logType)
		{
			static LogType logTypeCache = LogType::Info;

			if (logTypeCache == logType)
				return;

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

			logTypeCache = logType;
		}

		template<typename... Args>
		static constexpr inline const char* evaluate(LogType logType, const char* format, Args... args)
		{
			char* result;
			int argIndex = INVALID_INDEX;
			std::vector<char> evaluated;

			evaluated.reserve(std::strlen(format));

			switch (logType)
			{
				case LogType::Warning:
					copyStrToVec("Warning: ", evaluated);
					break;
				case LogType::Error:
					copyStrToVec("Error: ", evaluated);
					break;
				case LogType::Info:
					copyStrToVec("Info: ", evaluated);
					break;
			}

			while (*format != '\0')
			{
				argIndex = getIndex(format, true);

				if (argIndex != INVALID_INDEX)
				{
					loadArgument(args..., argIndex);
					copyStrToVec(argumentBuffer, evaluated);
				}
				else
				{
					evaluated.push_back(*format);
					format++;
				}
			}

			evaluated.push_back('\0');
			result = new char[evaluated.size()];
			for (int i = 0; i < evaluated.size(); i++)
				result[i] = evaluated[i];

			return result;
		}

		static constexpr inline void copyStrToVec(const char* str, std::vector<char>& vec)
		{
			while (*str != '\0')
			{
				vec.push_back(*str);
				str++;
			}
		}

		static constexpr inline int getIndex(const char*& format, bool advance = false)
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

		#pragma warning( push )
		#pragma warning( disable : 4477 )
		#pragma warning( disable : 4313 )
		template<typename F, typename... Args>
		constexpr void loadArgument(F first, Args... args)
		{
			if (getArgumentByIndex<sizeof...(Args)-1>(args...) == 0)
			{
				if (std::is_same<F, int>::value)
					std::sprintf(argumentBuffer, "%d\0", first);
				else if (std::is_same<F, double>::value)
					std::sprintf(argumentBuffer, "%f\0", first);
				else if (std::is_same<F, float>::value)
					std::sprintf(argumentBuffer, "%f\0", first);
				else if (std::is_same<F, char>::value)
					std::sprintf(argumentBuffer, "%c\0", first);
				else if (std::is_same<F, char*>::value)
					std::sprintf(argumentBuffer, "%s\0", first);
				else if (std::is_same<F, const char*>::value)
					std::sprintf(argumentBuffer, "%s\0", first);
				else if (std::is_same<F, unsigned int>::value)
					std::sprintf(argumentBuffer, "%i\0", first);
				else
					throw std::exception("Unsupported argument type");
			}
			else
				loadArgument(args..., getArgumentByIndex<sizeof...(Args)-1>(args...) - 1);
		}
		#pragma warning( pop )

		template <std::size_t I, class... Args>
		static constexpr inline decltype(auto) getArgumentByIndex(Args&&... args)
		{
			return std::get<I>(std::forward_as_tuple(args...));
		}
	}

}

#undef MAX_ARG_LEN
#undef INVALID_INDEX