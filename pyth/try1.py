#!/usr/bin/env python2
# -*- coding: utf-8 -*-
"""
Created on Thu Mar 29 04:58:27 2018

@author: wuyuankai
"""

from rsu_msgs.msg import radar
from rsu_msgs.msg import obj
import rospy
import socket 

def gen_veh_inf(h):
    t_2 = ''
    for i in range(8):
        h_16 = h[2*i:2*i+2]
        h_16int = int(h_16,16)
        h_2 = bin(h_16int)
        t = str(h_2[2:])
        t_bu_len = 8 -len(t)
        t_bu = ''
        for j in range(t_bu_len):
            t_bu = t_bu + '0'
        t = t_bu + t    
        t_2 = t_2 + t
    x_point1 = int(t_2[1:14],2)*0.128
    y_point1 = int(t_2[14:27],2)*0.128
    speed_x = int(t_2[27:38],2)*0.1
    speed_y = int(t_2[38:49],2)*0.1
    object_length = int(t_2[49:56],2)*0.2
    object_id = int(t_2[56:],2)
    return x_point1, y_point1, speed_x, speed_y, object_length, object_id

pub = rospy.Publisher('radar', radar)
rospy.init_node('talker', anonymous=True)
rate = rospy.Rate(10)

while not rospy.is_shutdown():
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)  
    sock.connect(('192.168.0.102', 4002))  
    import time  
    time.sleep(1)
    buffer = "" 
    t = 1
    while True:
        data = sock.recv(1)
        z = data.encode('hex')
        buffer = buffer + z
        #print buffer
        #length_str, ignored, buffer = buffer.partition('CACBCCCD')
        headflag = "cacbcccd"
        tailflag = "eaebeced61675c1489c6"
        if headflag in buffer and tailflag in buffer:
            t = t + 1
            lenbuffer = len(buffer)
            veh_num = int((lenbuffer - 94)/22)
            radar_msg = radar()
            h = radar_msg.header
            h.seq = t
            h.stamp = rospy.Time.now()
            h.frame_id = 'radar' 
            for i in range(veh_num):
                veh_id = buffer[8+22*i:12+22*i]
                #print veh_id
                veh_inf = buffer[14+22*i:30+22*i]
                #print veh_inf
                x_point1, y_point1, speed_x, speed_y, object_length, object_id = gen_veh_inf(veh_inf)
                #print x_point1, y_point1
                obj_msg = obj()
                obj_msg.pos_x = x_point1
                obj_msg.pos_y = y_point1
                obj_msg.v_x = speed_x
                obj_msg.v_y = speed_y
                obj_msg.length = object_length
                obj_msg.id = object_id
                
                radar_msg.objects.append(obj_msg)
            buffer = ""
            rospy.loginfo(radar_msg)
            pub.publish(radar_msg)
            rate.sleep();
