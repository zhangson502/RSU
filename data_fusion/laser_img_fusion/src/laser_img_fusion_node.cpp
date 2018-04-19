#include "laser_img_fusion.h"
using namespace fusion;
sensor_msgs::Image* L_I_handler::ModifyImg(sensor_msgs::LaserScan *scan,sensor_msgs::Image *img){
	//Fusion && Return Image
	return img;
}
sensor_msgs::LaserScan* L_I_handler::ModifyLaser(sensor_msgs::LaserScan *scan,sensor_msgs::Image *img){
	//Fusion && Return laser-scan
	return scan;
}
void L_I_handler::img_Callback(const sensor_msgs::Image::ConstPtr& msg){
	//Store Image message which has been received.
	img.header=msg->header;
	printf("Fusion Node Receive IMG@ %d\n",img.header.seq);
}
void L_I_handler::laser_Callback(const sensor_msgs::LaserScan::ConstPtr& msg){
	//Store Laser message which has been received.
	scan.header=msg->header;

}
void L_I_handler::Clk_Callback(const rsu_msgs::HeartBeat::ConstPtr& msg)
{
	static unsigned long t=ros::Time::now().toNSec();
//	printf("Img_Identifier&&Clock:ID=%d @ %d us interval\n",
//			msg->data,
//			(ros::Time::now().toNSec()-t)/1000);
//	t=msg->header.stamp.toNSec();
}
int main(int argc, char **argv)
{
	ros::init(argc, argv, "fusion");
	ros::NodeHandle n;
	L_I_handler fusion_handler;
	printf("Fusion node Initialized!\n");
	ros::AsyncSpinner spinner(5);		//6 independent threads for data processing
	spinner.start();
	while (ros::ok())
	{
	}
}
