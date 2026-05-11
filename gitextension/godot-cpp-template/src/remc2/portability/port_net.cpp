#define _CRT_SECURE_NO_WARNINGS
#include "port_net.h"
#include <thread>

#define USE_BOOST_ASIO_

#ifdef USE_BOOST_ASIO_
//#include <boost/asio.hpp>
//using namespace boost;
#else
//#include <asio.hpp>
#endif

uint8_t* Zero_pointer_net = nullptr;

int timest_index = 0;
clock_t timest_timer = 0;
const int timest_max_mess = 400;
std::string timest_buffer[timest_max_mess];

//#define TEST_NETWORK_MESSAGES

//#define TEST_BAD_NETWORK

//#define TEST_TIME_NETWORK_MESSAGES

using namespace std;

typedef struct {
	myNCB* connection;
	long startTime;
	int timeout;
	int32_t index;
} connectionTime;
//std::vector<connectionTime> handleConnections;

enum Neti_type {
	NETI_NOT_SET,
	NETI_ADD_NAME,
	NETI_ADD_NAME_REJECT,
	NETI_ADD_NAME_OK,
	NETI_CALL,
	NETI_CALL_ACCEPT,
	NETI_CALL_REJECT,
	NETI_LISTEN,
	//#define NETI_LISTEN_ACCEPT,
	NETI_LISTEN_REJECT,
	NETI_RECEIVE
	};

Neti_type netstate_shared = NETI_NOT_SET;

const int32_t MESS_UNKNOWN = -1;
const int32_t MESS_CLIENT_SERVER_NAME_ADDED = 0;
const int32_t MESS_SERVER_SERVER_NAME_ADDED = 1;
//const int32_t MESS_CLIENT_REGISTER_TIMEOUT = 2;
//const int32_t MESS_SERVER_REGISTER_TIMEOUT = 3;
const int32_t MESS_CLIENT_TESTADDNAME = 4;
const int32_t MESS_SERVER_TESTADDNAME_OK = 5;
const int32_t MESS_SERVER_TESTADDNAME_REJECT = 6;
const int32_t MESS_CLIENT_MESSAGE_LISTEN = 7;
const int32_t MESS_CLIENT_MESSAGE_CALL = 8;
const int32_t MESS_SERVER_CALL_ACCEPT = 9;
const int32_t MESS_SERVER_LISTEN_ACCEPT = 10;
const int32_t MESS_SERVER_CALL_REJECT = 11;
const int32_t MESS_SERVER_LISTEN_REJECT = 12;
const int32_t MESS_CLIENT_CANCEL = 13;
const int32_t MESS_CLIENT_DELETE = 14;
const int32_t MESS_CLIENT_SEND = 15;
const int32_t MESS_SERVER_SEND_OK = 16;
const int32_t MESS_SERVER_SEND = 17;
const int32_t MESS_CLIENT_GET_IP = 18;
const int32_t MESS_SERVER_GIVE_IP = 19;

