#include <Logging.h>
#include <ctime>

int main()
{
	char input[200];
	fgets(input, 199, stdin);

	WARNING_LOG("this variable is unsafe: {0}", input);

	std::cin.get();
	return 0;
}