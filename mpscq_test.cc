#include <iostream>
#include <thread>
#include <functional>

#include <cstdlib>

#include "mpscq.h"

#define N_MSG 1000000
#define	N_THR 10

long n_msg = N_MSG, n_thr = N_THR;

template<typename T> void consumer_func(T* queue)
{
	size_t count = (n_thr*n_msg);
	size_t value = 0;

	while (count > 0) {
		if (queue->dequeue(value)) {
			--count;
		}
	}
}

template<typename T> void producer_func(T* queue)
{
	for (int count = 0; count < n_msg; ++count) {
		queue->enqueue(count);
	}
}

template<typename T> long double run_test(T producer_func, T consumer_func)
{
	typedef std::chrono::high_resolution_clock clock_t;
	typedef std::chrono::time_point<clock_t> time_t;
	time_t start;
	time_t end;
	std::thread producer[10];
	

	start = clock_t::now();
	std::thread consumer(consumer_func);

	for( auto& p: producer ) {
		p = std::thread( producer_func );
	}

	for( auto& p: producer ) {
		p.join();
	}

	consumer.join();
	end = clock_t::now();

	return (end - start).count() * ((double) std::chrono::high_resolution_clock::period::num / std::chrono::high_resolution_clock::period::den);
}

int main(int C, char **V)
{
	typedef mpscq<size_t> queue_t;
	queue_t queue;
	long double seconds;

	switch(C) {
	case 3:
		n_thr = atoll( V[2] );
	case 2:
		n_msg = atoll( V[1] );
	case 1:
		break;
	default:
		std::cerr << "Usage: " << V[0] << " [n-msg] [n-thr]" << std::endl;
		return 1;
	}
	seconds = run_test(std::bind(&producer_func<queue_t>, &queue), std::bind(&consumer_func<queue_t>, &queue));

	std::cout << "MPSC dynamic queue completed "
		<< (n_msg * n_thr)
		<< " iterations in "
		<< seconds
		<< " seconds. "
		<< ((long double) (n_msg * n_thr) / seconds) / 1000000
		<< " million enqueue/dequeue pairs per second."
		<< std::endl;

	return 0;
}
