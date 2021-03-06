#ifndef __MNSER_HTTP_CONNECTION_H__
#define __MNSER_HTTP_CONNECTION_H__

#include <memory>
#include <list>
#include <atomic>

#include "mutex.h"
#include "uri.h"
#include "http.h"
#include "socket_stream.h"

namespace MNSER {
namespace http {

// HTTP响应结果
struct HttpResult {
    typedef std::shared_ptr<HttpResult> ptr;
    // 错误码定义
    enum class Error {
        OK 						= 0,        // 正常
        INVALID_URL 			= 1,        // 非法URL
        INVALID_HOST 			= 2,        // 无法解析HOST
        CONNECT_FAIL 			= 3,        // 连接失败
		SEND_CLOSE_BY_PEER 		= 4,     	// 连接被对端关闭
		SEND_SOCKET_ERROR 		= 5,      	// 发送请求产生Socket错误
        TIMEOUT 				= 6,        // 超时
        CREATE_SOCKET_ERROR 	= 7,    	// 创建Socket失败
        POOL_GET_CONNECTION 	= 8,    	// 从连接池中取连接失败
        POOL_INVALID_CONNECTION = 9,		// 无效的连接
    };

    HttpResult(int _result
               ,HttpResponse::ptr _response
               ,const std::string& _error)
        :result(_result)
        ,response(_response)
        ,error(_error) {}


    int result;    							// 错误码
    HttpResponse::ptr response;    			// HTTP响应结构体
    std::string error;    					// 错误描述

    std::string toString() const;
};

class HttpConnectionPool;
// HTTP客户端类
class HttpConnection : public SocketStream {
friend class HttpConnectionPool;
public:
    typedef std::shared_ptr<HttpConnection> ptr;

    // 发送HTTP的GET请求i, url 请求的url, timeout_ms 超时时间(毫秒), headers HTTP请求头部参数, body 请求消息体
    // 返回HTTP结果结构体
    static HttpResult::ptr DoGet(const std::string& url
                            , uint64_t timeout_ms
                            , const std::map<std::string, std::string>& headers = {}
                            , const std::string& body = "");

    // 发送HTTP的GET请求, uri URI结构体, timeout_ms 超时时间(毫秒), headers HTTP请求头部参数
	// body 请求消息体, 返回HTTP结果结构体
    static HttpResult::ptr DoGet(Uri::ptr uri
                            , uint64_t timeout_ms
                            , const std::map<std::string, std::string>& headers = {}
                            , const std::string& body = "");

    // 发送HTTP的POST请求, url 请求的url, timeout_ms 超时时间(毫秒), headers HTTP请求头部参数, body 请求消息体
    // 返回HTTP结果结构体
    static HttpResult::ptr DoPost(const std::string& url
                            , uint64_t timeout_ms
                            , const std::map<std::string, std::string>& headers = {}
                            , const std::string& body = "");

    // 发送HTTP的POST请求, uri URI结构体, timeout_ms 超时时间(毫秒), headers HTTP请求头部参数, body 请求消息体
    // 返回HTTP结果结构体
    static HttpResult::ptr DoPost(Uri::ptr uri
                            , uint64_t timeout_ms
                            , const std::map<std::string, std::string>& headers = {}
                            , const std::string& body = "");

    // 发送HTTP请求, method 请求类型, uri 请求的url, timeout_ms 超时时间(毫秒), headers HTTP请求头部参数, body 请求消息体
    // 返回HTTP结果结构体
    static HttpResult::ptr DoRequest(HttpMethod method
                            , const std::string& url
                            , uint64_t timeout_ms
                            , const std::map<std::string, std::string>& headers = {}
                            , const std::string& body = "");

    // 发送HTTP请求, method 请求类型, uri URI结构体, timeout_ms 超时时间(毫秒), headers HTTP请求头部参数, body 请求消息体
    // 返回HTTP结果结构体
    static HttpResult::ptr DoRequest(HttpMethod method
                            , Uri::ptr uri
                            , uint64_t timeout_ms
                            , const std::map<std::string, std::string>& headers = {}
                            , const std::string& body = "");

