#include "rsu_core_node.h"

void Rsu_Core::publish_HB()
{
	clk_Pub.publish(beep_Data);
	beep_Data.header.stamp=ros::Time::now();
	beep_Data.data++;
	//beep_Data.data++;
}
void Rsu_Core::Clk_Callback(const rsu_msgs::HeartBeat::ConstPtr& msg)
{
	static unsigned long t=ros::Time::now().toNSec();
	printf("Core&&Clock:ID=%d @ %d us interval\n",
			msg->data,
			(ros::Time::now().toNSec()-t)/1000);
	t=msg->header.stamp.toNSec();
}
int main(int argc, char **argv)
{
	ros::init(argc, argv, "core");
	Rsu_Core rsu_Core;
	ros::AsyncSpinner spinner(2);		//2 independent threads for data processing
	spinner.start();
	//state_monitor::Monitor monitor;
	//monitor.Init();
	ros::Rate r(rsu_Core.beepRate);
	while (ros::ok())
	{
		//Endless loop
		//Can be broken by terminator
		rsu_Core.publish_HB();
		r.sleep();
	}
}
