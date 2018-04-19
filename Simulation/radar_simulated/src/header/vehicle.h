#include "radar_simulated_node.h"
class vehicle
{
public:
	float Xpos;//Vehicle Center, X coordinate
	float Ypos;  //Vehicle Center, Y coordinate
	float length;
	float width;
	unsigned int ID;
	float Speed;
	float SpeedY;
	visualization_msgs::Marker marker;
	vehicle(int id,int lane,float speed,float l,float x)
	{
		ID=id;
		Xpos=0;
		Ypos=lane*3.0+0.2;
		Speed=speed;
		length=l;
		Xpos=x;
		SpeedY=0;
		if(id==4)
		{
			marker.header.frame_id = "base_link";
			marker.header.stamp = ros::Time();
			marker.ns = "my_namespace";
			marker.id = ID;
			marker.type = visualization_msgs::Marker::CUBE;
			marker.action = visualization_msgs::Marker::ADD;
			marker.pose.position.x = 50;
			marker.pose.position.y = 10;
			marker.pose.position.z = 0.2;
			marker.pose.orientation.x = 0.0;
			marker.pose.orientation.y = 0.0;
			marker.pose.orientation.z = 0.0;
			marker.pose.orientation.w = 1.0;
			marker.scale.x = 80;
			marker.scale.y = 20;
			marker.scale.z = 0.2;
			marker.color.a = 0.5; // Don't forget to set the alpha!
			marker.color.r = 0;
			marker.color.g = 1;
			marker.color.b = 0;
		}
	}
	vehicle()
	{
		ID=0;
		Xpos=0;
		Ypos=0;
	}
	void Move(float speedx,float speedy)
	{
		Xpos+=speedx;
		Ypos+=speedy;
	}
	void Move()
	{
		Xpos+=Speed/10;
		if(Xpos>100.0) Xpos=0.0;
	}
	void PushTF()
	{
		char i[4]={'C','a','r','0'};
		i[3]=ID+'0';
		static tf::TransformBroadcaster br;
		tf::Transform transform;
		transform.setOrigin(tf::Vector3(Xpos, Ypos, 0.0) );
		tf::Quaternion q;
		q.setRPY(0, 0, 0);
		transform.setRotation(q);
		br.sendTransform(tf::StampedTransform(transform, ros::Time::now(), "map", i));
		marker.header.frame_id = "base_link";
		marker.header.stamp = ros::Time();
		marker.ns = "my_namespace";
		marker.id = ID;
		marker.type = visualization_msgs::Marker::CUBE;
		marker.action = visualization_msgs::Marker::ADD;
		marker.pose.position.x = Xpos;
		marker.pose.position.y = Ypos-width/2;
		marker.pose.position.z = 0.75;
		marker.pose.orientation.x = 0.0;
		marker.pose.orientation.y = 0.0;
		marker.pose.orientation.z = 0.0;
		marker.pose.orientation.w = 1.0;
		marker.scale.x = length;
		marker.scale.y = 1.5;
		marker.scale.z = 1.2;
		marker.color.a = 1.0; // Don't forget to set the alpha!
		marker.color.r = ID*0.3;
		marker.color.g = 1.0-ID*0.3;
		marker.color.b = 0.0;
	}
};
