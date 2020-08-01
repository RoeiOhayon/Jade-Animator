#include <Logging.h>
#include <ctime>

int main()
{
	std::time_t beg = clock();
	
	for (int i = 0; i < 30000; i++)
	{
		WARNING_LOG("Here we go: {0} {2} {1} {0}\n", "zero", 1.0, 2);
		WARNING_LOG("different params: {0} {0} {0} {0}\n", "haha");
	}

	std::time_t mid = clock();
	
	system("color a");
	for (int i = 0; i < 30000; i++)
	{
		std::printf("Warning: Here we go: %s %d %f %s\n", "zero", 2, 1.0, "zero");
		std::printf("Warning: different params: %s %s %s %s\n", "haha", "haha", "haha", "haha");
	}

	std::time_t end = clock();



	std::cout << "\nMy Log: " << float(mid - beg) / CLOCKS_PER_SEC << '\n'
			  << "Printf: " << float(end - mid) / CLOCKS_PER_SEC;

	std::cin.get();
	return 0;
}