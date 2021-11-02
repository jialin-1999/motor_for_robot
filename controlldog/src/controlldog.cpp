// #include <ros/ros.h>
// #include <std_msgs/Float64MultiArray.h>
// #include <std_msgs/Float64.h>

// #include <sys/types.h>
// #include <sys/socket.h>
// #include <stdio.h>
// #include <netinet/in.h>
// #include <arpa/inet.h>
// #include <unistd.h>
// #include <string.h>
// #include <stdlib.h>
// #include <fcntl.h>
// #include <sys/shm.h>

// #define BUFFER_SIZE 16     // two double variables


// struct dog_msg
// {
// 	double angle;
// 	double speed;
// };

// double speed = 0.0;
// double angle = 0.0;

// void command_callback(const std_msgs::Float64MultiArray &msg)
// {
// 	speed = msg.data[0];
// }

// void angle_callback(const std_msgs::Float64 &msg)
// {
// 	angle = msg.data;
// }

// int main(int argc, char * argv[])
// {
// 	ros::init(argc, argv, "dog_control");
// 	ros::NodeHandle nh;
// 	ros::Subscriber sub_command = nh.subscribe("/dog_control", 2, &command_callback);
// 	ros::Subscriber sub_angle = nh.subscribe("/last_angle", 2, &angle_callback);
// 	// create a client socket cocket_client; params: ipv4\SOCK_STREAM\IPPROTO_TCP
// 	int socket_client = socket(AF_INET,SOCK_STREAM, 0);

// 	if( socket_client == -1){
// 		printf("create socket error: %s(errno: %d)\n", strerror(errno),errno);
// 		exit(0);
// 	}else{
// 		printf("create socket well and wait for cennecting! \n\n");
// 	}

	
// 	// set socket address
// 	struct sockaddr_in  servaddr;
// 	std::string ip = "192.168.1.20";
// 	int port = 1986;
// 	memset(&servaddr, 0, sizeof(servaddr));
// 	servaddr.sin_family = AF_INET;
// 	servaddr.sin_port = htons(port);
// 	servaddr.sin_addr.s_addr = inet_addr(ip.c_str());

// 	if( inet_pton(AF_INET, ip.c_str(), &servaddr.sin_addr) <= 0){
// 		printf("inet_pton error for %s\n", ip.c_str());
// 		exit(0);
// 	}

// 	// connect server; succuss return 0, defeat return -1
// 	if(connect(socket_client, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
// 	{
// 		printf("connect sever error: %s(errno: %d)\n", strerror(errno),errno);;
// 		exit(1);
// 	}

// 	printf("Connect sever OK! \n");

// 	ros::Rate loop_rate(10);
// 	while(ros::ok()){
// 		ros::spinOnce();

// 		dog_msg send_msg;
// 		send_msg.angle=angle;
// 		speed = 0.5;
// 		send_msg.speed= speed;
// 		if(send(socket_client, &send_msg, 16, 0) < 0){
// 			printf("send msg error: %s (errno %d)\n", strerror(errno), errno);
// 			continue;
// 		}
// 		else{
// 			printf("send: speed = %lf   angle = %lf \n", speed, angle);
// 		}
// 		// memset(sendbuf, 0, sizeof(sendbuf));
// 		loop_rate.sleep();
// 	}

// 	close(socket_client);
// 	return 0;
// }
#include <ros/ros.h>
#include <std_msgs/Float64MultiArray.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/shm.h>

#define BUFFER_SIZE 16     // two double variables


struct dog_msg
{
	double angle;
	double speed;
};

double speed = 0.0;
double angle = 0.0;

void command_callback(const std_msgs::Float64MultiArray &msg)
{
	speed = msg.data[0];
	angle = msg.data[1];
}



int main(int argc, char * argv[])
{
	ros::init(argc, argv, "dog_control");
	ros::NodeHandle nh;
	ros::Subscriber sub_command = nh.subscribe("/dog_control", 2, &command_callback);
	//ros::Subscriber sub_command = nh.subscribe("/dog_control2", 2, &command_callback);
	// create a client socket cocket_client; params: ipv4\SOCK_STREAM\IPPROTO_TCP
	int socket_client = socket(AF_INET,SOCK_STREAM, 0);

	if( socket_client == -1){
		printf("create socket error: %s(errno: %d)\n", strerror(errno),errno);
		exit(0);
	}else{
		printf("create socket well and wait for cennecting! \n\n");
	}

	
	// set socket address
	struct sockaddr_in  servaddr;
	std::string ip = "192.168.1.20";
	int port = 1986;
	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(port);
	servaddr.sin_addr.s_addr = inet_addr(ip.c_str());

	if( inet_pton(AF_INET, ip.c_str(), &servaddr.sin_addr) <= 0){
		printf("inet_pton error for %s\n", ip.c_str());
		exit(0);
	}

	// connect server; succuss return 0, defeat return -1
	if(connect(socket_client, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
	{
		printf("connect sever error: %s(errno: %d)\n", strerror(errno),errno);;
		exit(1);
	}

	printf("Connect sever OK! \n");

	ros::Rate loop_rate(10);
	while(ros::ok()){
		ros::spinOnce();

		speed = 0.5;
		dog_msg send_msg;
		send_msg.angle=angle;
		send_msg.speed=speed;
		if(send(socket_client, &send_msg, 16, 0) < 0){
			printf("send msg error: %s (errno %d)\n", strerror(errno), errno);
			continue;
		}
		else{
			printf("send: speed = %lf   angle = %lf \n", speed, angle);
		}
		// memset(sendbuf, 0, sizeof(sendbuf));
		loop_rate.sleep();
	}

	close(socket_client);
	return 0;
}
