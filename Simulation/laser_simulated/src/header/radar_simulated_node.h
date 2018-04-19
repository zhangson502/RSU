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
#include <unistd.h>
#include <pthread.h>
//Public Defination
#define QUEUE 3000		//控制器数量
#define PI 3.141592653
#define byte unsigned char
//交互协议相关的宏定义

//缓存全局变量
double v,vth,th,v_r,v_l;	//速度
double x=0,y=0;	//位置
double tf_odom,tf_map;	//TF坐标合集
double pos_accu;		//位置准度
double obtacle_dist;	//障碍距离
double net_relay;long netblock_length;	//传输延时
int DataExchangeFlag=0;
const double STANDARD_INIT_COV[36]={0.25, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
		0.25, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
		0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.06853891945200942};
const unsigned char STD_HEADER[4]={0xff,0x00,0xff,0x00};
class net_ui
{
public:
	static unsigned char car_Num=4;
	/*网络相关函数*/
	//Reload
	int Upload(int len,unsigned char cmdID,unsigned char*data);
	////////////////////////
	int  Download(unsigned char *data,unsigned char &cmd);
	void Connect();
	int login();
	double Getping();
	/*下载上传网络相关指令解析*/
	void PushData();
	/*数据间隔存储*/
	void Param_Wrt();
	void Param_Read();
	/*回调函数*/
	void spd_Callback(const geometry_msgs::Twist::ConstPtr& msg);
	void  amcl_Callback(const geometry_msgs::PoseWithCovarianceStamped::ConstPtr& msg);
	void clk_Callback(const std_msgs::String::ConstPtr& msg);
	/*析构函数*/
	~net_ui();
	net_ui();
	int Net_State;
	ros::NodeHandle n;
private:
//Public
	ros::Subscriber std_clk;
//Navigation Oriented
	ros::Publisher target_pub;
	ros::Publisher path_pub;
	ros::Publisher spd_pub;
	ros::Subscriber spd_info;
	ros::Publisher m_pub;
	ros::Publisher  floor_pub ;
	geometry_msgs::Twist vel_ctrl;
//Localization Oriented
	unsigned char floor_id;
	geometry_msgs::PoseWithCovarianceStamped amcl_pose;
	ros::Subscriber amcl_sub;
//TF Oriented
	tf::TransformListener AGV_transform_listener;
	tf::StampedTransform AGV_transform;
	nav_msgs::Path my_path;
	//network
		int sock_ser;
		int new_server_socket;
		struct sockaddr_in servaddr;
    	socklen_t length;
		int Heart_Pulse;
    //Position Initialization & File Executation
    	int header_for_initialpose;
    	geometry_msgs::PoseWithCovarianceStamped initial_pose;
    	ros::Publisher init;
	//Maps
    //buffer
};

