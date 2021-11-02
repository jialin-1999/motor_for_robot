/*
 * The send value is positive, digital servo turn left;
 * Control precision is 1 degree,
 * resolution"3" : 672*376; hfov: 88 vfov:56
 * resolution"2" : 1280*720; hfov: 85 vfov:54
*/

#include "ros/ros.h"
#include "std_msgs/Int16MultiArray.h"
#include "std_msgs/Float64MultiArray.h"
#include "std_msgs/Float64.h"

#include <iostream>
#include <fstream>

#define Pi 3.14159

using namespace std;
using std::cout;
using std::endl;

ofstream outfile;

// const int deg_HOFV = 88;    // x degree Horizontal Field Of View of the camera;
// const int h_pixel = 672;
// const int deg_HOFV = 85;           // x degree Horizontal Field Of View of the camera;
// const int h_pixel = 1280;          // resolution (pixel)
const int deg_HOFV = 72;           // x degree Horizontal Field Of View of the camera;
const int h_pixel = 1080;          // resolution (pixel)
const float angle_resolution = 2;  // angle_resolution ()
const float max_angle = 60.0;      // max angle adjust (-60, +60)

const float dog_angle_rate = 5.0;  // angle adjust speed(unit: degree)


std_msgs::Int16MultiArray target_position;

// [0] -> position kcf_dsst(pixel);
// [1] -> depth (depth of target, unit: cm)
// [2] -> is tracking? (if yes == 1) ;
// [3] -> is reinit? (if yes == 1)
// [4] -> is crowded?

bool has_msg = false;

void positionCallback(const std_msgs::Int16MultiArray& msg)
{
	target_position = msg;
	has_msg = true;
}

inline float degree2radian(float angle){ return angle * Pi / 180;}
inline float radian2degree(float radian){ return radian * 180 / Pi;}

int main(int argc, char** argv)
{
	ros::init(argc, argv, "motor");
	ros::NodeHandle nh;
	ros::Subscriber receive_target_position = nh.subscribe("/kcf_ssd/target_position", 1, &positionCallback);
	ros::Publisher motor_control = nh.advertise<std_msgs::Float64>("/dynamixel_mx28/command",1);
	ros::Publisher dog_control = nh.advertise<std_msgs::Float64MultiArray>("/dog_control", 1);

	double dog_speed = 0.0;
	double dog_angle = 0.0;

	int safe_pixel = 80;    // no adjust whthin [h_pixel/2 - safe_pixel, h_pixel/2 + safe_pixel] pixels
	int motor_angle = 0;

	std_msgs::Float64 motor_msg;
	std_msgs::Float64MultiArray dog_msg;   //2 double data, [0]: speed, [1]: angle
	dog_msg.data.resize(2);

	bool is_motor_init = true;

	int no_msg_frame = 0;

	float ix = 0.02;
	
	int x_track  = 0;
	int distance = 0;
	int is_track = 0;
	int is_reinit = 0;
	int is_crowded = 0;
	
	int loop_rate_dog = 3;
	int nframe_dog = 1;
	unsigned int nframe = 0;

	ros::Rate loop_rate(20);
	while(ros::ok()){
		++nframe;
		printf("Frame # %d \n" , nframe);

		// whether has msg from tracker
		has_msg = false;

		// message callback, only when topic has a new messge that the callback function will be called
		ros::spinOnce();

		if(!has_msg){
			++no_msg_frame;
		}

		// reset motor
		if(no_msg_frame > 100){
			motor_msg.data = 0.0;
			motor_control.publish(motor_msg);
			nframe = 0;
			printf("Track is OFF and reset the motor! \n\n");
		}

		
		if(has_msg){
			x_track  = target_position.data[0];
			distance = target_position.data[1];
			is_track = target_position.data[2];
			is_reinit =  target_position.data[3];
			is_crowded = target_position.data[4];

			no_msg_frame = 0;

			if(is_track == 1 && is_reinit != 1 && is_crowded != 1){
				int diffPixel = x_track - h_pixel/2;
				if(diffPixel > safe_pixel){
					motor_angle = motor_angle - angle_resolution;
					if(motor_angle < -max_angle)
						motor_angle = -max_angle;
				}
				if(diffPixel < -safe_pixel){
					motor_angle = motor_angle + angle_resolution;
					if(motor_angle > max_angle)
						motor_angle = max_angle;
				}
				motor_msg.data = degree2radian(motor_angle);    // control servo
				motor_control.publish(motor_msg); 	
				
// 				motor_control.publish(motor_msg); // send rad command to motor
				printf("Motor angle = %4.2f \n" , motor_msg.data);
			}

			// distance -> speed
			if(distance <= 200)
				dog_speed = 0.5;
			else
				dog_speed = -0.5;
			
			// Motor_angle -> dog_angle
			dog_angle = 0.7*motor_angle;
            
			dog_msg.data[0] = dog_speed;
			dog_msg.data[1] = degree2radian(dog_angle);

			printf("dog speed = %4.2lf \n" , dog_msg.data[0]);
			printf("dog angle = %4.2lf \n" , dog_msg.data[1]);		
            			
			// Control the command rate 
			nframe_dog++;
			if(nframe_dog == loop_rate_dog){
				nframe_dog = 1;
				dog_control.publish(dog_msg);
			}
		}

		loop_rate.sleep();
	}

	return 0;
}
