#include <bits/stdc++.h>
#include <curl/curl.h>
#include <jsoncpp/json/json.h>
//~ #include <project_time.h>

using namespace std;


int TotalTimeTaken = -1;

namespace
{
    std::size_t callback(
            const char* in,
            std::size_t size,
            std::size_t num,
            std::string* out)
    {
        const std::size_t totalBytes(size * num);
        out->append(in, totalBytes);
        return totalBytes;
    }
}

long long calcTime(string &start,string &end)
{
    std::tm st = {};
    std::tm en = {};
    std::istringstream ss1(start),ss2(end);
    if(start>end)
    return -1;
    if (ss1 >> std::get_time(&st, "%Y-%m-%dT%H:%M") && ss2 >> std::get_time(&en, "%Y-%m-%dT%H:%M"))
    {
        return (long long)(std::mktime(&en)-std::mktime(&st));
    }
    return -1;
}

string finTime(string &s,long long T)
{
    std::tm t = {};
    std::istringstream ss(s);
    if (ss >> std::get_time(&t, "%Y-%m-%dT%H:%M"))
    {
		//~ string s;
		time_t temp =  (mktime(&t)+T);
		tm* td = localtime(&temp);
		char buff[100];
		strftime(buff,sizeof(buff),"%Y-%m-%dT%H:%M",td);
		string ls(buff);
		return ls;
		//~ return s;
    }
    return s;
}

string time_to_string(time_t t)
{
	tm* temp = localtime(&t);
	char buff[100];
	strftime(buff,sizeof(buff),"%Y-%m-%d %H:%M",temp);
	string ls(buff);
	ls[10] = 'T';
	return ls;
}

