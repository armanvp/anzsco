#include <stdlib.h> 
#include <iostream> 
#include <string> 
#include <curl/curl.h>

using namespace std;

#define ANZSCOURL "https://www.anzscosearch.com/wp-admin/admin-ajax.php?action=get_data_anzsco&fromURL=true&anzsco="
#define PROXY NULL
#define DEF_ANZSCO "261312"

struct mystring {
	size_t size;
	char *string;
};

size_t write_callback(char *ptr, size_t size, size_t nmemb, void *userdata); 
void check_return_code(CURLcode *ret); void parse_code(char *buffer); 
void parse(char *buffer, char *sdel, char *edel,struct mystring *string);
string check_status(string raw, string state);
void check(string buff, string *anzsco);

struct Userdata {
  string buffer;
  size_t size;
};

int main (int argc, char **argv) {
  string anzsco;
  string url;

  CURL *curl;
  CURLcode ret;
  struct Userdata user;
  //Initialize Userdata
  user.size = 0;

  //Get parameters
  if(argc == 2) {
    anzsco = argv[1];
    for(int i=0; i<strlen(argv[1]); i++) {
      if(!isdigit(argv[1][i])) {
        anzsco = "";
        break;
      }
    }
  }

  if(!anzsco.length())
    anzsco = DEF_ANZSCO;

  url = ANZSCOURL + anzsco;

  //Start a libcurl easy session
  curl = curl_easy_init();
  if(curl) {

    //Set Proxy
    if(PROXY != NULL) {
      ret = curl_easy_setopt(curl, CURLOPT_PROXY, PROXY);
      check_return_code(&ret);
    }

    //Set URL
    ret = curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    check_return_code(&ret);

    //Set Write Callback Function
    ret = curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
    check_return_code(&ret);

    //Set UserData for Write Callback Function
    ret = curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&user);
    check_return_code(&ret);

    //Perform request
    ret = curl_easy_perform(curl);
    check_return_code(&ret);

    //Parse Data
    check(user.buffer,&anzsco);
	
    curl_easy_cleanup(curl);

  }else{
    fprintf(stderr, "Error: %s\n","Error initializing curl\n");
    return EXIT_FAILURE;
  }
  
  return EXIT_SUCCESS;
}
/*
   check_return_code - checks CURL return code
   for errors and reports it back */ 
void check_return_code(CURLcode *ret) {
  if(*ret != CURLE_OK)
    fprintf(stderr, "Error: %s", curl_easy_strerror(*ret));
}
/*
   write_callback - callback function called
   by curl to process received data */ 
size_t write_callback(char *ptr, size_t size, size_t nmemb, void *userdata) {

  size_t realsize = size * nmemb;
  struct Userdata *user = (struct Userdata *) userdata;

  user->buffer = user->buffer + ptr;

  return realsize;
}

string check_status(string raw, string state) {

	string ret;
	size_t pos, pos_end, pos_off;

	/* Find initial identifier */
	string needle = "<td>" + state + "</td>";
	pos = raw.find(needle);
	
	/* Identifier not found */
	if(pos == string::npos) {
		ret = "Unable to parse " + state;
		return ret;
	}
	
	/* Find status */
	needle = "alt='";
	pos_off = needle.size();
	pos = raw.find(needle,pos);
	
	/* Identifier not found */
	if(pos == string::npos) {
		ret = "Unable to parse " + state;
		return ret;
	}
	
	/* Find status end */
	needle = "' ";
	pos_end = raw.find(needle,pos+pos_off);
	
	return state + " : " + raw.substr(pos+pos_off,pos_end-pos-pos_off);
	
}

void check(string buff, string *anzsco) {

	cout<<"ANZSCO Status for : "<<*anzsco<<endl<<endl;
	cout<<check_status(buff, "Australian Capital Territory")<<endl;
	cout<<check_status(buff, "New South Wales")<<endl;
	cout<<check_status(buff, "Northern Territory")<<endl;
	cout<<check_status(buff, "Queensland")<<endl;
	cout<<check_status(buff, "South Australia")<<endl;
	cout<<check_status(buff, "Tasmania")<<endl;
	cout<<check_status(buff, "Victoria")<<endl;
	cout<<check_status(buff, "Western Australia")<<endl;
	
}
