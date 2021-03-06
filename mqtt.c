#include "mqtt.h"
#include "stdio.h"


unsigned char GetDataFixedHead(unsigned char MesType,unsigned char DupFlag,unsigned char QosLevel,unsigned char Retain)
{
	unsigned char dat = 0;
	dat = (MesType & 0x0f) << 4;
	dat |= (DupFlag & 0x01) << 3;
	dat |= (QosLevel & 0x03) << 1;
	dat |= (Retain & 0x01);
	return dat;
}
uint16_t GetDataConnet(unsigned char *buff)//获取连接的数据包正确连接返回20 02 00 00
{
	unsigned int i,len,lennum = 0;
	unsigned char *msg;
	buff[0] = GetDataFixedHead(MQTT_TypeCONNECT,0,0,0);

	
	buff[2] = 0x00;
	buff[3] = 0x04;
	buff[4] = 'M';
	buff[5] = 'Q';
	buff[6] = 'T';
	buff[7] = 'T';
	buff[8] = 0x04;//协议级别 Protocol Level
	buff[9] = 0 /*| (MQTT_StaCleanSession << 1)*/ | (MQTT_StaWillFlag << 2)
							| (MQTT_StaWillQoS << 3) | (MQTT_StaWillRetain << 5) 
							|	(MQTT_StaPasswordFlag << 6) |(MQTT_StaUserNameFlag << 7);//连接标志
	buff[10] = MQTT_KeepAlive >> 8;
	buff[11] = MQTT_KeepAlive;
	len = strlen(MQTT_ClientIdentifier);
	buff[12] = len >> 8;
	buff[13] = len;
	msg = (unsigned char *)MQTT_ClientIdentifier;
	for(i = 0;i<len;i++)
	{
		buff[14+i] =  msg[i];
	}
	lennum += len;
	if(MQTT_StaWillFlag)
	{
		len = strlen(MQTT_WillTopic);
		buff[13 + lennum + 1] = len >> 8;
		buff[13 + lennum + 2] = len;
		lennum += 2;
		msg = (unsigned char *)MQTT_WillTopic;
		for(i = 0;i<len;i++)
		{
			buff[14+lennum+i] =  msg[i];
		}
		lennum += len;
		len = strlen(MQTT_WillMessage);
		buff[12] = len >> 8;
		buff[13] = len;
		lennum += 2;
		msg = (unsigned char *)MQTT_WillMessage;
		for(i = 0;i<len;i++)
		{
			buff[14+lennum+i] =  msg[i];
		}
		lennum += len;
	}
	if(MQTT_StaUserNameFlag)
	{
		len = strlen(MQTT_UserName);
		buff[13 + lennum + 1] = len >> 8;
		buff[13 + lennum + 2] = len;
		lennum += 2;
		msg = (unsigned char *)MQTT_UserName;
		for(i = 0;i<len;i++)
		{
			buff[14+lennum+i] =  msg[i];
		}
		lennum += len;
		
	}
	if(MQTT_StaPasswordFlag)
	{
		len = strlen(MQTT_Password);
		buff[13 + lennum + 1] = len >> 8;
		buff[13 + lennum + 2] = len;
		lennum += 2;
		msg = (unsigned char *)MQTT_Password;
		for(i = 0;i<len;i++)
		{
			buff[14+lennum+i] =  msg[i];
		}
		lennum += len;
	}
	buff[1] = 13 + lennum - 1;

	return buff[1];
}


void GetDataDisConnet(unsigned char *buff)//获取断开连接的数据包
{
	buff[0] = 0xe0;
	buff[1] = 0;
}
void GetDataPINGREQ(unsigned char *buff)//心跳请求的数据包成功返回d0 00
{
	buff[0] = 0xc0;
	buff[1] = 0;
}
/*
	成功返回90 0x 00 Num RequestedQoS
*/
void GetDataSUBSCRIBE(unsigned char *buff,const char *dat,unsigned int Num,unsigned char RequestedQoS)//订阅主题的数据包 Num:主题序号 RequestedQoS:服务质量要求0,1或2
{
	unsigned int i,len = 0,lennum = 0; 
	buff[0] = 0x82;
	len = strlen(dat);
	buff[2] = Num>>8;
	buff[3] = Num;
	buff[4] = len>>8;
	buff[5] = len;
	for(i = 0;i<len;i++)
	{
		buff[6+i] = dat[i];
	}
	lennum = len;
	buff[6 + lennum ] = RequestedQoS;
	buff[1] = lennum + 5;
}
uint16_t GetDataPUBLISH(unsigned char *buff,unsigned char dup, unsigned char qos,unsigned char retain,const char *topic ,const char *msg)//获取发布消息的数据包
{
	unsigned int i,len=0,lennum=0;
	buff[0] = GetDataFixedHead(MQTT_TypePUBLISH,dup,qos,retain);
	
	len = strlen(topic);
	buff[2] = len>>8;
	buff[3] = len;
	for(i = 0;i<len;i++)
	{
		buff[4+i] = topic[i];
	}
	lennum = len;
	len = strlen(msg);
	for(i = 0;i<len;i++)
	{
		buff[4+i+lennum] = msg[i];
	}
	lennum += len;
	buff[1] = lennum + 2;
	return buff[1];
	
}


