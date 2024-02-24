#include "BtcPrice.hpp"



BtcPrice::BtcPrice(std::string host, std::string port, std::string pass, BotType type)
 : Bot("btcPrice", host, port, pass, type) {}

BtcPrice& BtcPrice::operator=( const BtcPrice& ) { return *this; }

std::string	localTime(time_t now)
{
	std::string t = ctime(&now);
	t.erase(t.size() - 1);
	return (t);
}

std::string	localPtime( void )
{
	time_t				now = time(0);
	tm					*ltm = localtime(&now);
	std::stringstream	date;
	ltm->tm_year += 1900;
	ltm->tm_mon += 1;
	date << ":" << ltm->tm_year 
	<< "-"<< ltm->tm_mon 
	<< "-"<< ltm->tm_mday 
	<< " "<< ltm->tm_hour 
	<< "-" << ltm->tm_min << "-" 
	<< ltm->tm_sec;
	return (date.str());
}

size_t replayCallBack(void* contents, size_t size, size_t nmemb, std::string* response) {
    size_t total_size = size * nmemb;
    response->append((char*)contents, total_size);
    return total_size;
}

void	parseApiReplay(std::string& response)
{
	response.erase(0, response.find("rate_float") + 12);
	response.erase(response.find("}"));
}
std::string getBtcPrice()
{
	std::string response_data;
	CURLcode res;
	CURL* curl = curl_easy_init();
	if (curl)
	{
		curl_easy_setopt(curl, CURLOPT_URL, BTCAPI);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, replayCallBack);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_data);

		res = curl_easy_perform(curl);
		if (res == CURLE_OK)
			parseApiReplay(response_data);
		else
			response_data.clear();
		curl_easy_cleanup(curl);
	}
	return response_data;
}

void			BtcPrice::botReply(std::string msg)
{
	std::string	cmd, nick, Msg, botName;
	std::stringstream	ss(msg);
	std::string finalMsg;
	int sendBytes;
	if (msg.size() <= 2)
		return;
	msg.erase(msg.size() - 2);
	ss >> nick >> cmd >> botName >> Msg;
	if (cmd != "BTCPRICE" && cmd != "DATE")
		return;
	nick = extractNick(msg);
	if (cmd == "BTCPRICE")
		finalMsg = static_cast<std::string>("PRIVMSG") + " " + nick + " " + getBtcPrice() + "\r\n";
	else
		finalMsg = static_cast<std::string>("PRIVMSG") + " " + nick + " :" + localTime(time(0)) + "\r\n";
	if ((sendBytes = send(this->getSocketFd(), finalMsg.c_str(), finalMsg.size(), 0)) <= 0)
	{	if (sendBytes == 0)
			std::cerr << "Connection closed" << std::endl;
		else
			std::cerr << "send: " << strerror(errno) << std::endl;}
}


BtcPrice::~BtcPrice() {}