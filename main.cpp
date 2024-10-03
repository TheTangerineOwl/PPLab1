#include "Header.h"
using namespace std;
using namespace chrono;

int n = 2048000;
int k = 1;
double a = 0;
double b = 1024*3.14;

typedef struct args_struct {
	int id;
	double res = 0;
	time_point<steady_clock> threadtime;
} args;

double func(double x) {
	//return - (x - 8.0) * (x - 8.0) + 64.0;

	return sin(x);
}

void* threadfunc(void* v) {
	args* pars = (args*)v;
	double res = 0;
	double h = (b - a) / n;

	// обычно от 0 до n
	// для k потоков 1 от 0 до n/k - 1 включ, 2 от n/k до 2n/k - 1 включ
	for (int step = pars->id * n / k; step < (pars->id + 1) * n / k; step++) {
		double x1 = a + step * h;
		double x2 = a + ((double)step + 1) * h;
		res += (x2 - x1) / 6.0 * (func(x1) + 4.0 * func(0.5 * (x1 + x2)) + func(x2));
	}

	pars->res = res;
	pars->threadtime = high_resolution_clock::now();
	
	return 0;
}

int main() {
	setlocale(LC_ALL, "Russian");
	args* params = new args[k];
	pthread_t* tid = new pthread_t[k];

	time_point<steady_clock> start = high_resolution_clock::now();

	double res = 0;

	for (int i = 0; i < k; i++) {
		params[i].id = i;
		pthread_create(&tid[i], NULL, threadfunc, (void*)&params[i]);
	}
	for (int i = 0; i < k; i++) {
		pthread_join(tid[i], NULL);//////////////////////////////////////////

		cout << "Поток id: " << i << ", результат " << params[i].res << ", время выполнения " << duration_cast<milliseconds>(params[i].threadtime - start).count() << " мсек"  << endl;

		res += params[i].res;
	}
	
	cout << "Итоговый результат: " << res << endl;
	cout << "Итоговое время (мсек): " << duration_cast<milliseconds>(high_resolution_clock::now() - start).count() << endl;

	return 0;
}