#include "socket.h"
#include "log.h"
#include "iomanager.h"
#include "address.h"

static MNSER::Logger::ptr g_looger = MS_LOG_ROOT();

void test_socket() {
    MNSER::IPAddress::ptr addr = MNSER::Address::LookupAnyIPAddress("www.baidu.com");
    if(addr) {
        MS_LOG_INFO(g_looger) << "get address: " << addr->toString();
    } else {
        MS_LOG_ERROR(g_looger) << "get address fail";
        return;
    }

    MNSER::Socket::ptr sock = MNSER::Socket::CreateTCP(addr);
    addr->setPort(80);
    MS_LOG_INFO(g_looger) << "addr=" << addr->toString();
    if(!sock->connect(addr)) {
        MS_LOG_ERROR(g_looger) << "connect " << addr->toString() << " fail";
        return;
    } else {
        MS_LOG_INFO(g_looger) << "connect " << addr->toString() << " connected";
    }

    const char buff[] = "GET / HTTP/1.0\r\n\r\n";
    int rt = sock->send(buff, sizeof(buff));
    if(rt <= 0) {
        MS_LOG_INFO(g_looger) << "send fail rt=" << rt;
        return;
    }

    std::string buffs;
    buffs.resize(4096);
    rt = sock->recv(&buffs[0], buffs.size());

    if(rt <= 0) {
        MS_LOG_INFO(g_looger) << "recv fail rt=" << rt;
        return;
    }

    buffs.resize(rt);
    MS_LOG_INFO(g_looger) << std::endl << std::endl << buffs << std::endl;
}

void test2() {
    MNSER::IPAddress::ptr addr = MNSER::Address::LookupAnyIPAddress("www.baidu.com:80");
    if(addr) {
        MS_LOG_INFO(g_looger) << "get address: " << addr->toString();
    } else {
        MS_LOG_ERROR(g_looger) << "get address fail";
        return;
    }

    MNSER::Socket::ptr sock = MNSER::Socket::CreateTCP(addr);
    if(!sock->connect(addr)) {
        MS_LOG_ERROR(g_looger) << "connect " << addr->toString() << " fail";
        return;
    } else {
        MS_LOG_INFO(g_looger) << "connect " << addr->toString() << " connected";
    }

    uint64_t ts = MNSER::GetCurrentUS();
    for(size_t i = 0; i < 10000000000ul; ++i) {
        if(int err = sock->getError()) {
            MS_LOG_INFO(g_looger) << "err=" << err << " errstr=" << strerror(err);
            break;
        }

        //struct tcp_info tcp_info;
        //if(!sock->getOption(IPPROTO_TCP, TCP_INFO, tcp_info)) {
        //    MS_LOG_INFO(g_looger) << "err";
        //    break;
        //}
        //if(tcp_info.tcpi_state != TCP_ESTABLISHED) {
        //    MS_LOG_INFO(g_looger)
        //            << " state=" << (int)tcp_info.tcpi_state;
        //    break;
        //}
        static int batch = 10000000;
        if(i && (i % batch) == 0) {
            uint64_t ts2 = MNSER::GetCurrentUS();
            MS_LOG_INFO(g_looger) << "i=" << i << " used: " << ((ts2 - ts) * 1.0 / batch) << " us";
            ts = ts2;
        }
    }
}
int main(int argc, char** argv) {
	MNSER::IOManager iom;	
	iom.schedule(&test_socket);
	//iom.schedule(&test2);
	return 0;
}
