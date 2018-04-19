#ifndef MONITOR_H
#define MONITOR_H
/*
 * @Version:1.0.0
 * @Author:ApleZhang
 * @Date:20180304
 * @Note:RSU的自身状态评估模块，用于向外输出调试信息以及输出日志信息。
 */
#include "ros/ros.h"
#include "std_msgs/String.h"
#include "rsu_core_node.h"
namespace state_monitor{
bool State_Srv_callback(std_srvs::state_call::Request &req, std_srvs::state_call::Response &res);

class Monitor{
private:
	unsigned int autoNum;	//与当前RSU连接智能车的数量
	unsigned int baudRate;	//当前主机与外部及其通信的总带宽
	unsigned int relay;		//最高网络延迟
	ros::ServiceServer monitorSrv;
public:
	Monitor();
	ros::NodeHandle n_m;
	bool State_Log();
	bool Init();

};
}

#endif
