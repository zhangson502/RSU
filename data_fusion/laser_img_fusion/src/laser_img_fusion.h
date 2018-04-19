#ifndef LASER_IMG_FUSION_H
#define  LASER_IMG_FUSION_H
/**
*@Author:AppleZhang(TEL:+86-18810577546)
×@Version:1.0.0
*@Date:2018-3-13
*@License:BSD
*@Note: Functions for Image & Laser fusion
*@Note: ....此处添加更详细说明....
*/
#include <boost/thread.hpp>
#include <image_transport/image_transport.h>
#include <opencv2/highgui/highgui.hpp>
#include <cv_bridge/cv_bridge.h>
#include "ros/ros.h"
#include "std_msgs/String.h"
#include "sensor_msgs/LaserScan.h"
#include "rsu_msgs/HeartBeat.h"
namespace fusion{
class L_I_handler{

private:
	sensor_msgs::LaserScan scan;
	sensor_msgs::Image img;
	ros::Subscriber img_sub;
	ros::Subscriber laser_sub;
	ros::Subscriber clk_Recall;
public:
	L_I_handler()
	{
		img_sub=n.subscribe<sensor_msgs::Image>("/cam1/image",1,&L_I_handler::img_Callback,this);
		laser_sub=n.subscribe<sensor_msgs::LaserScan>("rsu_laser",1,&L_I_handler::laser_Callback,this);
		clk_Recall=n.subscribe<rsu_msgs::HeartBeat>("CORE_clk", 1,&L_I_handler::Clk_Callback,this);
	}
	sensor_msgs::Image* ModifyImg(sensor_msgs::LaserScan *scan,sensor_msgs::Image *img);
	sensor_msgs::LaserScan* ModifyLaser(sensor_msgs::LaserScan *scan,sensor_msgs::Image *img);
	void img_Callback(const sensor_msgs::Image::ConstPtr& msg);
	void laser_Callback(const sensor_msgs::LaserScan::ConstPtr& msg);
	void Clk_Callback(const rsu_msgs::HeartBeat::ConstPtr& msg);
	ros::NodeHandle n;
};
}
#endif
