#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <cjson/cJSON.h>
#include <stdlib.h>
const char *mpv_socket="/tmp/mpvsocket";
void die(const char *msg){
    fprintf(stderr,"SHIT! %s \n",msg);
    abort();
}
void ifndie(int cond,const char *msg){
    if(!cond)die(msg);
} 
const int alloc_step = 128;
/*
char * recvline(int fd){
    int len = 0;
    int size =0;
    while(1){
	char tbuf[alloc_step]={0};	
	
	for(int i=0;i<alloc_step;i++){
	    recv(fd,
	    if(tbuf == '\n')

	}
	buf = realloc(buf,size); 

    }

}*/
int main(int argc, const char *argv[]){
    int fd = socket(AF_UNIX,SOCK_STREAM,0);
    struct sockaddr_un addr;
    memset(&addr,0,sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path,mpv_socket,sizeof(addr.sun_path)-1);
    int err = connect(fd,(struct sockaddr*)&addr,sizeof(addr));    
    if(err){
	die("Socket failed to connect!");
    }
    if(argc<=1){
	die("Not enough args!");
    }
    cJSON *request = cJSON_CreateObject();
    cJSON *cmd = cJSON_CreateStringArray(argv+1,argc-1);
    cJSON_AddItemToObject(request,"command", cmd);
    char *request_str = cJSON_PrintUnformatted(request);
    cJSON_Delete(request);
    fprintf(stderr,"Cmd: %s\n",request_str);
    
    if(send(fd,request_str,strlen(request_str),0)==-1 || send (fd,"\n",1,0)==-1){
	die("Send failed!");
    
    }
    char buf[1024]={0};
    int len = recv(fd,buf,sizeof(buf),0);
    fprintf(stderr,"Resp:%s\n",buf);
    cJSON *resp = cJSON_ParseWithLength(buf,len);
    ifndie(cJSON_IsObject(resp),"Invalid json structure!");
    cJSON *data = cJSON_GetObjectItemCaseSensitive(resp,"data");
    ifndie(data!=NULL,"data is NULL!");
    printf("%s\n",cJSON_IsString(data)?data->valuestring:cJSON_PrintUnformatted(data));
    close(fd);
    return 0;
}