    // 发送HTTP请求, req 请求结构体, uri URI结构体, timeout_ms 超时时间(毫秒)
    // 返回HTTP结果结构体
    static HttpResult::ptr DoRequest(HttpRequest::ptr req
                            , Uri::ptr uri
                            , uint64_t timeout_ms);

    HttpConnection(Socket::ptr sock, bool owner = true);

    ~HttpConnection();

    // 接收HTTP响应
    HttpResponse::ptr recvResponse();

    // 发送HTTP请求, req HTTP请求结构
    int sendRequest(HttpRequest::ptr req);

private:
    uint64_t m_createTime = 0;  
    uint64_t m_request = 0;
};

class HttpConnectionPool {
public:
    typedef std::shared_ptr<HttpConnectionPool> ptr;
    typedef Mutex MutexType;

    static HttpConnectionPool::ptr Create(const std::string& uri
                                   ,const std::string& vhost
                                   ,uint32_t max_size
                                   ,uint32_t max_alive_time
                                   ,uint32_t max_request);

    HttpConnectionPool(const std::string& host
                       , const std::string& vhost
                       , uint32_t port
                       , bool is_https
                       , uint32_t max_size
                       , uint32_t max_alive_time
                       , uint32_t max_request);

    HttpConnection::ptr getConnection();


    // 发送HTTP的GET请求, url 请求的url, timeout_ms 超时时间(毫秒), headers HTTP请求头部参数, body 请求消息体
    // 返回HTTP结果结构体
    HttpResult::ptr doGet(const std::string& url
                          , uint64_t timeout_ms
                          , const std::map<std::string, std::string>& headers = {}
                          , const std::string& body = "");

    // 发送HTTP的GET请求, uri URI结构体, timeout_ms 超时时间(毫秒), headers HTTP请求头部参数, body 请求消息体
    // 返回HTTP结果结构体
    HttpResult::ptr doGet(Uri::ptr uri
                           , uint64_t timeout_ms
                           , const std::map<std::string, std::string>& headers = {}
                           , const std::string& body = "");

    // 发送HTTP的POST请求, url 请求的url, timeout_ms 超时时间(毫秒), headers HTTP请求头部参数, body 请求消息体
    // 返回HTTP结果结构体
    HttpResult::ptr doPost(const std::string& url
                           , uint64_t timeout_ms
                           , const std::map<std::string, std::string>& headers = {}
                           , const std::string& body = "");

    // 发送HTTP的POST请求, uri URI结构体, timeout_ms 超时时间(毫秒), headers HTTP请求头部参数, body 请求消息体
    // 返回HTTP结果结构体
    HttpResult::ptr doPost(Uri::ptr uri
                           , uint64_t timeout_ms
                           , const std::map<std::string, std::string>& headers = {}
                           , const std::string& body = "");

    // 发送HTTP请求, method 请求类型, uri 请求的url, timeout_ms 超时时间(毫秒), headers HTTP请求头部参数, body 请求消息体
    // 返回HTTP结果结构体
    HttpResult::ptr doRequest(HttpMethod method
                            , const std::string& url
                            , uint64_t timeout_ms
                            , const std::map<std::string, std::string>& headers = {}
                            , const std::string& body = "");

    // 发送HTTP请求, method 请求类型, uri URI结构体, timeout_ms 超时时间(毫秒), headers HTTP请求头部参数, body 请求消息体
    // 返回HTTP结果结构体
    HttpResult::ptr doRequest(HttpMethod method
                            , Uri::ptr uri
                            , uint64_t timeout_ms
                            , const std::map<std::string, std::string>& headers = {}
                            , const std::string& body = "");

    // 发送HTTP请求, req 请求结构体, timeout_ms 超时时间(毫秒)
    // 返回HTTP结果结构体
    HttpResult::ptr doRequest(HttpRequest::ptr req
                            , uint64_t timeout_ms);

private:
    static void ReleasePtr(HttpConnection* ptr, HttpConnectionPool* pool);

private:
    std::string m_host;
    std::string m_vhost;
    uint32_t m_port;
    uint32_t m_maxSize;
    uint32_t m_maxAliveTime;
    uint32_t m_maxRequest;
    bool m_isHttps;

    MutexType m_mutex;
    std::list<HttpConnection*> m_conns;
    std::atomic<int32_t> m_total = {0};
};

}
}

#endif
