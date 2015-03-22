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
  //user.buffer = malloc(1);
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
    //parse_code(user.buffer);
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
  //user->buffer = realloc(user->buffer, user->size + realsize + 1);
  if(user->buffer == NULL) {
    printf("Not enough memory\n");
    return 0;
  }

  memcpy(&(user->buffer[user->size]), ptr, realsize);
  user->size += realsize;
  user->buffer[user->size] = 0;

  return realsize;
}