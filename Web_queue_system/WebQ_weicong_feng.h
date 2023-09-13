#pragma once
#ifndef WebQ
#define WebQ

#include <random>

namespace WebQ {


	template <class T>
	class UniformRandom {
	public:
		UniformRandom(T init_x, T init_y) : x(init_x), y(init_y), generator(std::random_device()()), distribution(x, y) {}

		T getRandom() {
			return distribution(generator);
		}
	private:
		T x;
		T y;
		std::default_random_engine generator;
		std::uniform_int_distribution<T> distribution;
	};


	class User {
	public:
		User() : uid(0), t_query(0){};
		void set_uid(int id) { uid = id; };
		void think(UniformRandom<int>& think_t, const float sim_time) { t_query = sim_time + think_t.getRandom(); }
		float query() { return t_query; }
	private:
		int uid;
		float t_query;
	};


	class EventQueue {
	public:
		float e_time;
		std::string e_type;
		int uid;

		// Constructor
		EventQueue(int t, const std::string& type, const int id) : e_time(t), e_type(type), uid(id) {}

		std::string type() { return e_type; }
		int id() { return uid; }
		float time() { return e_time; }

		// Overload the < operator for comparison
		bool operator<(const EvenQueue& other) const {
			return e_time > other.e_time;
		}
	};


	class Server {
	public:
		Server() { is_busy = 0; }
		void set_status(int st) { is_busy = st; };
		int status_query() { return is_busy; };
	private:
		int is_busy;
	};

}

#endif