char* MessageIndexToText(int32_t index)
{
	switch (index) {
	case MESS_CLIENT_SERVER_NAME_ADDED:
		return (char*)"MESS_CLIENT_SERVER_NAME_ADDED";
	case MESS_SERVER_SERVER_NAME_ADDED:
		return (char*)"MESS_SERVER_SERVER_NAME_ADDED";
//	case MESS_CLIENT_REGISTER_TIMEOUT:
//		return (char*)"MESS_CLIENT_REGISTER_TIMEOUT";
//	case MESS_SERVER_REGISTER_TIMEOUT:
//		return (char*)"MESS_SERVER_REGISTER_TIMEOUT";
	case MESS_CLIENT_TESTADDNAME:
		return (char*)"MESS_CLIENT_TESTADDNAME";
	case MESS_SERVER_TESTADDNAME_OK:
		return (char*)"MESS_SERVER_TESTADDNAME_OK";
	case MESS_SERVER_TESTADDNAME_REJECT:
		return (char*)"MESS_SERVER_TESTADDNAME_REJECT";
	case MESS_CLIENT_MESSAGE_LISTEN:
		return (char*)"MESS_CLIENT_MESSAGE_LISTEN";
	case MESS_CLIENT_MESSAGE_CALL:
		return (char*)"MESS_CLIENT_MESSAGE_CALL";
	case MESS_SERVER_CALL_ACCEPT:
		return (char*)"MESS_SERVER_CALL_ACCEPT";
	case MESS_SERVER_LISTEN_ACCEPT:
		return (char*)"MESS_SERVER_LISTEN_ACCEPT";
	case MESS_SERVER_CALL_REJECT:
		return (char*)"MESS_SERVER_CALL_REJECT";
	case MESS_SERVER_LISTEN_REJECT:
		return (char*)"MESS_SERVER_LISTEN_REJECT";
	case MESS_CLIENT_CANCEL:
		return (char*)"MESS_CLIENT_CANCEL";
	case MESS_CLIENT_DELETE:
		return (char*)"MESS_CLIENT_DELETE";
	case MESS_CLIENT_SEND:
		return (char*)"MESS_CLIENT_SEND";
	case MESS_SERVER_SEND_OK:
		return (char*)"MESS_SERVER_SEND_OK";
	case MESS_SERVER_SEND:
		return (char*)"MESS_SERVER_SEND";
	case MESS_CLIENT_GET_IP:
		return (char*)"MESS_CLIENT_GET_IP";
	case MESS_SERVER_GIVE_IP:
		return (char*)"MESS_SERVER_GIVE_IP";
	}
	return (char*)"";
}

#pragma pack (1)
typedef struct message_info{
	uint32_t size;
	int32_t message;
	int32_t index;
	int32_t port;
	shadow_myNCB messNCB;
	char data[2048*30];
};
#pragma pack (16)

shadow_myNCB myNCBtoShadow(myNCB from) {
	shadow_myNCB to;
	to.ncb_command_0= from.ncb_command_0;
	to.ncb_retcode_1 = from.ncb_retcode_1;
	to.ncb_lsn_2 = from.ncb_lsn_2;
	to.ncb_num_3 = from.ncb_num_3;
	to.ncb_buffer_4 = (from.ncb_buffer_4.p - Zero_pointer_net);
	to.ncb_bufferLength_8 = from.ncb_bufferLength_8;
	for (int i = 0; i < 16; i++)to.ncb_callName_10[i] = from.ncb_callName_10[i];
	for (int i = 0; i < 16; i++)to.ncb_name_26[i] = from.ncb_name_26[i];
	to.ncb_rto_42 = from.ncb_rto_42;
	to.ncb_sto_43 = from.ncb_sto_43;
	to.ncb_cmd_cplt_49 = from.ncb_cmd_cplt_49;
	return to;
};
myNCB myNCBfromShadow(shadow_myNCB from) {
	myNCB to;
	to.ncb_command_0 = from.ncb_command_0;
	to.ncb_retcode_1 = from.ncb_retcode_1;
	to.ncb_lsn_2 = from.ncb_lsn_2;
	to.ncb_num_3 = from.ncb_num_3;
	to.ncb_buffer_4.p = (from.ncb_buffer_4 + Zero_pointer_net);
	to.ncb_bufferLength_8 = from.ncb_bufferLength_8;
	for (int i = 0; i < 16; i++)to.ncb_callName_10[i] = from.ncb_callName_10[i];
	for (int i = 0; i < 16; i++)to.ncb_name_26[i] = from.ncb_name_26[i];
	to.ncb_rto_42 = from.ncb_rto_42;
	to.ncb_sto_43 = from.ncb_sto_43;
	to.ncb_cmd_cplt_49 = from.ncb_cmd_cplt_49;
	return to;
};

