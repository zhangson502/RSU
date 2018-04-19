#ifndef RADAR_SIM_NODE
#define RADAR_SIM_NODE

#include <arpa/inet.h>
#include <boost/thread.hpp>
#include <fcntl.h>
#include <geometry_msgs/Twist.h>
#include <geometry_msgs/Vector3.h>
#include <geometry_msgs/PoseStamped.h>
#include <geometry_msgs/PoseWithCovarianceStamped.h>
#include <nav_msgs/OccupancyGrid.h>
#include <linux/input.h>
#include <math.h>
#include <nav_msgs/Path.h>
#include <netinet/in.h>
#include <pthread.h>
#include <ros/duration.h>
#include <ros/node_handle.h>
#include <ros/publisher.h>
#include <ros/rate.h>
#include <ros/spinner.h>
#include <ros/subscriber.h>
#include <ros/time.h>
#include <std_msgs/Float64.h>
#include <std_msgs/String.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/shm.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <tf/exceptions.h>
#include <tf/LinearMath/Quaternion.h>
#include <tf/LinearMath/Transform.h>
#include <tf/LinearMath/Vector3.h>
#include <tf/transform_datatypes.h>
#include <tf/transform_broadcaster.h>
#include <tf/transform_listener.h>
#include <visualization_msgs/Marker.h>
#include <rsu_msgs/HeartBeat.h>
#include <unistd.h>
#include <pthread.h>
#include "vehicle.h"
//Public Defination
#define PI 3.141592653
#define byte unsigned char
//交互协议相关的宏定义
int car_Num=4;
vehicle car[5];
ros::Publisher a;
//缓存全局变量
double v,vth,th,v_r,v_l;	//速度
double x=0,y=0;	//位置
double tf_odom,tf_map;	//TF坐标合集
double pos_accu;		//位置准度
double obtacle_dist;	//障碍距离
double net_relay;long netblock_length;	//传输延时
int DataExchangeFlag=0;
const unsigned char STD_HEADER[4]={0xCA,0xCB,0xCC,0xCD};
void Clk_Callback(const rsu_msgs::HeartBeat::ConstPtr& msg);
int64_t Tolong(uint16_t ID, float length,float SpeedY,float Speed,float Ypos,float Xpos);
class net_ui
{
public:

	/*网络相关函数*/
	//Reload

	/*下载上传网络相关指令解析*/

	/*数据间隔存储*/

	/*析构函数*/
	~net_ui();
	net_ui();
	ros::NodeHandle n;
	void Connect();
	short Longto(long long data, int low, int higth);
	void PushData(vehicle *car);
//Navigation Oriented

//Localization Oriented
	unsigned char floor_id;
	geometry_msgs::PoseWithCovarianceStamped amcl_pose;
	ros::Subscriber amcl_sub;
//TF Oriented
	tf::TransformListener AGV_transform_listener;
	tf::StampedTransform AGV_transform;
	//network
	int new_server_socket;
	int sock_ser;
	struct sockaddr_in servaddr;
	socklen_t length;
	int Net_State;
    //Position Initialization & File Executation

	//Maps
    //buffer
    	char *i_buffer;
    //ID

};
#endif
