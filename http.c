#include "http.h"
#include "shopfloor.h"

char* postDataToServer(const char *hostname, const char *url);
int parseStandardResponse(char *content);
int getElement(const char *content, char *tag, char *value);
int parseUploadTestItemResultResponse(char *content);
int parseUploadFinalTestResultResponse(char *content);
int parseRoutingResponse(char *content, s_routing_respone_parameter_list *result);
int parseStandardResponse(char *content);

char* host_to_ip(const char *hostname)
{
    struct hostent *host_entry;
    host_entry = gethostbyname(hostname);
    if (host_entry)
    {
        return inet_ntoa(*(struct in_addr *)*host_entry->h_addr_list);
    } else return NULL;
}

bool http_create_socket(SOCKET *sock, SOCKADDR_IN *sin, const char *ip)
{
#ifdef WIN32
    u_long mode = 0; //0:block
#endif
    *sock = socket(AF_INET, SOCK_STREAM, 0);      //init the socket

    sin->sin_addr.s_addr = inet_addr(ip);         //init the socket address on ip / port / network
    sin->sin_port = htons(PORT);
    sin->sin_family = AF_INET;

    if (connect(*sock, (SOCKADDR *)sin, sizeof(*sin)) == SOCKET_ERROR) return false;

#ifdef linux
    fcntl(*sock, F_SETFL, O_NONBLOCK);
#elif defined WIN32
    ioctlsocket(*sock, FIONBIO, &mode);
#endif

    return TRUE;
}

char* http_request(SOCKET sock, const char *hostname, const char *url)
{
    char buf[BUFSIZ];
    char *result = NULL;
    int len = 0;

    send(sock, url, strlen(url), 0);
    // timeout
    struct timeval tv;
    tv.tv_sec = TIMEOUT_SEC;
    tv.tv_usec = TIMEOUT_MSEC;

    //receive data from server
    len = recv(sock, buf, sizeof(buf), 0);
    if (len == 0)
    {
        return NULL;
    } else if (len == SOCKET_ERROR)
    {
        printf("recv() Failed: %d \n", WSAGetLastError());
        return NULL;
    } else{
        printf("recv() data from sever: %s \n", buf);
    }
    result = malloc(sizeof(buf));

    if (NULL != result)
    {
        strcpy(result, buf);
    }
    return result;
}

char* http_header_strip(char *content)
{
    return strstr(content, "\r\n\r\n") + 4;
}

//3. ROUTING CHECK
int QueryRoutingParameterList(s_routing_respone_parameter_list *result)
{
    char url[BUFSIZ] = { 0 };
    char *content = NULL;
    int res = 0;
    s_query_routing_parameter_list routing_para = { 0 };

#ifdef SHOP_DEBUG
    strcpy(url, "POST ");
    strcat(url, "/shop/MyServlet?cmd=QUERY_ROUTING&wip_no=BIR0000000000000&station_type=BB1 ");
    strcat(url, HTTP_VERSION);
    strcat(url, "\r\n");
    strcat(url, "Host: localhost:8088\r\n");
    strcat(url, "Connection: Keep-Alive\r\n");
    strcat(url, "\r\n");
    content = postDataToServer("127.0.0.1", url);
#else
    sprintf(url, IP_CONNECT_STRING, IP_ADDRESS, routing_para.cmd, routing_para.wip_no, routing_para.station_type);
    content = postDataToServer(IP_ADDRESS, url);
#endif
    res = parseRoutingResponse(content,result);
    return res;
}
//http://10.195.226.56/MLB/AddGK?cmd=QUERY_ROUTING&wip_no=BIR0000000000000&station_type=BB1
int parseRoutingResponse(char *content, s_routing_respone_parameter_list *result)
{
    if (NULL == content || NULL == result)
    {
        return -1;
    }
    getElement(content,"cmd",result->cmd);
    getElement(content,"wip_no",result->wip_no);
    getElement(content,"permission",result->permission);
    getElement(content,"sfc_error_msg",result->sfc_error_msg);
    getElement(content,"sw_version_check",result->sw_version_check);
    getElement(content,"rf_band_id",result->rf_band_id);
    getElement(content,"imei",result->imei);
    getElement(content,"imei2",result->imei2);
    getElement(content,"meid",result->meid);
    getElement(content,"esn",result->esn);
    getElement(content,"skuid",result->skuid);
    getElement(content,"bt_addr",result->bt_addr);
    getElement(content,"wifi_mac_addr",result->wifi_mac_addr);
    getElement(content,"sim_lock_nkey",result->sim_lock_nkey);
    getElement(content,"sim_lock_nskey",result->sim_lock_nskey);
    getElement(content,"chip_version",result->chip_version);
    getElement(content,"emmc_version",result->emmc_version);
    getElement(content,"tp_version",result->tp_version);
    getElement(content,"camera_version",result->camera_version);
    getElement(content,"sim_category",result->sim_category);
    getElement(content,"tool_version",result->tool_version);
    getElement(content,"rdl_fw_path",result->rdl_fw_path);
    getElement(content,"customer_product_id",result->customer_product_id);
    getElement(content,"customer_product_id2",result->customer_product_id2);
    getElement(content,"customer_sw_id",result->customer_sw_id);
    getElement(content,"battery_pn",result->battery_pn);
    getElement(content,"rdcode_with_skuid",result->rdcode_with_skuid);
    return 0;
    //TODO: Parsing response
}