std::string DataToString(message_info messInfo)
{
	std::string output;
	for (int i = 0; i < messInfo.size + sizeof(shadow_myNCB) + 24; i++)
	{
		output += ((char*)&messInfo)[i];
	}
	return output;
}

std::string Pack_Message(uint32 message, shadow_myNCB locNCB, int32_t locIndex, int32_t port=-1, const char* data = nullptr, int size_of_data = 0) {
	message_info locmessage_info;
	locmessage_info.message = message;
	locmessage_info.size = size_of_data;
	locmessage_info.messNCB = locNCB;
	locmessage_info.index = locIndex;
	locmessage_info.port = port;
	if(data)
		memcpy(locmessage_info.data, data, size_of_data);
	return DataToString(locmessage_info);
}

message_info Unpack_Message(std::string data) {
	message_info output;
	for (int i = 0; i < sizeof(shadow_myNCB) + 24; i++)
		((char*)(&output))[i] = data[i];
	for (int i = sizeof(shadow_myNCB) + 24; i < output.size + sizeof(shadow_myNCB) + 24; i++)
		((char*)(&output))[i] = data[i];
	return output;
}

void singleThreadSleep(int sleepMs)
{
	std::this_thread::sleep_for(std::chrono::milliseconds(sleepMs));
}

std::string IHaveNameStrP = "";

FILE* debug_net_output;
const char* debug_net_filename1 = "net_messages_log.txt";
std::string debug_net_filename2 = {};

bool debug_net_first = true;

//std::mutex print_mt;

void debug_net_printf(const char* format, ...) {
}


//std::vector<std::string> NetworkName;
typedef struct {
	std::string adress;
	int port;
} TypeIpPort;
//std::vector<TypeIpPort> clientIpPort;

//std::vector<std::string> ListenName;
//std::vector<TypeIpPort> clientListenID;
//std::vector<std::string> ListenName2;
//std::vector<TypeIpPort> clientListenID2;

//std::vector<myNCB*> clientConnection;

bool receiveServerAddName = false;

bool ReceiveServerAddName() {
	return receiveServerAddName;
}

TypeIpPort GetIpPortFromName(std::string name) {
	TypeIpPort result;
	return result;
}

std::string GetNameNetworkFromId(TypeIpPort ipPort) {
	return "";
}

std::string GetNameNetwork(std::string name) {
	return "";
}

int GetIndexNetworkIpPort(TypeIpPort ipPort) {
	return -1;
}

int GetNameNetworkIndex(std::string name) {
	return -1;
}

void AddNetworkName(std::string name, TypeIpPort ipPort) {
}

bool ExistNetworkName(std::string name, TypeIpPort ipPort) {
	return false;
}

TypeIpPort GetOtherSide(TypeIpPort ipPort) {
	TypeIpPort result;
	return result;
}

std::string GetListenNetwork(std::string name) {
	return "";
}

int GetNameListenIndex(std::string name) {
	return -1;
}

int GetName2ListenIndex(std::string name) {
	return -1;
}

void AddListenName(myNCB* connection) {//10 26
}

bool AddListenName2(const shadow_myNCB* connection){
	return true;
}

bool IsListenName2(const shadow_myNCB* connection) {
	return true;
}

void RemoveListenName(std::string name) {
}

void RemoveNetworkName(std::string name) {
}

bool TestAddName(std::string name, uint32_t clientID) {
	return false;
}

long oldtime_shared;
int networkTimeout_shared = 10000;

class myMutex {
private:
	bool locked = false;
public:
	void lock() {
		while (locked);
		locked = true;
	};
	void unlock() {
		locked = false;
	};
};//replace std::mutex for debug

//myMutex handleConnections_mt;
//std::mutex handleConnections_mt;
//std::mutex netstate_mt;
//std::mutex networkTimeout_mt;

Neti_type netstate()
{
	Neti_type result;
	return result;
}

void netstate(Neti_type input)
{
}

//std::mutex clientConnection_mt;

