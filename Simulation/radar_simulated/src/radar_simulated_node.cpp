#include "header/radar_simulated_node.h"

/**************************************CLASS DEFINATION FOR NETWORK************************************
	网络通信接口（interface）		解析下载信息
*/

const unsigned char RADAR_HEAD[4]={0xCa,0xCb,0xCc,0xCd};
const unsigned char RADAR_TAIL[10]={0xEA,0xEB,0xEC,0xED,0x61,0x67,0x5C,0x14,0x89,0xc6};

void Clk_Callback(const rsu_msgs::HeartBeat::ConstPtr& msg)
{
	for(int i=0;i<4;i++)
	{
	}
}
int64_t Tolong(uint16_t ID, float length,float SpeedY,float Speed,float Ypos,float Xpos)
{
	uint64_t dat;
	unsigned char mov=0;
	uint64_t tmp=0;
	dat =0x00;
	dat&=0x7fffffffffffffff;
	//ID
	tmp=(uint64_t)(ID);

	dat|=((tmp&(uint64_t)(0x00ff))<<mov);
	mov+=8;
	//Length
	tmp=length*5;
	dat|=((tmp&(uint64_t)(0x007f))<<mov);
	mov+=7;
	//Spd_y
	tmp=SpeedY*10;
	dat|=((tmp&(uint64_t)(0x01ff))<<mov);
	mov+=11;
	//Spd_x
	tmp=Speed*10;
	dat|=((tmp&(uint64_t)(0x01ff))<<mov);
	mov+=11;
	//Y
	tmp=Ypos/0.128;
	dat|=((tmp&(uint64_t)(0x07ff))<<mov);
	mov+=13;
	//X
	tmp=Xpos/0.128;
	dat|=((tmp&(uint64_t)(0x07ff))<<mov);
	return dat;
}


net_ui::net_ui()
{
	Net_State=0;
}
net_ui::~net_ui()
{
	close(sock_ser);
	close(new_server_socket);
}
short net_ui::Longto(long long data, int low, int higth)
{
	int length = higth - low + 1;
	short tempint;
	short tempand;
	tempand = (short)((1 << length) - 1);
	data = data >> low;
	tempint = (short)data;
	return (short)(tempint & tempand);
}

void net_ui::PushData(vehicle *car)
{

	printf("\ec");
	printf("Prime Data:\n");
	send(new_server_socket,RADAR_HEAD,4,0);
	for(int m=0;m<4;m++)
	{
		printf("%x ",RADAR_HEAD[m]);
	}
	for(int i=0;i<5;i++)
	{
		printf("[");
		uint16_t dat=(0x05<<8)|0x02+i;
		char data_id[3];
		data_id[0]=dat/256; printf("%x ",data_id[0]);
		data_id[1]=dat%256; printf("%x ",data_id[1]);
		data_id[2]=8;printf("%x ",data_id[2]);
		send(new_server_socket,data_id,3,0);
		//printf("\n");
		//printf("%d",car[i].ID);
		//printf("\n");
		uint64_t tmp=Tolong(car[i].ID,car[i].length,car[i].SpeedY,car[i].Speed,car[i].Ypos,car[i].Xpos);
		unsigned char buf[8];
		for(int k=0;k<8;k++)
		{
			buf[k]=(tmp&((uint64_t)(0xff)<<((7-k)*8)))>>((7-k)*8);
			printf("%x ",buf[k]);
		}
		send(new_server_socket,buf,8,0);

		printf("]");
	}
	unsigned char null_data[33]={0};
	for(int l=0;l<33;l++)
	{
		printf("0x00 ");
	}
	send(new_server_socket,&null_data,33,0);
	for(int m=0;m<10;m++)
	{
		printf("%x ",RADAR_TAIL[m]);
	}
	send(new_server_socket,RADAR_TAIL,10,0);
	printf ("\n");

}
void net_ui::Connect()
{
	sock_ser = socket(AF_INET,SOCK_STREAM, 0);
	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(4002);
	servaddr.sin_addr.s_addr = htons(INADDR_ANY);
    	int opt =1;
	printf("Radar Simulator:Initialized!");
	if(bind(sock_ser,(struct sockaddr*)&servaddr,sizeof(servaddr)))
	{
		printf("Server Bind Port : %d Failed!\n",4002);
		exit(1);
	}
	printf("Radar Simulator:Bind Success!\n");
	if(listen(sock_ser,10) )
	{
		printf("Radar Simulator:Server Listen Failed!\n");
		exit(1);
	}
	printf("Radar Simulator:Listen Success!\n");
	struct sockaddr_in client_addr;
	socklen_t length = sizeof(client_addr);
	new_server_socket = accept(sock_ser,(struct sockaddr*)&client_addr,&length);
    if ( new_server_socket < 0)
	{
		printf("Server Accept Failed!\n");
		exit(0);
	}
    printf("Radar Simulator:Target Linked!\n");
}
/*************************************++++++++++*		       MAIN  	              *************************************/
int main(int argc, char** argv)
{
	ros::init(argc, argv, "radar_simulator");
	ros::NodeHandle n;
	ros::Subscriber clk_Recall;
	a=n.advertise<visualization_msgs::Marker>("cars",5);
	clk_Recall=n.subscribe<rsu_msgs::HeartBeat>("CORE_clk", 1,&Clk_Callback);
	ros::AsyncSpinner spinner(5); 	//数量由消息数量而定，线程数为消息数量+1
	spinner.start();

	vehicle car[5];
	car[0]=vehicle(0,2,10.0,3.0,0.0);
	car[1]=vehicle(1,1,15.0,3.0,2.0);
	car[2]=vehicle(2,3,5.0,8.0,4.0);
	car[3]=vehicle(3,3,5.0,8.0,16.0);
	car[4]=vehicle(4,0,0,0,0);
	net_ui Resolver;
	Resolver.Connect();
	ros::Rate radar_Rate(10);
	while(Resolver.n.ok())
	{
		static long i = 0;
		i++;
		Resolver.PushData(car);
		for(int i=0;i<3;i++)
		{
			printf("Vehicle State---ID:%d Speed:%f Length:%f XPos:%f\n",car[i].ID,car[i].Speed,car[i].length,car[i].Xpos);
			car[i].Move();
			car[i].PushTF();
			a.publish(car[i].marker);
		}
		a.publish(car[4].marker);
		radar_Rate.sleep();
	}
	return 0;
}


