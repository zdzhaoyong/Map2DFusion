#include <iostream>
#include <unistd.h>

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <base/Svar/Svar_Inc.h>
#include <base/debug/debug_config.h>
#include <network/MessagePassing.h>
#include <base/Svar/Scommand.h>

using namespace std;
using namespace pi;


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

void test_internetCommand(void *ptr,string command,string parament)
{
    cout<<"Entered the test internet_Command.\n";
    cout<<"parament="<<parament<<endl;
    RMessagePassing* mp=(RMessagePassing*)ptr;
    mp->sendString(parament,"I have received your command "+command);
}

void test_Scommand()
{
    RMessagePassing mp;
    string my_name=svar.GetString("Node.name","Master");
    Scommand::instance().RegisterCommand("test_internetCommand",test_internetCommand,&mp);
    if(0!=mp.begin(my_name))
    {
        cout<<"Initial failed.\n";
        return;
    }
    if("Master"==my_name)
    {
        while(mp.getAlive())
        {
            string str;
            RMP_Node* node=mp.recvString(str);
            SvarWithType<RMP_Node*>::instance()["OtherNode"]=node;
            if(str.size())
            {
                cout<<"Received:"<<str<<endl;
//                mp.sendString(node->nodeName,"I have received the message");
            }
            sleep(1);
        }
    }
    else
    {
        while(mp.getAlive())
        {

            string str;
            RMP_Node* node=mp.recvString(str);
            if(str.size())
            {
                cout<<"Received:"<<str<<endl;
                mp.sendString(node->nodeName,"I have received the message.");
            }
            static string command="test_internetCommand "+my_name;
            mp.sendString("Master",command,true);
            sleep(1);
        }
    }

}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

int test_server()
{
    int         imsg=0;
    string      msg;
    RSocket     server;
    int         ret;

    int         port = 30000;

    // parse input argument
    port=svar.GetInt("port", port);


    // start socket server
    std::cout << "running....\n";
    server.startServer(port);

    while(1)
    {
        RSocket new_socket;

        if( 0 != server.accept(new_socket) )
        {
            //dbg_pe("server.accept failed!");
            tm_sleep(4);
            continue;
        }

        RSocketAddress ca;
        new_socket.getClientAddress(ca);

        printf("\n");
        dbg_pt("accept a new connection! client: %s (%d)\n",
               ca.addr_str, ca.port);

        while(1)
        {
            ret = new_socket.recv(msg);
            usleep(100);

            if( ret < 0 ) break;
            else if( ret == 0 ) continue;

            new_socket.send(msg);

            printf("[%3d] %s\n", imsg++, msg.c_str());

            if( msg == "quit" ) {
                goto SERVER_QUIT;
            }
        }
        usleep(10000);
    }

SERVER_QUIT:
    server.close();

    return 0;
}