bool setListen(myNCB* locNCB) {
	bool result = false;
	return result;
}

connectionTime* GetConnection(int32_t index) {
	connectionTime* result = NULL;
	return result;
};

void DeleteConnection(int32_t index)
{
};

void SetNetworkTime(int32_t index, int timeout) {
};

int32_t lastIndex = 0;
//std::mutex index_mt;

int32_t GetNextIndex() {
	return 0;
}

void setListenConnection(myNCB* connection) {
}

enum { max_length = 1024 };

void deleteListenConnection(myNCB* connection) {
};

namespace MyNetworkLib {
	class typeConfirmedMessage {
	public:
		std::string cxMessage;
		std::string cxDestination_ip;
		unsigned short cxPort;
		//std::chrono::system_clock::time_point timestamp;
		typeConfirmedMessage(std::string message, std::string destination_ip, unsigned short port) {
			cxMessage = message;
			cxDestination_ip = destination_ip;
			cxPort = port;
			//timestamp = std::chrono::system_clock::now();
		};
	};	

	const int maxConfirmedMessage = 2000;

	class NetworkClass
	{
	private:
		thread receiver_thread;
		thread receiver2_thread;
		thread handler_thread;
		bool HandleReceiver2On;
		bool HandleReceiverOn;
		bool HandleHandlerOn;
		int clPort;
		int clServerPort;
		std::string clHost;
		bool clIam_server = false;
		
		bool IpPortIsSet = false;

		unsigned int stamp = 0;

		//asio::io_service io_service_TCP;
		//asio::ip::tcp::socket* sockptr_TCP;

		//std::vector<TypeIpPort> registered;

		bool serverAddname = false;

		void Receiver2();
		void Receiver();

		//bool HandleMessagesServerPre(std::string bufferStr, asio::ip::udp::endpoint sender);
		//bool HandleMessagesClientPre(std::string bufferStr, asio::ip::udp::endpoint sender);
		//void HandleMessagesServer(std::string bufferStr, asio::ip::udp::endpoint sender);
		//void HandleMessagesClient(std::string bufferStr, asio::ip::udp::endpoint sender);

		void Handler();
		//void SendGiveIPMessage(asio::ip::udp::endpoint sender, int port);
		
		bool Registered(TypeIpPort ipPort);
		void Register(TypeIpPort ipPort);

		void StartReceiver2();
		void StopReceiver2();
		void StartReceiver();
		void StopReceiver();
		void StartHandler();
		void StopHandler();

		void SendToServer(const std::string& message);
		void SendToClient(const std::string& message, std::string adress, int port);

	public:
		NetworkClass(bool iam_server, std::string host, int port, int serverPort, bool IAmServer);
		~NetworkClass();

		bool SendMessage_UDP(const std::string& message, const std::string& destination_ip, const unsigned short port);

		void AddName(myNCB* connection, int32_t index);
		void AddNameReceive(bool nameIsOk, uint32_t id);
		void CancelNetwork(myNCB* connection, int32_t index);
		void DeleteNetwork(myNCB* connection, int32_t index);
		void CallNetwork(myNCB* connection, int32_t index);
		void ListenNetwork(myNCB* connection, int32_t index);
		void SendNetwork(myNCB* connection, int32_t index);
		void ReceiveNetwork(myNCB* connection);
	};

	NetworkClass::NetworkClass(bool iam_server, std::string host, int port, int serverPort, bool IAmServer) {
		clPort = port;
		clServerPort = serverPort;
		clHost = host;
		clIam_server = iam_server;

		//if ((clIam_server) && (clPort!=clServerPort))
		//	StartReceiverServer();
		StartReceiver();
		StartReceiver2();
		StartHandler();
	};

	NetworkClass::~NetworkClass() {
		//if ((clIam_server) && (clPort != clServerPort))
		//	StopReceiverServer();
		StopReceiver();
		StopReceiver2();
		StopHandler();
	};