map<string,pair<string,pair<string,string>>> min_route(string src,string dst)
{
	map<string,string> ans;
	map<string,string> mpend_time;
	priority_queue<pair<long long,string>,vector<pair<long long,string>>,greater<pair<long long,string>>> pq;
	pq.push(make_pair(0,src));
	map<string,long long> tm;
	map<string,pair<string,pair<string,string>>> parent;
	parent[src] = make_pair("none",make_pair("0","0"));
	tm[src] = 0;
	string init_time = time_to_string(time(0));
	int i = 0;
	mpend_time[src] = init_time;
	while(!pq.empty())
	{
		long long time = pq.top().first;
		string u = pq.top().second;
		pq.pop();
		//~ cout<<i<<" "<<u<<" th iteration\n";
		++i;
		if(i>20)
		{
			cout<<"Api calls exhausted\n";
			return parent;
		}
		if(u==dst)
		{
			//~ cout<<"sdsdgd\n";
			TotalTimeTaken = time;
			return parent;
		}
		
			//start of dijkstra's 
			CURL *hnd = curl_easy_init();
			
			curl_easy_setopt(hnd, CURLOPT_CUSTOMREQUEST, "GET");
			string HeadUrl = "https://aerodatabox.p.rapidapi.com/flights/airports/icao/";
			string start_time = finTime(init_time,tm[u]);
			string end_time = finTime(start_time,43000);
			string TailUrl = "?withLeg=true&withCancelled=false&withCodeshared=true&withCargo=false&withPrivate=true&withLocation=true";
			string URL = HeadUrl+u+"/"+start_time+"/"+end_time+TailUrl;
			string url = "https://aerodatabox.p.rapidapi.com/flights/airports/icao/ZSPD/2021-11-09T08:00/2021-11-09T20:00?withLeg=true&withCancelled=false&withCodeshared=true&withCargo=false&withPrivate=true&withLocation=true";
			curl_easy_setopt(hnd, CURLOPT_URL,URL.c_str() );
			//credentials start..
			
			struct curl_slist *headers = NULL;
			headers = curl_slist_append(headers, "x-rapidapi-host: aerodatabox.p.rapidapi.com");
			headers = curl_slist_append(headers, "x-rapidapi-key: XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX");
			curl_easy_setopt(hnd, CURLOPT_HTTPHEADER, headers);
			
			//credertial end..
			long httpCode(0);
			unique_ptr<std::string> httpData(new std::string());

			curl_easy_setopt(hnd, CURLOPT_WRITEFUNCTION, callback);

			curl_easy_setopt(hnd, CURLOPT_WRITEDATA, httpData.get());
				
			CURLcode ret = curl_easy_perform(hnd);
			//~ curl_easy_perform(hnd);
			curl_easy_getinfo(hnd, CURLINFO_RESPONSE_CODE, &httpCode);

			//~ std::cout<<httpCode<<'\n';
			if(httpCode==200)
			{
				Json::Value jsonData;
				Json::Reader jsonReader;
				if(jsonReader.parse(*httpData, jsonData))
				{
					//~ std::cout << jsonData.toStyledString() << std::endl;
					for(Json::Value::ArrayIndex i=0;i!=jsonData["departures"].size();++i)
					{
						//~ std::cout<<jsonData["arrivals"][i]["departure"]["airport"]["name"]<<' ';
						if(jsonData["departures"][i]["arrival"]["airport"]["icao"]!=Json::Value::null && jsonData["departures"][i]["arrival"]["airport"]["name"]!=Json::Value::null)
						{
							Json::FastWriter fastWriter;
							Json::Value temp = jsonData["departures"][i]["arrival"]["airport"]["icao"];
							string v = fastWriter.write(temp);
							v = v.substr(1,4);
							string endT = fastWriter.write(jsonData["departures"][i]["arrival"]["scheduledTimeUtc"]);
							string startT = fastWriter.write(jsonData["departures"][i]["departure"]["scheduledTimeUtc"]);
							endT = endT.substr(1,16);
							startT = startT.substr(1,16);
							endT[10] = startT[10] = 'T';
							long long time_taken = calcTime(startT,endT);
							//~ cout<<jsonData["arrivals"][i]["departure"]["airport"]["name"]<<' '<<startT<<' '<<endT<<' '<<time_taken<<u<<' '<<v<<'\n';
							if(startT>mpend_time[u] && (tm.find(v)==tm.end() || tm[v]>tm[u]+time_taken) )
							{
								//~ cout<<"airport: "<<v<<'\n';
								tm[v] = tm[u]+time_taken;
								mpend_time[v] = endT;
								parent[v] = make_pair(u,make_pair(startT,endT));
								//~ parent[u].second = startT;
								pq.push(make_pair(tm[v],v));
								if(v==dst)
								{
									TotalTimeTaken = tm[v];
									return parent;
								}
							}
						}
						//~ if(jsonData["departures"][i]["arrival"]["scheduledTimeUtc"]!=Json::Value::null)
						//~ std::cout<<jsonData["departures"][i]["arrival"]["scheduledTimeUtc"]<<' '<<<<'\n';
					}
				}
			}
			//end of dijkstra's
	}
	//end of while
	return parent;
}

void printMinTimePath(map<string,pair<string,pair<string,string>>> &parent,string dst,string prev,string sc_time,string sc_timeE)
{
	if(parent.find(dst)==parent.end())
	{
		cout<<"NO path found...:(<br>";
		return;
	}
	if(parent[dst].first=="none")
	{
		//~ cout<<dst<<' ';
		return;
	}
	printMinTimePath(parent,parent[dst].first,dst,parent[dst].second.first,parent[dst].second.second);
	cout<<parent[dst].first<<' '<<dst<<' '<<parent[dst].second.first<<' '<<parent[dst].second.second<<"<br>";
}

int main(int argc,char** argv)
{
	//~ cout<<"Enter the src city icao code\n";
	string src,dst;
	//~ cin>>src;
	if(argc<3)
	{
		cout<<"Wrong parameters\n";
		return 0;
	}
	src = argv[1];
	//~ cout<<"Enter the dest city icao code\n";
	//~ cin>>dst;
	dst = argv[2];
	map<string,pair<string,pair<string,string>>> parent = min_route(src,dst);
	//~ cout<<parent.size()<<'\n';
	//~ for(pair<string,string> p:parent)
	//~ cout<<p.first<<' '<<p.second<<'\n';
	printMinTimePath(parent,dst,dst,"0","0");
	int hrs = TotalTimeTaken/3600;
	int min = (TotalTimeTaken%3600)/60;
	int sc  = ((TotalTimeTaken%3600)%60);
	cout<<"\nTOTAL TIME TAKEN: "<<hrs<<"hrs "<<min<<"mins "<<sc<<"secs<br>";
}
