#include <Logging.h>
#include <ctime>

int main()
{
	WARNING_LOG("Here we go: {0} {2} {1} {0}", "zero", 1.0, 2, -2);

	std::cin.get();
	return 0;
}