int test_client()
{
    string      msg, msg_s;
    ri64        pid;
    RSocket     client;
    int         ret1, ret2;

    string      addr;
    int         port;

    // parse input arguments
    addr = "127.0.0.1";
    port = 30000;

    addr=svar.GetString("client.addr", addr);
    port=svar.GetInt("client.port", port);


    // generate default message
    osa_get_pid(&pid);
    msg = fmt::sprintf("Test message! from pid=%d", pid);
    msg=svar.GetString("msg", msg);


    // begin socket
    if( 0 != client.startClient(addr, port) ) {
        dbg_pe("client.start_client failed!");
        return -1;
    }

    dbg_pt("client started!\n");

    for(int i=0; i<1000;i++) {
        msg_s = msg;

        ret1 = client.send(msg_s);
        ret2 = client.recv(msg_s);
        printf("receive message from sever: %s (%d), ret = %d, %d\n",
               msg.c_str(), msg.size(),
               ret1, ret2);
        usleep(500000);
    }

    client.close();

    return 0;
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

int test_messagepassing()
{
    RMessagePassing     mp;
    RMessage            *msg, msg_send;
    int                 msgType = 1, msgID = 0;
    StringArray         nl;


    string              my_name;

    int                 i;
    ru64                t0, t1, dt;

    int                 ret;

    my_name=svar.GetString("Node.name","Master");
    if( mp.begin(my_name) != 0 ) {
        return -1;
    }

    t0 = tm_get_millis();

    while(1) {
        // receive a message & show it
        msg = mp.recvMsg();
        if( msg != NULL ) {
            string      n1, n2;
            DateTime    t0, t1;
            double      dt;

            t1.setCurrentDateTime();
            msg->data.rewind();
            msg->data.read(n1);
            msg->data.read(n1);
            t0.fromStream(msg->data);
            dt = t1.diffTime(t0);

            printf("\nrecved message, msg.size = %d, dt = %f\n   ",
                   msg->data.size(), dt);
            msg->print();

            delete msg;
        }

        // send message to other nodes
        t1 = tm_get_millis();
        dt = t1 - t0;
        if( dt > 60 ) {
            mp.getNodeMap()->getNodeList(nl);

            printf("\n");
            mp.getNodeMap()->print();
            printf("\n");


            for(i=0; i<nl.size(); i++) {
                if( nl[i] != my_name ) {
                    msg_send.msgType = msgType;
                    msg_send.msgID   = msgID++;

                    DateTime tm;
                    tm.setCurrentDateTime();

                    msg_send.data.clear();
                    msg_send.data.write(my_name);
                    msg_send.data.write(nl[i]);
                    tm.toStream(msg_send.data);

                    printf("send message: %s -> %s, msg_size = %d\n",
                           my_name.c_str(), nl[i].c_str(), msg_send.data.size());

                    mp.sendMsg(nl[i], &msg_send);
                }
            }

            t0 = t1;
        }

        usleep(1000);
    }
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

// References:
//  http://ntrg.cs.tcd.ie/undergrad/4ba2/multicast/antony/example.html
//  https://en.wikipedia.org/wiki/Multicast_addresss
//
int test_udp_server(void)
{
    RSocket     socket;
    string      msg;

    if( 0 != socket.startServer("127.0.0.1", 10000, SOCKET_UDP) ) {
        dbg_pe("Can not start UDP multicast!");
        return -1;
    }

    int idx = 0;
    while(1) {
        msg = fmt::sprintf("Hello [%6d]", idx++);
        printf("send message: %s\n", msg.c_str());
        socket.send(msg);

        tm_sleep(500);
    }

    return 0;
}


int test_udp_client(void)
{
    RSocket     socket;
    string      msg;
    int         ret;

    if( 0 != socket.startClient("127.0.0.1", 10000, SOCKET_UDP) ) {
        dbg_pe("Can not start UDP multicast!");
        return -1;
    }

    while(1) {
        ret = socket.recv(msg, 256);
        if( ret > 0 ) printf("recv message: %s\n", msg.c_str());
    }

    return 0;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

// References:
//  http://ntrg.cs.tcd.ie/undergrad/4ba2/multicast/antony/example.html
//  https://en.wikipedia.org/wiki/Multicast_addresss
//
int test_udp_multicast_server(void)
{
    RSocket     socket;
    string      msg;

    if( 0 != socket.startServer("225.0.0.10", 10000, SOCKET_UDP_MULTICAST) ) {
        dbg_pe("Can not start UDP multicast!");
        return -1;
    }

    int idx = 0;
    while(1) {
        msg = fmt::sprintf("Hello [%6d]", idx++);
        printf("send message: %s\n", msg.c_str());
        socket.send(msg);

        tm_sleep(500);
    }

    return 0;
}


int test_udp_multicast_client(void)
{
    RSocket     socket;
    string      msg;
    int         ret;

    // https://en.wikipedia.org/wiki/Multicast_addresss
    if( 0 != socket.startClient("225.0.0.10", 10000, SOCKET_UDP_MULTICAST) ) {
        dbg_pe("Can not start UDP multicast!");
        return -1;
    }

    while(1) {
        ret = socket.recv(msg, 256);
        if( ret > 0 ) printf("recv message: %s\n", msg.c_str());
    }

    return 0;
}



////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

int test_NetTransfer_UDP_server(void)
{
    NetTransfer_UDP nt;
    string          msg;

    // open network transfer
    if( 0 != nt.open(1, 10000, "225.0.0.10", SOCKET_UDP_MULTICAST) ) {
        dbg_pe("Open network transfer failed!");
        return -1;
    }

    int idx = 0;
    while( 1 ) {
        msg = fmt::sprintf("Hello [%6d]", idx++);
        printf("send message: %s\n", msg.c_str());

        nt.send((uint8_t*) msg.c_str(), msg.size());

        tm_sleep(500);
    }

    return 0;
}


int test_NetTransfer_UDP_client(void)
{
    NetTransfer_UDP nt;
    char            buf[2000];
    uint64_t        recv_len = 2000;
    int             ret;

    if( 0 != nt.open(0, 10000, "225.0.0.10", SOCKET_UDP_MULTICAST) ) {
        dbg_pe("Open network transfer failed!");
        return -1;
    }

    while(1) {

        ret = nt.recv((uint8_t*) buf, recv_len);

        if( ret > 0 ) {
            buf[recv_len] = 0;
            printf("recv message: %s\n", buf);
        }
    }

    return 0;
}



////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

int test_videoServer(void)
{
    cv::VideoCapture    vc;
    NetTransfer_UDP     nt;

    string              addr = "225.0.0.10";
    int                 port = 10000;
    int                 useMulticast = 1;
    RSocketType         st = SOCKET_UDP_MULTICAST;

    int                 showImg = 0;
    double              scale = 0.5;

    // open video
    string video = svar.GetString("video", "");
    if( video.size() == 0 ) {
        dbg_pe("please input video file name by 'video=xxx'");
        return -1;
    }
    vc.open(video);

    showImg = svar.GetInt("showImg", showImg);
    scale = svar.GetDouble("scale", scale);


    // open network transfer
    addr = svar.GetString("addr", addr);
    port = svar.GetInt("port", port);
    useMulticast = svar.GetInt("useMulticast", useMulticast);
    if( useMulticast ) st = SOCKET_UDP_MULTICAST;
    else               st = SOCKET_UDP;
    if( 0 != nt.open(1, port, addr, st) ) {
        dbg_pe("Open network transfer failed!");
        return -2;
    }

    cv::Mat     imgIn, img;
    RDataStream ds;
    uint32_t    imgIdx = 0;

    while(1) {
        vc >> imgIn;
        if( imgIn.empty() ) break;

        cv::resize(imgIn, img, cv::Size(), scale, scale);

        int32_t imgW, imgH, imgC;
        imgW = img.cols;
        imgH = img.rows;
        imgC = img.channels();

        ds.clear();
        ds.write(imgIdx);
        ds.write(imgW);
        ds.write(imgH);
        ds.write(imgC);
        ds.write(img.data, imgW*imgH*imgC);

        double t1 = tm_getTimeStamp();
        int nSend = nt.send(ds.data(), ds.size());
        double t2 = tm_getTimeStamp();

        dbg_pt("send frame [%5d] %d x %d (%d), %6d bytes, send time = %f",
               imgIdx++, imgW, imgH, imgC,
               nSend, t2-t1);

        if( showImg ) {
            imshow("videoServer", img);
            cv::waitKey(15);
        }
    }

    return 0;
}

int test_videoClient(void)
{
    NetTransfer_UDP     nt;

    string              addr = "225.0.0.10";
    int                 port = 10000;
    int                 useMulticast = 1;
    RSocketType         st = SOCKET_UDP_MULTICAST;

    int                 ret;
    uint8_t             *buf;
    int                 bufLen;


    addr = svar.GetString("addr", addr);
    port = svar.GetInt("port", port);
    useMulticast = svar.GetInt("useMulticast", useMulticast);
    if( useMulticast ) st = SOCKET_UDP_MULTICAST;
    else               st = SOCKET_UDP;

    if( 0 != nt.open(0, port, addr, st) ) {
        dbg_pe("Open network transfer failed!");
        return -2;
    }

    RDataStream ds;
    uint32_t    imgIdx = 0;

    bufLen = 4000*3000*3;
    buf = new uint8_t[bufLen];

    while(1) {
        ret = nt.recv(buf, bufLen);
        if( ret <= 0 ) continue;

        ds.fromRawData_noCopy(buf, bufLen);

        int32_t imgW, imgH, imgC;
        ds.read(imgIdx);
        ds.read(imgW);
        ds.read(imgH);
        ds.read(imgC);

        cv::Mat img(imgH, imgW, CV_8UC3, ds.currDataPtr());

        dbg_pt("recv frame [%5d] %d x %d (%d)", imgIdx, imgW, imgH, imgC);

        imshow("videoClient", img);
        cv::waitKey(5);
    }

    delete [] buf;

    return 0;
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

int main(int argc, char *argv[])
{
    pi::dbg_stacktrace_setup();

    svar.ParseMain(argc, argv);
    string act = svar.GetString("Act", "test_server");

    if("test_server" == act)                    test_server();
    if("test_client" == act)                    test_client();
    if("test_messagepassing" == act)            test_messagepassing();
    if("test_Scommand" == act)                  test_Scommand();

    if("test_udp_server" == act)                test_udp_server();
    if("test_udp_client" == act)                test_udp_client();

    if("test_udp_multicast_server" == act)      test_udp_multicast_server();
    if("test_udp_multicast_client" == act)      test_udp_multicast_client();

    if("test_NetTransfer_UDP_server" == act)    test_NetTransfer_UDP_server();
    if("test_NetTransfer_UDP_client" == act)    test_NetTransfer_UDP_client();

    if("test_videoServer" == act)               test_videoServer();
    if("test_videoClient" == act)               test_videoClient();

    return 0;
}
