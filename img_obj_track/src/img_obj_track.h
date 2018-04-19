#ifndef OBJ_TRACK
#define OBJ_TRACK
/**
*@Author:AppleZhang(TEL:+86-18810577546)
×@Version:1.0.0
*@Date:2018-3-13
*@License:BSD
*@Note: Functions for Image processing (Vehicle Identification)
*@Note: ....此处添加更详细说明....
*/
#include "ros/ros.h"
#include "std_msgs/String.h"
#include "rsu_msgs/HeartBeat.h"
namespace IMG_Identifier{

class Obj_Track
{
private:

	rsu_msgs::HeartBeat beep_Data;			//Unique beep_ID
	std::ofstream *log;					//Log Components by File IO
	ros::Subscriber clk_Recall;
public:
	Obj_Track()
	{
		/*@Note:AppleZhang: Parameters Initializer Here*/
		 n.param<int>("Core_Hz",beepRate,10);
		/*@Note:AppleZhang: Add ROS Publishers here*/
		/*@Note:AppleZhang: Add ROS Subscribers here*/
		clk_Recall=n.subscribe<rsu_msgs::HeartBeat>("CORE_clk", 1,&Obj_Track::Clk_Callback,this);
		/*@Note:AppleZhang: Utilities*/
	}
	void Clk_Callback(const rsu_msgs::HeartBeat::ConstPtr& msg);
	ros::NodeHandle n;
	int beepRate;						//Heartbeat interval
};

}
#endif
