// A C++ implementation of Dmitry Vyukov's lock free MPSC queue from:
// 	http://www.1024cores.net/home/lock-free-algorithms/queues/non-intrusive-mpsc-node-based-queue
//
// Adaptated from Matt Stump's version at:
// 	https://github.com/mstump/queues/blob/master/include/mpsc-queue.hpp
//
// This version is intentionally sub-optimal & 'dumbed down', in that it:
// 	1. Does not specify memory order
//	2. Does not attempt to align storage size
//

#ifndef __MPSCQ_H__
#define __MPSCQ_H__

#include <atomic>

template<typename T> class mpscq {
public:
	// Enqueue
	void enqueue(const T& input) {
		__node__ *N;
		__node__ *P;

		N = new __node__;
		N->data = input;
		N->next.store(nullptr);

		P = __head__.exchange(N);
		P->next.store(N);
	}

	// Dequeue
	bool dequeue(T& output) {
		__node__ *tail;
		__node__ *next;

		tail = __tail__.load();
		next = tail->next.load();

		if (next == nullptr) {
			return false;
		}

    		output = next->data;
		__tail__.store(next);

		delete tail;
		return true;
	}

	// Constructor
	mpscq() {
		__node__ *front;

		__head__ = new __node__;
		__tail__ = __head__.load();

		front = __head__.load();
		front->next.store(nullptr);
	}

	// Destructor
	~mpscq() {
		T output;
		__node__ *front;

		while( dequeue(output) ) {
			// empty
		}

		front = __head__.load();
		delete front;
	}

private:
	struct __node__ {
		T data;
		std::atomic<__node__ *> next;
	};

	std::atomic<__node__ *> __head__;
	std::atomic<__node__ *> __tail__;

	// Block copy constructor
	mpscq(const mpscq&) {
		// empty
	}

	// Block assignment
	void operator=(const mpscq&) {
		// empty
	}
};

#endif
