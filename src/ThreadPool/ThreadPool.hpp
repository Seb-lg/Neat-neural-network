//
// Created by seb on 23/12/18.
//

#ifndef ECS_THREADPOOL_HPP
#define ECS_THREADPOOL_HPP

#include <vector>
#include <queue>
#include <thread>
#include <functional>
#include <mutex>
#include <iostream>
#include <atomic>

class ThreadPool {
public:
	ThreadPool(int nbSlaves = 4): slaves(nbSlaves)
	{
		alive = true;
		status.resize(slaves, true);
		workers.reserve(slaves);
		missingDone = 0;

		for (int i = 0; i < slaves; i++) {
			workers.emplace_back([this, i](){
				std::function<void()> ob;
				while (alive) {
					mutex.lock();
					if (!work.empty()) {
						status[i] = false;
						ob = work.front();
						work.pop();
						mutex.unlock();
						ob();
						missingDone--;
						status[i] = true;
					} else {
						mutex.unlock();
					}
				}
			});
		}
	}

	~ThreadPool() {
		alive = false;
		for (int i = 0; i < slaves; i++)
			workers[i].join();
	}

	void addTask(std::function<void()> newWork) {
		work.push(newWork);
		missingDone++;
	}

	void lockWork(){
		mutex.lock();
	}

	void unlockWork() {
		mutex.unlock();
	}

	bool isDone() {
		return (missingDone == 0);
	}

private:
	std::vector<bool>			status;
	std::vector<std::thread>		workers;
	bool 					alive;
	std::mutex				mutex;
	std::queue<std::function<void()>>	work;
	std::atomic<unsigned int>		missingDone;
	int					slaves;
};


#endif //ECS_THREADPOOL_HPP
