#include <stdlib.h> 
#include <stdio.h> 
#include <string.h> 
#include <curl/curl.h>

#define ANZSCOURL "https://www.anzscosearch.com/wp-admin/admin-ajax.php?action=get_data_anzsco&fromURL=true&anzsco=261312"
#define PROXY NULL

struct mystring {
	size_t size;
	char *string;
};

size_t write_callback(char *ptr, size_t size, size_t nmemb, void *userdata); 
void check_return_code(CURLcode *ret); void parse_code(char *buffer); 
void parse(char *buffer, char *sdel, char *edel,struct mystring *string);

struct Userdata {
  char *buffer;
  size_t size;
};



int main (int argc, char **argv) {
  CURL *curl;
  CURLcode ret;
  struct Userdata user;
  //Initialize Userdata
  user.buffer = malloc(1);
  user.size = 0;
  //Start a libcurl easy session
  curl = curl_easy_init();
  if(curl) {

    //Set Proxy
    if(PROXY != NULL) {
      ret = curl_easy_setopt(curl, CURLOPT_PROXY, PROXY);
      check_return_code(&ret);
    }

    //Set URL
    ret = curl_easy_setopt(curl, CURLOPT_URL, ANZSCOURL);
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
    parse_code(user.buffer);
    curl_easy_cleanup(curl);

  }else{
    fprintf(stderr, "Error: %s\n","Error initializing curl\n");
    return EXIT_FAILURE;
  }
  free(user.buffer);
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
  user->buffer = realloc(user->buffer, user->size + realsize + 1);
  if(user->buffer == NULL) {
    printf("Not enough memory\n");
    return 0;
  }

  memcpy(&(user->buffer[user->size]), ptr, realsize);
  user->size += realsize;
  user->buffer[user->size] = 0;

  return realsize;
}

char *search(char *buffer, char *start, char *end, char *str) {

  size_t strln;

  char *sdel = strstr(buffer, start);
  if(sdel == NULL)
	return NULL;
	
  char *edel = strstr(sdel, end);
  if(edel == NULL)
	return NULL;
	
  strln = edel - sdel + 1;
  int offset = strlen(start);
  sdel+=offset;
  strncpy(str,sdel,strln - offset - 1);
  str[strln] = '\0';
  
  return str;

}

void check_state(char*buffer, char *state) {

  char txt[50] = "";
  struct mystring thestring;
  
  thestring.size = 1;
  thestring.string = (char *) malloc(1);
  
  strcat(txt,state);
  strcat(txt,"</td>");

  parse(buffer,txt,"</td>",&thestring);
  parse(thestring.string,"alt='","'",&thestring);
  printf("%s: %s\n",state,thestring.string);
  
  free(thestring.string);

}

void parse_code(char *buffer) { 

//<td>Sydney</td><td class='centre'><img src='https://www.anzscosearch.com/wp-content/uploads/2013/09/no16.png' alt='Occupation NOT Eligible' title='Occupation NOT Eligible'></td>

  check_state(buffer,"Adelaide");
  check_state(buffer,"Canberra");
  check_state(buffer,"Sydney");
  check_state(buffer,"Darwin");
  check_state(buffer,"Brisbane");
  check_state(buffer,"Hobart");
  check_state(buffer,"Melbourne");
  check_state(buffer,"Perth");
  
}

void parse(char *buffer, char *sdel, char *edel,struct mystring *string) {
	
	size_t sdel_len = strlen(sdel);
	char *start = strstr(buffer,sdel);
	size_t offset_start = (size_t) start - (size_t) buffer + sdel_len;
	
	buffer += offset_start;

	char *nd = strstr(buffer,edel);
	
	size_t len = (size_t)nd - (size_t)buffer;
	
	string->string = realloc(string->string, len+1);
	if(string->string != NULL) {
		string->size = len + 1;
		strncpy(string->string,buffer,len);
		string->string[len] = '\0';
	}
	
	//strncpy(result,buffer,len);
	//result[len] = '\0';
	
}
