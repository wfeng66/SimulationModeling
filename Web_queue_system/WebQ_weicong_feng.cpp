#include <iostream>
#include <cmath>
#include <queue>
#include <vector>
#include "WebQ_weicong_feng.h"


int running_times = 30;
float think_low = 18.0;
float think_high = 22.0;
float process_low = 0.2;
float process_high = 0.3;
int hour_running = 3;
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
float sum_d = 0;
float sum_S = 0;
float sum_U = 0;
float sum_q = 0;
float sum_W = 0;
float sum_X = 0;
float avg_d = 0;
float avg_S = 0;
float avg_U = 0;
float avg_q = 0;
float avg_W = 0;
float avg_X = 0;
float sd_d = 0;
float sd_S = 0;
float sd_U = 0;
float sd_q = 0;
float sd_W = 0;
float sd_X = 0;
int running_seconds = 3600 * hour_running;

std::priority_queue<EventQueue> e_queue;			// event queue
std::queue<User> w_queue;							// waiting queue
// initialize server
Server server;
UniformRandom<float> think_time(think_low, think_high);
UniformRandom<float> process_time(process_low, process_high);

void arrival(float Sim_time, User user);
void departure(float Sim_time, User departuring_user);
WebQ::MyStat<float> one_shot();


int main() {
	WebQ::MyStat<float>* stat_lst;
	stat_lst = new MyStat<float>[running_times];

	for (int i = 0; i < running_times; i++) {
		Counter_arrival = 0;
		Counter_departure = 0;
		Counter_served = 0;
		Sim_time = 0;
		n_queue = 0;								// the number of queries in the queue waiting
		t_last_even = 0;							// time of the last even
		n_delay = 0;								// total number of delay
		D = 0.0;									// total delay time
		Area_Q = 0;
		Area_B = 0;
		server.set_status(0);
		// clear the event queue and waiting queue
		while (!e_queue.empty()) {
			e_queue.pop();
		}
		while (!w_queue.empty()) {
			w_queue.pop();
		}
		stat_lst[i] = one_shot();
	}

	// produce report
	std::cout << "Number of Active Users: " << Active_users << "\tRunning Hours: " << hour_running << "\tRunning Times: " << running_times << std::endl;
	std::cout << "Average Delay\t\t" << "Average Serving Time\t" << "\tUtilization\t" << "Average # in Queue\t" << "Average Time in System\t" << "X" << std::endl;

	for (int j = 0; j < running_times; j++) {
		sum_d += stat_lst[j].d;
		sum_S += stat_lst[j].S;
		sum_U += stat_lst[j].U;
		sum_q += stat_lst[j].q;
		sum_W += stat_lst[j].W;
		sum_X += stat_lst[j].X;
	}
	// calculate the mean
	avg_d = sum_d / running_times;
	avg_S = sum_S / running_times;
	avg_U = sum_U / running_times;
	avg_q = sum_q / running_times;
	avg_W = sum_W / running_times;
	avg_X = sum_X / running_times;

	
	// calculate the standard deviation
	for (int k = 0; k < running_times; k++) sd_d += pow(stat_lst[k].d - avg_d, 2);
	for (int k = 0; k < running_times; k++) sd_S += pow(stat_lst[k].S - avg_S, 2);
	for (int k = 0; k < running_times; k++) sd_U += pow(stat_lst[k].U - avg_U, 2);
	for (int k = 0; k < running_times; k++) sd_q += pow(stat_lst[k].q - avg_q, 2);
	for (int k = 0; k < running_times; k++) sd_W += pow(stat_lst[k].W - avg_W, 2);
	for (int k = 0; k < running_times; k++) sd_X += pow(stat_lst[k].X - avg_X, 2);

	//std::cout << sd_d << "\t" << sd_S << "\t" << sd_U << "\t" << sd_q << "\t" << sd_W << "\t" << sd_X;
	std::cout << std::endl;
	std::cout << avg_d << " : " << sqrt(sd_d / (running_times - 1)) << "\t" << avg_S << " : " << sqrt(sd_S / (running_times - 1)) << "\t";
	std::cout << avg_U << " : " << sqrt(sd_U / (running_times - 1)) << "\t" << avg_q << " : " << sqrt(sd_q / (running_times - 1)) << "\t";
	std::cout << avg_W << " : " << sqrt(sd_W / (running_times - 1)) << "\t" << avg_X << " : " << sqrt(sd_X / (running_times - 1)) << std::endl;
	
}


WebQ::MyStat<float> one_shot(){
	// metrics: X, S, U, W
	MyStat<float> q_stat;


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


	// main loop
	while (Sim_time < running_seconds) {
		// update Q(t) and B(t)
		Area_Q += n_queue * (Sim_time - t_last_even);
		Area_B += server.status_query() * (Sim_time - t_last_even);
		//std::cout << n_queue << "\t\t\t";
		if (event.type() == "arrival") {
			arrival(Sim_time, users[event.id()]);
		}
		else {
			departure(Sim_time, users[event.id()]);
		}

		t_last_even = Sim_time;								// correct place???
		// advance time
		event = e_queue.top();
		e_queue.pop();
		Sim_time = event.time();

	}

	// produce report data
	q_stat.d = D / Counter_departure;						// average delay
	q_stat.S = Area_B / Counter_served;						// average serving time
	q_stat.U = Area_B / running_seconds;					// utilization of server
	q_stat.q = Area_Q / running_seconds;					// average number of responses in queue
	q_stat.W = q_stat.d + q_stat.S;							// average time in system
	q_stat.X = Counter_departure / running_seconds;			// throughput


	return q_stat;
}



void arrival(float Sim_time, User user) {
	Counter_arrival++;
	if (server.status_query() == 1) {						// server is busy
		if (n_queue > queue_size)
			std::cout << "!!! Caution: Exceed the queue size setting!!!" << "\t" << n_queue << std::endl;
		w_queue.push(user);
		n_queue++;
	}
	else {													// server is idle
		e_queue.push(EventQueue(Sim_time + process_time.getRandom(), "departure", user.id()));				// create a departure event
		D += 0;
		n_delay++;
		server.set_status(1);
	}
}



void departure(float Sim_time, User departuring_user) {
	Counter_departure++;
	Counter_served++;
	departuring_user.think(think_time, Sim_time);
	e_queue.push(EventQueue(departuring_user.query(), "arrival", departuring_user.id()));					// create the arrival event for departuring user
	if (w_queue.empty()) {									// waitting queue is empty 
		//std::cout << "Caution: Queue Empty!!!";
		server.set_status(0);
	}
	else {													// waitting queue isn't empty
		User served_user = w_queue.front();					// get the first user in the queue
		w_queue.pop();
		e_queue.push(EventQueue(Sim_time + process_time.getRandom(), "departure", served_user.id()));		// create a departure event
		n_queue--;
		D += (Sim_time - served_user.query());
		n_delay++;
	}
}