//4. Upload TestItem Result
int UploadTestItemResult(s_test_item_result_parameter_list *result)
{
    if (NULL == result)
    {
        return -1;
    }
    char url[BUFSIZ] = { 0 };
    char *content = NULL;
    int res = 0;
//  http://10.195.226.56/MLB/AddGK?cmd=ADD_TEST_ITEM&wip_no=BIR0000000000000& test_station_name=MT
//&station_code=MT001&test_item_name=BT_TX_BDR& test_spec_name=Power_Average_dBm
//&test_value=16.2&upper_bound=13.0&low_bound=1.0&test_result=1&symptom_code=BT
//_OUTPUT_POWER_FAIL
    strcat(url, "http: //");
    strcat(url, "");
    strcat(url, IP_ADDRESS);
    strcat(url, "/MLB/AddGK?cmd=ADD_TEST_ITEM");

    strcat(url, "&wip_no=");
    strcat(url, result->wip_no);

    strcat(url, "&test_station_name=");
    strcat(url, result->test_station_name);

    strcat(url, "&station_code=");
    strcat(url, result->station_code);

    strcat(url, "&test_item_name=");
    strcat(url, result->test_item_name);

    strcat(url, "&=test_spec_name");
    strcat(url, result->test_spec_name);

    strcat(url, "&=test_value");
    strcat(url, result->test_value);

    strcat(url, "&=upper_bound");
    strcat(url, result->upper_bound);

    strcat(url, "&=low_bound");
    strcat(url, result->low_bound);

    strcat(url, "&=test_result");
    strcat(url, result->test_result);

    strcat(url, "&=symptom_code");
    strcat(url, result->symptom_code);

    content =  postDataToServer(IP_ADDRESS, url);
    //TODO: Parsing response
    res = parseUploadTestItemResultResponse(content);
    if (NULL != content)
    {
        free(content);
    }
    return res;
}

