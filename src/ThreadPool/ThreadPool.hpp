//
// Created by seb on 23/12/18.
//

#ifndef ECS_THREADPOOL_HPP
#define ECS_THREADPOOL_HPP

#include <vector>
#include <list>
#include <thread>
#include <functional>
#include <mutex>
#include <iostream>

template <typename T>
class ThreadPool {
public:
	ThreadPool(std::function<void(T)> func, int nbSlaves = 4): work(nbSlaves), slaves(nbSlaves)
	{
		alive = true;
		status.resize(slaves, true);
		workers.reserve(slaves);

		for (int i = 0; i < slaves; i++) {
			workers.emplace_back([this, func, i](){
				T ob;
				while (alive) {
					work[i].first.lock();
					if (!work[i].second.empty()) {
						status[i] = false;
						ob = work[i].second.back();
						work[i].second.pop_back();
						work[i].first.unlock();
						func(ob);
						status[i] = true;
					} else {
						work[i].first.unlock();
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

	void addTask(T newWork) {
		static int i = 0;

		work[i].second.emplace_front(newWork);
		i++;
		i = i % slaves;
	}

	void lockWork(){
		for (auto &mut : work)
			mut.first.lock();
	}

	void unlockWork() {
		for (auto &mut : work)
			mut.first.unlock();
	}

	bool isDone() {
		for (int i = 0; i < slaves; i++)
			if (status[i])
				return false;
		return true;
	}

private:
	std::vector<bool>					status;
	std::vector<std::thread>				workers;
	bool 							alive;
	std::vector<std::pair<std::mutex, std::list<T>>>	work;
	int							slaves;
};


#endif //ECS_THREADPOOL_HPP
