#include "img_obj_track.h"
using namespace IMG_Identifier;
void Obj_Track::Clk_Callback(const rsu_msgs::HeartBeat::ConstPtr& msg)
{
	static unsigned long t=ros::Time::now().toNSec();
	printf("Img_Identifier&&Clock:ID=%d @ %d us interval\n",
			msg->data,
			(ros::Time::now().toNSec()-t)/1000);
	t=msg->header.stamp.toNSec();
}
int main(int argc, char **argv)
{
	ros::init(argc, argv, "tracker");
	ros::AsyncSpinner spinner(4);		//4 threads for Object Tract 1:Clock response 2:Algorithm 3:Data input 4:Utils
	spinner.start();
	Obj_Track tracker;
	while (ros::ok())
	{
	}
}