int parseUploadTestItemResultResponse(char *content)
{
    return parseStandardResponse(content);
}
//5. Upload Final Test Result
int UploadFinalTestResult(s_upload_final_test_result *result)
{
    char url[BUFSIZ] = { 0 };
    char *content = NULL;
    int res = 0;
    if (NULL == result)
    {
        return -1;
    }
//http://10.195.226.56/MLB/AddGK?cmd=ADD_TEST
// &wip_no=BIR0000000000000&station_type=BB1&station_code=FXZZ_K06-2FT-01A_6_BB1
// &test_machine_id=PC-0001
// &start_time=2012-12-05 13:00:07&stop_time=2012-12-0513:00:11
//&test_result=0&symptom_code=&symptom_msg=&bt_addr=
//&wifi_mac_addr=&sim_lock_nkey&sim_lock_nskey=&shi pping_os_image_name=VKY-3420-0-15CN-A01
//&test_item_name=BT_TX_BDR&test_spec_name=Power_Average_dBm
//&test_v alue=6.2&upper_bound=13.0&low_bound=1.0
//&symptom_code=BT_OUTPUT_POWER_FAIL&diag_version=DBU-0390-0-000F-A01

    strcat(url, "http: //");
    strcat(url, "");

    strcat(url, IP_ADDRESS);
    strcat(url, "/MLB/AddGK?cmd=ADD_TEST");

    strcat(url, "&=wip_no");
    strcat(url, result->wip_no);

    strcat(url, "&=station_type");
    strcat(url, result->station_type);

    strcat(url, "&=station_code");
    strcat(url, result->station_code);

    strcat(url, "&=test_machine_id");
    strcat(url, result->test_machine_id);

    strcat(url, "&=start_time");
    strcat(url, result->start_time);

    strcat(url, "&=stop_time");
    strcat(url, result->stop_time);

    strcat(url, "&=test_result");
    strcat(url, result->test_result);

    strcat(url, "&=symptom_code");
    strcat(url, result->symptom_code);

    strcat(url, "&=symptom_msg");
    strcat(url, result->symptom_msg);

    strcat(url, "&=bt_addr");
    strcat(url, result->bt_addr);

    strcat(url, "&=wifi_mac_addr");
    strcat(url, result->wifi_mac_addr);

    strcat(url, "&=sim_lock_nkey");
    strcat(url, result->sim_lock_nkey);

    strcat(url, "&=sim_lock_nskey");
    strcat(url, result->sim_lock_nskey);

    strcat(url, "&=shipping_os_image_name");
    strcat(url, result->shipping_os_image_name);

    strcat(url, "&=test_item_name");
    strcat(url, result->test_item_name);

    strcat(url, "&=test_spec_name");
    strcat(url, result->test_spec_name);

    strcat(url, "&=test_value");
    strcat(url, result->test_value);

    strcat(url, "&=upper_bound");
    strcat(url, result->upper_bound);

    strcat(url, "&=low_bound");
    strcat(url, result->low_bound);

    strcat(url, "&=symptom_code");
    strcat(url, result->symptom_code);

    strcat(url, "&=diag_version");
    strcat(url, result->diag_version);

    content =  postDataToServer(IP_ADDRESS, url);

    res = parseUploadFinalTestResultResponse(content);
    if (NULL != content)
    {
        free(content);
    }
    return res;
}
int parseUploadFinalTestResultResponse(char *content){
    if (NULL == content)
    {
        return -1;
    }
    return parseStandardResponse(content);
}
//common function
char* postDataToServer(const char *hostname, const char *url)
{
    char *content = NULL;
    WSADATA WSAData;
    SOCKET sock;
    SOCKADDR_IN sin;

    if (NULL == url || strlen(url) == 0)
    {
        return NULL;
    }
    int erreur = WSAStartup(MAKEWORD(2, 2), &WSAData);
    if (erreur)
    {
        puts("Cannot start WSA");
        exit(EXIT_FAILURE);
    }

    if (!http_create_socket(&sock, &sin, hostname))
    {
        puts("Cannot create http socket");
        return NULL;
    }
#ifdef SHOP_DEBUG
    printf("Connected to %s/%s (%s:%d)\n", hostname, url, inet_ntoa(sin.sin_addr), htons(sin.sin_port));
#endif
    content = http_request(sock, NULL, url);

    if (content == NULL)
    {
        return NULL;
    }

    //char *content_core = http_header_strip(content);

    //free(content);
    closesocket(sock);

#if defined (WIN32)
    WSACleanup();
#endif
#ifdef SHOP_DEBUG
    puts(content);
#endif
    return content;
}
int getElement(const char *content, char *tag, char *value)
{
    char buf[100] = { 0 };
    char *subString = NULL;
    char tempChar = 0;
    int i = 0;
    if (NULL == tag || NULL == content || NULL == value)
    {
        return -1;
    }
    if ((subString = strstr(content, tag)) == NULL)
    {
        return -1;
    }
    subString += strlen(tag) + strlen("="); //include '='
    tempChar = *subString;
    while (tempChar != '\n'
           && tempChar != '\0'
           && tempChar != '&')
    {
        buf[i] = tempChar;
        tempChar = *(++subString);
        i++;
    }
    strcpy(value, buf);
    return 0;
}

int parseStandardResponse(char *content)
{
    if (NULL == content)
    {
        return -1;
    }
    if (strstr(content, RESPONSE_OK) != NULL)
    {
        return 0;
    } else if (strstr(content, RESPONSE_ERROR) != NULL)
    {
        return 1;
    } else if (strstr(content, RESPONSE_FATAL_ERROR) != NULL)
    {
        return 2;
    } else
    {
        return -1;
    }
}

int main()
{
#ifdef SHOP_DEBUG
    int i = 0;
    printf("hello world\n");
    s_routing_respone_parameter_list routingParameter={0};
    QueryRoutingParameterList(&routingParameter);
    scanf("%d", &i);
#endif
    return 0;
}
