#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/prettywriter.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/error/en.h"
#define PORT 8401

static int server_sockfd = -1;
static char buffer[1024];
static struct sockaddr_in client_addr;
static int client_len = 0;
static int global_path_tick = 0;
static int block_tick = 0;

#define PRETTY_DEBUG 0

void handle_msg(char* buf, int len) {
	char b[1024];
	memset(b, 0, 1024);
	memcpy(b, buf, len);
	rapidjson::StringStream s(b);
	rapidjson::Document d;

	if(d.ParseStream(s).HasParseError()) {
		printf("handle_msg parse fail: err:[%d]%s, offset: %u\n", d.GetParseError(), rapidjson::GetParseError_En(d.GetParseError()), d.GetErrorOffset());
		return;
	}
	rapidjson::StringBuffer sb;
	rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(sb);
	d.Accept(writer);
	printf("\n%s\n",sb.GetString());

	rapidjson::Value::ConstMemberIterator itr = d.FindMember("type");

	if (itr == d.MemberEnd()) {
		printf("no [type] in msg\n");
		return;
	}

	std::string type = itr->value.GetString();
	//printf("[type]: %s\n", type.c_str());

	if(type == "pathReq") {
		rapidjson::Document jsonDoc; 
		jsonDoc.SetObject();
		jsonDoc.AddMember("type", "pathResp", jsonDoc.GetAllocator());
		jsonDoc.AddMember("id", d["id"].GetInt(), jsonDoc.GetAllocator());

		if(block_tick%4 == 0) {
			jsonDoc.AddMember("result", true, jsonDoc.GetAllocator());
		} else {
			jsonDoc.AddMember("result", false, jsonDoc.GetAllocator());
		}

		block_tick++;
		if(block_tick%2 == 0) {
			rapidjson::Value block(rapidjson::Type::kArrayType);

			for (int i = 0; i < 5; ++i) {
				rapidjson::Value object(rapidjson::Type::kObjectType);
				object.AddMember("x", 700+i, jsonDoc.GetAllocator());
				object.AddMember("y", 800+i, jsonDoc.GetAllocator());

				block.PushBack(object, jsonDoc.GetAllocator());
			}

			jsonDoc.AddMember("block", block, jsonDoc.GetAllocator());

		}

		rapidjson::StringBuffer pathResp;
		rapidjson::Writer<rapidjson::StringBuffer> writer(pathResp);
		jsonDoc.Accept(writer);

	    //printf("len:%d, resp: %s\n", pathResp.GetLength(), pathResp.GetString());
		strncpy(buffer, pathResp.GetString(), pathResp.GetLength());	

		sendto(server_sockfd, buffer, pathResp.GetLength(), 0, (struct sockaddr *)&client_addr, client_len);
	} else if(type == "info") {
		rapidjson::Document jsonDoc; 
		jsonDoc.SetObject();

		jsonDoc.AddMember("type", "hotInfo", jsonDoc.GetAllocator());

		// data array
		rapidjson::Value data(rapidjson::Type::kArrayType);

		for (int i = 0; i < 5; ++i) {
			rapidjson::Value object(rapidjson::Type::kObjectType);
			const std::string& agv_name = "ubot-" + std::to_string(300+i);

			object.AddMember("name", agv_name, jsonDoc.GetAllocator());
			object.AddMember("x", 10000+i, jsonDoc.GetAllocator());
			object.AddMember("y", 20000+i, jsonDoc.GetAllocator());

			// path array
			rapidjson::Value path(rapidjson::Type::kArrayType);
			for (int j = 0; j < 5; ++j) {
				std::string path_point = "p" + std::to_string(100+j);
				rapidjson::Value v;
				v.SetString(path_point.c_str(), jsonDoc.GetAllocator());
				path.PushBack(v, jsonDoc.GetAllocator());
			}
			object.AddMember("path", path, jsonDoc.GetAllocator());

			data.PushBack(object,jsonDoc.GetAllocator());
		}

		jsonDoc.AddMember("data", data, jsonDoc.GetAllocator());

		// to Serialize to JSON String
		rapidjson::StringBuffer hot;
		rapidjson::Writer<rapidjson::StringBuffer> writer(hot);
		jsonDoc.Accept(writer);
		// get json format string
		#if PRETTY_DEBUG
		rapidjson::PrettyWriter<rapidjson::StringBuffer> pw(sb);
		jsonDoc.Accept(pw);    // Accept() traverses the DOM and generates Handler events.
		printf("\nsend dump:%s\n",sb.GetString());
		#endif

		char b[1024];
		strncpy(b, hot.GetString(), hot.GetLength());

		sendto(server_sockfd, b, hot.GetLength(), 0, (struct sockaddr *)&client_addr, client_len);
	}

	global_path_tick++;
	if(global_path_tick % 3 == 0) {
		rapidjson::Document jsonDoc; 
		jsonDoc.SetObject();
		jsonDoc.AddMember("type", "globalpath", jsonDoc.GetAllocator());
		jsonDoc.AddMember("serverpathid", global_path_tick, jsonDoc.GetAllocator());

		rapidjson::Value point(rapidjson::Type::kArrayType);

		for (int i = 0; i < 5; ++i) {
			rapidjson::Value object(rapidjson::Type::kObjectType);
			object.AddMember("x", 700+i, jsonDoc.GetAllocator());
			object.AddMember("y", 800+i, jsonDoc.GetAllocator());

			point.PushBack(object, jsonDoc.GetAllocator());
		}

		jsonDoc.AddMember("point", point, jsonDoc.GetAllocator());

		// to Serialize to JSON String
		rapidjson::StringBuffer glob;
		rapidjson::Writer<rapidjson::StringBuffer> writer(glob);
		jsonDoc.Accept(writer);
		// get json format string
		#if PRETTY_DEBUG
		rapidjson::StringBuffer sb;
		rapidjson::PrettyWriter<rapidjson::StringBuffer> pw(sb);
		jsonDoc.Accept(pw);    // Accept() traverses the DOM and generates Handler events.
		printf("\nglobalpath dump:%s\n",sb.GetString());
		#endif

		char b[1024];
		strncpy(b, glob.GetString(), glob.GetLength());

		sendto(server_sockfd, b, glob.GetLength(), 0, (struct sockaddr *)&client_addr, client_len);
	}
}

int main(int argc, char** argv)
{
	
	int server_len = 0;
	
	
	char* sendbuffer = (char*)calloc(1,1024);
	char* dump;
	int recv_len = 0;
	struct sockaddr_in server_addr;
	
	server_sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	server_addr.sin_port = htons(PORT);
	server_len = sizeof(server_addr);
	bind(server_sockfd, (struct sockaddr*)&server_addr, server_len);
	signal(SIGCHLD, SIG_IGN);

	printf("begin receive package with port:%d\n",PORT);
	while(1)
	{	
		recv_len = recvfrom(server_sockfd, buffer, sizeof(buffer), 0, 
				(struct sockaddr*)&client_addr,(socklen_t*) &client_len);

		printf("len: %d, port %d\n", recv_len, client_addr.sin_port);
        
		handle_msg(buffer, recv_len);
	}

	close(server_sockfd);
}


