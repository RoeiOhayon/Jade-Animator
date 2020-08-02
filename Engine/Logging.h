#pragma once

#ifdef _DEBUG
	#define WARNING_LOG(fmt, ...) Logging::log(Logging::LogType::Warning, fmt, false, __VA_ARGS__)
	#define ERROR_LOG(fmt, ...) Logging::log(Logging::LogType::Error, fmt, false, __VA_ARGS__)
	#define INFO_LOG(fmt, ...) Logging::log(Logging::LogType::Info, fmt, false, __VA_ARGS__)
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
    - Add colors (for all platforms)
    - Add seperate thread option

	* Add advanced formatting options
*/

#define MAX_ARG_LEN 256
#define INVALID_INDEX -1

#include <cstdio>
#include <iostream>
#include <sstream>
#include <exception>
#include <vector>
#include <tuple>

namespace Logging {

	enum class LogType { Warning = 0, Error, Info, None };

	template<typename... Args>
	void fileLog(LogType logType, const char* fileName, Args... args)
	{
		log(LogType, fileName, true, args);
	}

	template<typename... Args>
	void log(LogType logType, const char* format, bool toFile,  Args... args)
	{
		FILE* destFile;

		setColor(logType);

		if (sizeof...(args) == 0)
		{
			if (toFile)
			{
				destFile = fopen(format, "a");
				std::vfprintf(destFile, format, nullptr);
				fclose(destFile);
			}
			else
				std::vprintf(format, nullptr);
		}
		else
		{
			std::vector<char> buffer;
			char* result;
			int argIndex = INVALID_INDEX;

			// assuming average of argument length is 2
			buffer.reserve(std::strlen(format) + sizeof...(args) * 2);

			switch (logType)
			{
				case LogType::Warning:
					copyStrToVec("Warning: ", buffer);
					break;
				case LogType::Error:
					copyStrToVec("Error: ", buffer);
					break;
				case LogType::Info:
					copyStrToVec("Info: ", buffer);
					break;
				case LogType::None:
					break;
			}

			while (*format != '\0')
			{
				argIndex = getIndex(format, true);

				if (argIndex != INVALID_INDEX)
				{
					loadArgument(args..., argIndex);
					copyStrToVec(argumentBuffer, buffer);
				}
				else
				{
					buffer.push_back(*format);
					format++;
				}
			}

			buffer.push_back('\0');
			result = new char[buffer.size()];
			copyVecToStr(buffer, result);

			if (toFile)
			{
				destFile = fopen(format, "a");
				std::vfprintf(destFile, result, nullptr);
				fclose(destFile);
			}
			else
				std::vprintf(result, nullptr);
			
			delete[] result;
		}
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
				case LogType::None:
					system("color 7");
					break;
			}

			logTypeCache = logType;
		}

		static inline void copyVecToStr(std::vector<char> vec, char*& str)
		{
			for (int i = 0; i < vec.size(); i++)
				str[i] = vec[i];
		}

		static inline void copyStrToVec(const char* str, std::vector<char>& vec)
		{
			while (*str != '\0')
			{
				vec.push_back(*str);
				str++;
			}
		}

		int getIndex(const char*& format, bool advance = false)
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
		void loadArgument(F first, Args... args)
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
				else if (std::is_same<F, unsigned char>::value)
					std::sprintf(argumentBuffer, "%u\0", first);
				else if (std::is_same<F, std::string>::value)
					std::sprintf(argumentBuffer, "%s\0", ((std::string*)&first)->c_str());
				else
				{
					std::stringstream ss;
					ss << argumentBuffer;
					ss >> argumentBuffer;
				}
			}
			else
				loadArgument(args..., getArgumentByIndex<sizeof...(Args)-1>(args...) - 1);
		}
		#pragma warning( pop )

		template <std::size_t I, class... Args>
		static inline decltype(auto) getArgumentByIndex(Args&&... args)
		{
			return std::get<I>(std::forward_as_tuple(args...));
		}
	}

}

#undef MAX_ARG_LEN
#undef INVALID_INDEX