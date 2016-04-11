#include <unistd.h>
#include <iostream>

#include <base/time/Global_Timer.h>

using namespace pi;
using namespace std;

int main()
{
    int i=100;
    while(i--)
    {
        timer.enter("Timer::DelayTest");
        timer.leave("Timer::DelayTest");
        timer.enter("Timer::True=10us");
        usleep(10);
        timer.leave("Timer::True=10us");

        timer.enter("Timer::True=1ms");
        usleep(1000);
        timer.leave("Timer::True=1ms");

        timer.enter("Timer::True=10ms");
        usleep(10000);
        timer.leave("Timer::True=10ms");
    }

    TicTac tictac;
    Rate rate(60);
    tictac.Tic();
    for(int j=0;j<60;j++)
    {
        timer.enter("Timer::Rate60");
        rate.sleep();
        usleep(5000);
        timer.leave("Timer::Rate60");
    }
    cout<<"TicTac:"<<tictac.Tac();
}
