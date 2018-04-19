#include "state_monitor.h"
using namespace state_monitor;
bool state_monitor::State_Srv_callback(std_srvs::state_call::Request &req, std_srvs::state_call::Response &res){
	//res.ActiveNodes=req.indexer;
	// @Note by AppleZhang: Service code here
	ROS_INFO("RequestFrom:%d");
	return true;
}
Monitor::Monitor(){

}
bool Monitor::Init(){
	monitorSrv=n_m.advertiseService("global_state",State_Srv_callback);
	ROS_INFO("Enviroment Monitor Established");
	return true;
}
