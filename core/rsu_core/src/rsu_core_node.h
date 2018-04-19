#ifndef RSU_CORE
#define RSU_CORE
/**
*@Author:AppleZhang(TEL:+86-18810577546)
×@Version:1.0.0
*@Date:2018-3-13
*@License:BSD
*@Note: Core Node of RSU, which publish heart-beat message, Collect message and provide system Health service
*@Note: ....此处添加更详细说明....
*/
#include "ros/ros.h"
#include "std_msgs/String.h"
#include "rsu_msgs/HeartBeat.h"
#include <fstream>
#include <iostream>
#include <tf/transform_broadcaster.h>
#include <tf/transform_listener.h>
#include <nav_msgs/Odometry.h>
#include <sensor_msgs/LaserScan.h>
#include <geometry_msgs/Twist.h>
#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <unistd.h>
#include <math.h>
#include <boost/thread.hpp>
#include <actionlib_msgs/GoalID.h>
#include <rsu_msgs/radar.h>
class Rsu_Core{
public:
	Rsu_Core()
	{
		/*@Note:AppleZhang: Parameters Initializer Here*/
		 n.param<int>("Core_Hz",beepRate,10);
		/*@Note:AppleZhang: Add ROS Publishers here*/
		clk_Pub = n.advertise<rsu_msgs::HeartBeat>("CORE_clk",5);
		/*@Note:AppleZhang: Add ROS Subscribers here*/

		clk_Recall=n.subscribe<rsu_msgs::HeartBeat>("CORE_clk", 1,&Rsu_Core::Clk_Callback,this);
		/*@Note:AppleZhang: Utilities*/
	}
	void publish_HB();
	void Clk_Callback(const rsu_msgs::HeartBeat::ConstPtr& msg);
	ros::NodeHandle n;
	int beepRate;						//Heartbeat interval
	rsu_msgs::HeartBeat beep_Data;			//Unique beep_ID
	std::ofstream *log;					//Log Components by File IO
	ros::Publisher clk_Pub;
	ros::Subscriber clk_Recall;
};
#endif
