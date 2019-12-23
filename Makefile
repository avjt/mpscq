mpscq_test: mpscq_test.cc mpscq.h
	g++ -std=c++17 -O3 -o $@ mpscq_test.cc -lpthread

clean:
	rm -rf code mpscq_test a.out
