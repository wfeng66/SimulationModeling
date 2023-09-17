#include <iostream>
#include <queue>
#include <vector>
#include "WebQ_weicong_feng.h"


float think_low = 18.0;
float think_high = 22.0;
float process_low = 0.2;
float process_high = 0.3;
int hour_running = 1;
int Active_users = 100;
int queue_size = 300;


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
int running_seconds = 20 * hour_running;

std::priority_queue<EventQueue> e_queue;			// event queue
std::queue<User> w_queue;							// waiting queue
// initialize server
Server server;
UniformRandom<float> think_time(think_low, think_high);
UniformRandom<float> process_time(process_low, process_high);

void arrival(float Sim_time, User user);
void departure(float Sim_time, User departuring_user);


int main(){
	// metrics: X, S, U, W



	// initialize users
	User* users = new User[Active_users];
	for (int i = 0; i <= Active_users; i++) {
		users[i].set_uid(i);
		users[i].think(think_time, 0.0);
		e_queue.push(EventQueue(users[i].query(), "arrival", i));			// create the first arrival events for each user
	}

	// retrieve the first event
	EventQueue event = e_queue.top();
	e_queue.pop();
	Sim_time = event.time();

	std::cout << "!!!BEGIN!!!" << std::endl;

	// main loop
	while (Sim_time < running_seconds) {
		std::cout << "Sim_time" << "\t" << "n_queue"<<"\t\t\t"<< "Event Type" << "\t\t" << "uid" << "\t" << "D" << "\t\t"<<"last time"<<std::endl;
		std::cout << Sim_time << "\t\t";
		// update Q(t) and B(t)
		Area_Q += n_queue * (Sim_time - t_last_even);
		Area_B += server.status_query() * (Sim_time - t_last_even);
		std::cout << n_queue << "\t\t\t";
		if (event.type() == "arrival") {
			std::cout << "arr" << "\t\t\t"<< users[event.id()].id()<< "\t";
			arrival(Sim_time, users[event.id()]);
		}
		else {
			std::cout << "dep" << "\t\t\t" << users[event.id()].id() << "\t";
			departure(Sim_time, users[event.id()]);
		}

		t_last_even = Sim_time;								// correct place???
		std::cout << D << "\t\t" << t_last_even << std::endl;
		// advance time
		event = e_queue.top();
		e_queue.pop();
		Sim_time = event.time();

	}

	// produce report 
	float d = D / Counter_arrival;							// average delay
	float S = Area_B / Counter_arrival;						// average serving time
	float U = Area_B / running_seconds;						// utilization of server
	float Q = Area_Q / running_seconds;						// average number of responses in queue
	float W = d + S;										// average time in system
	float X = Counter_departure / running_seconds;			// throughput


}



void arrival(float Sim_time, User user) {
	Counter_arrival++;
	if (server.status_query() == 1) {					// server is busy
		if (n_queue > queue_size)
			std::cout << "!!! Caution: Exceed the queue size setting!!!" << "\t" << n_queue << std::endl;
		w_queue.push(user);
		n_queue++;
	}
	else {												// server is idle
		e_queue.push(EventQueue(Sim_time + process_time.getRandom(), "departure", user.id()));		// create a departure event
		D += 0;
		n_delay++;
		server.set_status(1);
	}
}



void departure(float Sim_time, User departuring_user) {
	Counter_departure++;
	Counter_served++;
	if (w_queue.empty()) {									// waitting queue is empty 
		std::cout << "Caution: Queue Empty!!!";
		server.set_status(0);
	}
	else {												// waitting queue isn't empty
		User served_user = w_queue.front();				// get the first user in the queue
		w_queue.pop();
		e_queue.push(EventQueue(Sim_time + process_time.getRandom(), "departure", served_user.id()));		// create a departure event
		n_queue--;
		D += (Sim_time - served_user.query());
		n_delay++;
		departuring_user.think(think_time, Sim_time);
		e_queue.push(EventQueue(departuring_user.query(), "arrival", departuring_user.id()));				// create the arrival event for departuring user
	}
}