	bool NetworkClass::SendMessage_UDP(const std::string& message, const std::string& destination_ip,
		const unsigned short port) {
		if (!IpPortIsSet)
		{
			message_info unpacked_message = Unpack_Message(message);
			if ((unpacked_message.message!= MESS_CLIENT_GET_IP)&& (unpacked_message.message != MESS_SERVER_GIVE_IP))
				return false;
		}

#ifdef TEST_NETWORK_MESSAGES
		debug_net_printf("Send message: %s ip: %s port: %d\n", message.c_str(), destination_ip.c_str(), port);
#endif //TEST_NETWORK_MESSAGES
		//asio::io_service io_service;
		//asio::ip::udp::socket socket(io_service);
		// Create the remote endpoint using the destination ip address and
		// the target port number.  This is not a broadcast
		//auto remote = asio::ip::udp::endpoint(asio::ip::address::from_string(destination_ip), port);
#ifndef __ANDROID__
		try
#endif
		{
		}
#ifndef __ANDROID__
#ifdef USE_BOOST__
		catch (const system::system_error& ex)
#else
		catch (std::exception& e)
#endif
		{
			// Exception thrown!
			// Examine ex.code() and ex.what() to see what went wrong!
			return false;
		}
#endif
		return true;
	}

	void NetworkClass::StartReceiver2() {
		HandleReceiver2On = true;
		receiver2_thread = std::thread(&NetworkClass::Receiver2, this);
	}

	void NetworkClass::StopReceiver2() {
		HandleReceiver2On = false;
		SendMessage_UDP("Xend", "127.0.0.1", clPort);
		singleThreadSleep(500);
		receiver2_thread.join();
	};

	void NetworkClass::StartReceiver() {
		HandleReceiverOn = true;
		receiver_thread = std::thread(&NetworkClass::Receiver, this);
	}

	void NetworkClass::StopReceiver() {
		HandleReceiverOn = false;
		singleThreadSleep(500);
		receiver_thread.join();
	};

	void NetworkClass::StartHandler() {
		HandleHandlerOn = true;
		handler_thread = std::thread(&NetworkClass::Handler, this);
	}

	void NetworkClass::StopHandler() {
		HandleHandlerOn = false;
		singleThreadSleep(500);
		handler_thread.join();
	};

	void NetworkClass::Register(TypeIpPort ipPort) {
		//registered.push_back(ipPort);
	}

	void NetworkClass::Handler() {
	}

	//std::vector<type_savedMessages> savedMessages;

	void NetworkClass::Receiver2() {
	}

	void NetworkClass::Receiver() {
	}

	void NetworkClass::SendToServer(const std::string& message) {
		SendMessage_UDP(message, clHost, clServerPort);
	};

	void NetworkClass::SendToClient(const std::string& message, std::string adress, int port) {
		SendMessage_UDP(message, adress, port);
	};

	void NetworkClass::AddName(myNCB* connection, int32_t index) {
		SendToServer(Pack_Message(MESS_CLIENT_TESTADDNAME, myNCBtoShadow(*connection), index, clPort, connection->ncb_name_26, sizeof(connection->ncb_name_26)));
	};

	void NetworkClass::AddNameReceive(bool nameIsOk, uint32_t id)
	{
		if (nameIsOk)
			;// AddNetworkName(std::string name, uint32_t id)
		else
			;//xx
	}

	void NetworkClass::CancelNetwork(myNCB* connection, int32_t index) {
		SendToServer(Pack_Message(MESS_CLIENT_CANCEL, myNCBtoShadow (*connection), index, clPort, connection->ncb_name_26, sizeof(connection->ncb_name_26)));
		deleteListenConnection(connection);
	}

	void NetworkClass::DeleteNetwork(myNCB* connection, int32_t index) {
	}