uint16_t GetDataPointPUBLISH(unsigned char *buff,unsigned char dup, unsigned char qos,unsigned char retain,const char *topic ,const char *msg)//获取发布消息的数据包
{
	unsigned int i;
	int len_size = 1;
	*buff = GetDataFixedHead(MQTT_TypePUBLISH,dup,qos,retain);

	int remainedLength = 0;
	
	int topic_len = strlen(topic);
	int msg_len = strlen(msg);

	remainedLength += 2; //topic_len bytes
	remainedLength += topic_len;
	remainedLength += 2; // PacketIdentifier bytes

	remainedLength += 1; //type bytes
	remainedLength += 2; //msg_len bytes
	remainedLength += msg_len;

	if(remainedLength>=0x80)
	{
		
		*++buff = (0x80|(remainedLength%0x80));
		*++buff = remainedLength/0x80;
		len_size += 2;
	}
	else
	{
		*++buff = remainedLength;
		len_size += 1;
	}
	
	
	*++buff = topic_len>>8;
	*++buff = topic_len;
	
	
	for(i = 0;i<topic_len;i++)
	{
		
		*++buff = topic[i];
		
	}
	*++buff = 0;
	*++buff = 0x2;


	*++buff = 0x01;
	
	*++buff = msg_len>>8;
	*++buff = msg_len;

	
	for(i = 0;i<msg_len;i++)
	{
		
		*++buff = msg[i];
	}
	
	return remainedLength + len_size;
	
}



void PlatfromPUBLISHAnalysis(unsigned char *buff, FixedHeader_t *FixedHeader, VariableHeader_t *VariableHeader, unsigned char *payload)
{
	
	FixedHeader->PacketType = *buff>>4;

	uint8_t multiplier = 1;
	uint32_t len = 0;

	
	do{
		len += (*++buff & 0x7f) * multiplier;
		multiplier *= 0x80;
	}while((*buff & 0x80) != 0);

	FixedHeader->RemainingLength = len;

	VariableHeader->ProtocolNameLength = *++buff;
	
	memcpy(VariableHeader->ProtocolName, ++buff, VariableHeader->ProtocolNameLength);

	buff += VariableHeader->ProtocolNameLength;
	VariableHeader->ConnectFlag.UserFlag = (*buff>>7) & 0x01;
	VariableHeader->ConnectFlag.PasswordFlag = (*buff>>6) & 0x01;
	VariableHeader->ConnectFlag.WillRetainFlag = (*buff>>5) & 0x01;
	VariableHeader->ConnectFlag.WillQosFlag = (*buff>>2) & 0x03;
	VariableHeader->ConnectFlag.WillFlag = (*buff>>1) & 0x01;
	VariableHeader->ConnectFlag.CleanSessionFlag = *buff & 0x01;

	VariableHeader->KeepAlive = *++buff;
	
	memcpy(payload, ++buff, VariableHeader->ProtocolNameLength-10);
	
}


void PlatfromCmdAnalysis(unsigned char *buff, FixedHeader_t *FixedHeader, unsigned short *topicnamelen, unsigned char *topicname, unsigned short *payloadlen, unsigned char *payload)
{
	
	FixedHeader->PacketType = *buff>>4;

	uint8_t multiplier = 1;
	uint32_t len = 0;
	uint16_t topic_len = 0;
	uint16_t payload_len = 0;
	
	do{
		len += (*++buff & 0x7f) * multiplier;
		multiplier *= 0x80;
	}while((*buff & 0x80) != 0);

	FixedHeader->RemainingLength = len;
	printf("FixedHeader->RemainingLength = %d.\n", FixedHeader->RemainingLength);

	topic_len = (*++buff)<<8;
	topic_len |= (*++buff);
	printf("topic_len = %d.\n", topic_len);
	if(topicnamelen != NULL)
	{
		*topicnamelen = topic_len;
	}
	
	memcpy(topicname, ++buff, topic_len);

	buff += topic_len;

	payload_len = len - topic_len - 2;  //topic_len byte
	if(payloadlen != NULL)
	{
		*payloadlen = payload_len;
	}
	
	memcpy(payload, buff, payload_len);
	
}



