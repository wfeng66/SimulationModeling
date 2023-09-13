#include <iostream>
#include <queue>
#include <vector>
#include "WebQ_weicong_feng.h"


int think_low = 18;
int think_high = 22;
float process_low = 0.2;
float process_high = 0.3;
int hour_running = 3;
int Active_users = 100;


int Counter_arrival = 0;
int Counter_departure = 0;
int Counter_served = 0;
float Sim_time = 0;
int n_queue = 0;								// the number of queries in the queue waiting
float t_last_even = 0;							// time of the last even
int n_delay = 0;								// total number of delay
float D = 0.0;									// total delay time
float Area_Q = 0;
float Area_B = 0;
int running_seconds = 3600 * hour_running;

std::priority_queue<EventQueue> e_queue;			// event queue
std::queue<User> w_queue;						// waiting queue
// initialize server
Server server;
UniformRandom<int> think_time(think_low, think_high);
UniformRandom<float> process_time(process_low, process_high);

void arrival(float Sim_time, User* user, EventQueue even);
void departure();


int main(){




	// initialize users
	User* users = new User[Active_users];
	for (int i = 0; i <= Active_users; i++) {
		users[i].set_uid(i);
		users[i].think(think_time, 0.0);
		e_queue.push(EventQueue(users[i].query(), "arrival", i));
	}

	// retrieve the first event
	EventQueue event = e_queue.top();
	e_queue.pop();
	Sim_time = event.time();

	// main loop
	while (Sim_time < running_seconds) {


		if (event.type() == "arrival") {
			arrival(Sim_time, users, event);
		}
		else {
			departure();
		}


		// advance time
		event = e_queue.top();
		e_queue.pop();
		Sim_time = event.time();

	}


}



void arrival(float Sim_time, User* user, EventQueue even) {
	// update Q(t) and B(t)
	Area_Q += n_queue * (Sim_time - t_last_even);
	Area_B += server.status_query() * (Sim_time - t_last_even);

	if (server.status_query() == 1) {					// server is busy
		w_queue.push(user[even.id()]);
		n_queue++;
	}
	else {												// server is idle
		server.set_status(1);
		D += 0;
		n_delay++;
	}

	t_last_even = Sim_time;								// correct place???
}