	void NetworkClass::CallNetwork(myNCB* connection, int32_t index) {
		SendToServer(Pack_Message(MESS_CLIENT_MESSAGE_CALL, myNCBtoShadow(*connection), index, clPort, (char*)connection, sizeof(*connection)));// +connection->ncb_name_26 + std::string(";") + connection->ncb_callName_10);
	};

	void NetworkClass::ListenNetwork(myNCB* connection, int32_t index) {
		SendToServer(Pack_Message(MESS_CLIENT_MESSAGE_LISTEN, myNCBtoShadow(*connection), index, clPort, (char*)connection, sizeof(*connection)));/*->ncb_callName_10 + ';' + connection->ncb_name_26*/
		setListenConnection(connection);
	};

	void NetworkClass::SendNetwork(myNCB* connection, int32_t index) {
		if (connection->ncb_command_0 != 0x94)
		{
			int a = 10;
			int b = 0;
			int c = a / b;
		}
		SendToServer(Pack_Message(MESS_CLIENT_SEND, myNCBtoShadow(*connection), index, clPort, (char*)connection->ncb_buffer_4.p, connection->ncb_bufferLength_8));
#ifdef TEST_NETWORK_MESSAGES
		debug_net_printf("CONVERT TO MESSAGE:%d:%d\n", connection->ncb_bufferLength_8, connection->ncb_bufferLength_8);
#endif //TEST_NETWORK_MESSAGES
	};

	void NetworkClass::ReceiveNetwork(myNCB* connection) {
	};
}

MyNetworkLib::NetworkClass* locNetworkClass;


int messageToDelete = 1000;

//std::vector<std::string> recMessages;
//std::mutex RecMess_mt;

void AddRecMess(std::string message){
};


unsigned int GetRecCount() {
	return 0;
};

std::string GetRecMess() {
	return 0;
};

void CleanMessages(myNCB locNCB) {
}

int lastnetworkname = 0;

void makeNetwork(myNCB* connection) {
}

void printState(myNCB** connections) {
#ifdef TEST_NETWORK_MESSAGES
	debug_net_printf("NetworkGetState: %d %p %d %s\n", 0, connections[0], connections[0]->ncb_lsn_2, (!connections[0]->ncb_cmd_cplt_49) ? "true" : "false");
	debug_net_printf("NetworkGetState: %d %p %d %s\n", 1, connections[1], connections[1]->ncb_lsn_2, (!connections[1]->ncb_cmd_cplt_49) ? "true" : "false");
	debug_net_printf("NetworkGetState: %d %p %d %s\n", 2, connections[2], connections[2]->ncb_lsn_2, (!connections[2]->ncb_cmd_cplt_49) ? "true" : "false");
#endif //TEST_NETWORK_MESSAGES
}

void printState2(char* text) {
#ifdef TEST_NETWORK_MESSAGES
	debug_net_printf("%s", text);
#endif //TEST_NETWORK_MESSAGES
}

void timeState(bool start, const char* text) {
#ifdef TEST_TIME_NETWORK_MESSAGES
	if (start||(timest_index==0))timest_timer = clock();
	char buff[100];
	snprintf(buff, sizeof(buff), "%s | %d", text, clock()- timest_timer);
	timest_buffer[timest_index].assign(buff, strlen(buff));
	timest_index++;
	if (timest_index > timest_max_mess) {
		ofstream ofs("net_time_messages_log.txt", std::ofstream::out);
		for (int i = 0; i < timest_max_mess; i++) {

			ofs << timest_buffer[i] << endl; // I also tried replacing endl with a "\n"
		}
		ofs.close();
		exit(0);
	}
#endif //TEST_TIME_NETWORK_MESSAGES
}

void InitMyNetLib(bool iam_server, char* ip, int networkPort, int serverPort)
{
	if (iam_server)networkPort = serverPort;
	locNetworkClass = new MyNetworkLib::NetworkClass(iam_server, ip, networkPort, serverPort,true);
}

void EndMyNetLib() {
	singleThreadSleep(500);
	delete locNetworkClass;
}

