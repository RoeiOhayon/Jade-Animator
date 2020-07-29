#include <Logging.h>
#include <ctime>

int main()
{
	std::clock_t begin = clock();

	for (int i = 0; i < 1000; i++)
		WARNING_LOG("Let's see {0} {1}\n", 8.6, 16);

	std::clock_t middle = clock();

	system("color a");
	for (int i = 0; i < 1000; i++)
		printf("Let's see %f %d\n", 8.6, 16);

	std::clock_t end = clock();

	std::cout << "My log: " << float(middle - begin) / CLOCKS_PER_SEC << std::endl;
	std::cout << "Printf: " << float(end - middle) / CLOCKS_PER_SEC << std::endl;

	std::cin.get();
	return 0;
}