#include "header/ui.h"

/**************************************CLASS DEFINATION FOR NETWORK************************************
	机器人的网络通信接口（interface）		解析下载信息
*/
net_ui::net_ui()
{
			
			//Declaration for all Publishers(From Remote Server's Command)
			//Maunal Control
			spd_pub = n.advertise<geometry_msgs::Twist>("manual_vel", 1);
			//Target & Adviced path
			target_pub = n.advertise<geometry_msgs::PoseStamped>("agv_target", 1);
			floor_pub = n.advertise<std_msgs::String>("floor_id", 1);
			path_pub = n.advertise<nav_msgs::Path>("agv_path", 1);
			//Position Initialization
			init=n.advertise<geometry_msgs::PoseWithCovarianceStamped>("initialpose", 1);
			//Nav_Mode
			m_pub = n.advertise<std_msgs::String>("User_Nav_Mode", 1);
			//Declaration for all Subscriers(For Calculation & Data upload)
			amcl_sub=n.subscribe<geometry_msgs::PoseWithCovarianceStamped>("amcl_pose",1,&net_ui::amcl_Callback,this);
			//Speed Calculated
			spd_info = n.subscribe<geometry_msgs::Twist>("AGV_vel",1,&net_ui::spd_Callback,this);
			//Standard Clock published by agv_core
			std_clk = n.subscribe<std_msgs::String>("CORE_clk",1,&net_ui::clk_Callback,this);
			UID[0]=0x01;UID[1]=0x01;
			header_for_initialpose=1;
			Net_State=0;
			Heart_Pulse=0;
			
}
net_ui::~net_ui()
{
	close(sock_cli);

}
void net_ui::Connect()
{
	sock_cli = socket(AF_INET,SOCK_STREAM, 0);
	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(18888);
			servaddr.sin_addr.s_addr = inet_addr("192.168.33.110");
    	if (connect(sock_cli, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
    	{
			printf("Network Error!!!\n");
			return; 
    	}
}
void net_ui::Manual_Ctrl(unsigned char *data, int len)
{
	double v_tmp,vth_tmp;
	int v_int,vth_int;
	int addr_mov=0;
	unsigned char v_dir,th_dir;
		if(len!=sizeof(int)*2+sizeof(unsigned char)*2) return;
	memcpy(&v_int,data,sizeof(int));
	addr_mov+=sizeof(int);
	memcpy(&v_dir,data+addr_mov,sizeof(unsigned char));
	addr_mov+=sizeof(unsigned char);
	memcpy(&vth_int,data+addr_mov,sizeof(int));
	addr_mov+=sizeof(int);
	memcpy(&th_dir,data+addr_mov,sizeof(unsigned char));
	v_int=ntohl(v_int);
	vth_int=ntohl(vth_int);
	v_tmp=(double)(v_int)/3000;
	vth_tmp=(double)(vth_int)/2000;
	vel_ctrl.linear.x=v_dir==0?v_tmp:-v_tmp;
	vel_ctrl.angular.z=th_dir==0?vth_tmp:-vth_tmp;
 	std_msgs::String m;
	m.data="M";
	m_pub.publish(m);
	spd_pub.publish(vel_ctrl);
	Upload(len,6,data);
}
void net_ui::Floor_Download(unsigned char *data, int len)
{
	memcpy(&floor_id,data,sizeof(unsigned char));
	Floor_Init();
	Upload(len,10,data);
}
void net_ui::Floor_Init()
{
	std_msgs::String m;
	switch(floor_id)
	{
	case 0:
		m.data="0";
		 break;
	case 1:
		m.data="1";
		 break;
	case 2:
		m.data="2";
		 break;
	case 3:
		m.data="3";
		break;
	case 4:
		m.data="4";
		break;
	}
	floor_pub.publish(m);
}
void net_ui::Target_Download()
{

}
void net_ui::Pos_Download(unsigned char *data, int len)
{
	int x,y,th;
	unsigned char floor;
	memcpy(&x,data,sizeof(int));
	memcpy(&y,data+sizeof(int),sizeof(int));
	memcpy(&th,data+sizeof(int)*2,sizeof(int));
	memcpy(&floor,data+sizeof(int)*3,sizeof(unsigned char));
	if(floor!=0) floor_id=floor; 
	Floor_Init();
	ros::Rate r(0.5);
	r.sleep();
	x=ntohl(x);y=ntohl(y);th=ntohl(th);
	initial_pose.header.seq=header_for_initialpose++;
	initial_pose.header.frame_id="map";
	initial_pose.pose.pose.position.x=(double)x/1000;
	initial_pose.pose.pose.position.y=(double)y/1000;
	initial_pose.pose.pose.position.z=0;
	initial_pose.pose.pose.orientation.w=cos((double)th/2000);
	initial_pose.pose.pose.orientation.z=sin((double)th/2000);
	for(int i=0;i<36;i++) initial_pose.pose.covariance[i]=STANDARD_INIT_COV[i];
	init.publish(initial_pose);
	Upload(len,12,data);
}
void net_ui::Path_Download(unsigned char *data, int len)
{
	nav_msgs::Path path;
	path.poses.resize(len/(3*sizeof(int))+1);
	for(int i=0;i<path.poses.size()-1;i++)
	{
		unsigned int tmp_x,tmp_y,dir;
		memcpy(&tmp_x,data+i*(3*sizeof(int)),sizeof(int));
		memcpy(&tmp_y,data+i*(3*sizeof(int))+sizeof(int),sizeof(int));
		memcpy(&dir,data+i*(3*sizeof(int))+sizeof(int)*2,sizeof(int));
		path.poses[i+1].pose.position.x=(double)(ntohl(tmp_x))/1000;
		path.poses[i+1].pose.position.y=(double)(ntohl(tmp_y))/1000;
		dir=ntohl(dir);
		//printf("%d\n",dir);
		if(dir==0) {path.poses[i+1].pose.orientation.z=0;}
		if(dir==1) path.poses[i+1].pose.orientation.z=1;
		if(dir==2) path.poses[i+1].pose.orientation.z=0.5;
	}
		//Get_TF();
		path.poses[0].pose.position.x=x;
		path.poses[0].pose.position.y=y;
	path.header.frame_id="map";
	path_pub.publish(path);
 	std_msgs::String m;
	m.data="A";
	m_pub.publish(m);
	Upload(len,8,data);
}
void net_ui::Param_Read()
{
	FILE *fp_rd;
	float init_x=15,init_y=15,init_th=0;
	unsigned char floor=0;
	printf("正在初始化中20%...\n");
	fp_rd = fopen("/home/exbot/AGV/src/cfg/primitive_cfg","r+");
	if(fp_rd==NULL) {printf("警告：文件系统中丢失初始配置文件！\n");return;}
	fscanf(fp_rd,"X=%f  Y=%f  T=%f F=%c",&init_x,&init_y,&init_th,&floor_id);
	floor_id=0;
	fclose(fp_rd);
	initial_pose.header.seq=header_for_initialpose++;
	initial_pose.header.frame_id="map";
	initial_pose.pose.pose.position.x=init_x;
	initial_pose.pose.pose.position.y=init_y;
	initial_pose.pose.pose.position.z=0;
	ros::Rate r1(1);
	printf("正在初始化中60%...\n");
	r1.sleep();
	//Floor_Init();
	ros::Rate r2(0.3);
	r2.sleep();
	initial_pose.pose.pose.orientation.w=cos(init_th/2);
	initial_pose.pose.pose.orientation.z=sin(init_th/2);
	printf("读取初始位置：(%4f , %4f) ARG:%4f！\n",init_x,init_y,init_th);
	for(int i=0;i<36;i++) initial_pose.pose.covariance[i]=STANDARD_INIT_COV[i];
	init.publish(initial_pose);
}
void net_ui::Param_Wrt()
{
	static int i=0;
	i++;
	if(i>=10)
	{
		i=0;
		FILE *fp_wrt;
		fp_wrt = fopen("/home/exbot/AGV/src/cfg/primitive_cfg","w");
			//fprintf(fp_wrt,"\ec");
			fprintf(fp_wrt,"X=%5f  Y=%5f  T=%5f F=%c\n",(float)x,(float)y,(float)th,floor_id);
		fclose(fp_wrt);
	}
}
double net_ui::Getping()
{

}
int  net_ui::Download(unsigned char *data,unsigned char &cmd)
{
	unsigned char header[5];
	recv(sock_cli, header, sizeof(header),0);
	int len;
	memcpy(&len,header,sizeof(len));
	len=ntohl(len);
	memcpy(&cmd,header+sizeof(len),sizeof(cmd));
	int ll=recv(sock_cli, data, len,0);
//	if(ll!=len) return 0;
	return len;
}
int net_ui::Upload(int len,unsigned char cmdID,unsigned char *data)
{
	//Return 0 if success, 1 Fail
	//Create Header
	int htonl_len=htonl(len);
	unsigned char Header[sizeof(len)+sizeof(cmdID)];
	memcpy(Header,& htonl_len,sizeof(len));
	Header[sizeof(len)]=cmdID;
	//Data Transfer
	send(sock_cli,Header,sizeof(Header),0);
	send(sock_cli,data,len,0);
	return 0;
}

void net_ui::amcl_Callback(const geometry_msgs::PoseWithCovarianceStamped::ConstPtr& msg)
{
	amcl_pose.pose=msg->pose;
}
void net_ui::spd_Callback(const geometry_msgs::Twist::ConstPtr& msg)
{	
	v=msg->linear.x;
	vth=msg->angular.z;
}
void net_ui::clk_Callback(const std_msgs::String::ConstPtr& msg)
{	
	if(DataExchangeFlag==1)
	{
		try
		{
			AGV_transform_listener.lookupTransform("/map", "/base_link",ros::Time(0), AGV_transform);
		}
		catch (tf::TransformException ex)
		{
	      		printf("ERROR: %s",ex.what());
	      		ros::Duration(1.0).sleep();
	    	}
		x=AGV_transform.getOrigin().x();
		y=AGV_transform.getOrigin().y();
		th=AGV_transform.getRotation().getAxis().getZ()>=0?AGV_transform.getRotation().getAngle():PI*2-AGV_transform.getRotation().getAngle();
		//数据上行同步写入
		//if(Net_State) Std_Upload();
			Param_Wrt();
		printf("\ec");
		printf("当前机器人里程记录位置：%d 楼，X->%3f    Y->%3f    ARG->%3f \n",floor_id,x,y,th);
		printf("当前机器人雷达分析位置：X->%3f    Y->%3f    ARG->%3f\n",amcl_pose.pose.pose.position.x,amcl_pose.pose.pose.position.y,amcl_pose.pose.pose.orientation.w);
		printf("当前最近处障碍：%f米\n",obtacle_dist);
		printf("当前机器人速度：%fm/s\n",v);
		printf("当前机器人网络状态：%s\n",Net_State==0?"断线":"在线");
		Heart_Pulse++;
		printf("Net Delay：%d\n",Heart_Pulse);
		if(Heart_Pulse>=60) 
		{
			close(sock_cli);
			printf("\ec");
			printf("当前机器人网络状态：%s\n",DataExchangeFlag==1?"断线":"在线");
			DataExchangeFlag=0;
			Net_State=0;
			ros::Rate r(1);
			while(Heart_Pulse>=60)
			{
				Connect();
				login();
				r.sleep();
			}
		}
	}
}
int net_ui::Std_Upload()
{
	Heart_Pulse=0;
	DataExchangeFlag=1;
	unsigned char std_buf[sizeof(int)*3+sizeof(unsigned char)]={0};
	int len=0;
	//Net Transfer Data Format Converter;
   int htonl_x=htonl((int)(x*1000));	//From Meters To Centimeters
   int htonl_y=htonl((int)(y*1000));
    int thonl_th=htonl((int)(th*1000));
    //Preparation;
    memcpy(std_buf,& htonl_x,sizeof( htonl_x));
    len+=sizeof( htonl_x);
    memcpy(std_buf+len,&htonl_y,sizeof(htonl_y));
    len+=sizeof( htonl_x);
    memcpy(std_buf+len,&thonl_th,sizeof(thonl_th));
    len+=sizeof(thonl_th);
    memcpy(std_buf+len,&floor_id,sizeof(floor_id));
    len+=sizeof(floor_id);
	int i=Upload(len,4,std_buf);
	return 0;
}
int net_ui::login()
{
	Upload(sizeof(UID),1,UID);
	unsigned char dat,cmd;
	int len=Download(&dat,cmd);
	if((cmd==0x02)) {Net_State=1;return 0;}
	else return 1;
}
/*************************************++++++++++*		       MAIN  	              *************************************/
int main(int argc, char** argv)
{
	ros::init(argc, argv, "agv_ui");
	ros::AsyncSpinner spinner(5); 	//数量由消息数量而定，线程数为消息数量+1
	spinner.start();
	net_ui Resolver;
	Resolver.Connect();
	while(Resolver.login());
	Resolver.Param_Read();
	//DataExchangeFlag=1;
	while(Resolver.n.ok())
	{
		unsigned char data[2048];
		unsigned char cmd;
		int len=Resolver.Download(data,cmd);
		switch(cmd)
		{
		case 0x03:
			Resolver.Std_Upload();
			 break;
		case 0x05:
			 Resolver.Manual_Ctrl(data,len);
			 break;
		case 0x07:
			 Resolver.Path_Download(data,len);
			 break;
		case 0x09:
			 Resolver.Floor_Download(data,len);
			 break;
		case 11:
			  Resolver.Pos_Download(data,len);
			 break;
		}
	}
	return 0;
}
