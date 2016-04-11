#ifndef DATASTREAM_H
#define DATASTREAM_H

class DataStream;

class StreamAble
{
public:
    //Success return 0
    virtual int   toStream(const DataStream& stream)=0;
    virtual int fromStream(const DataStream& stream)=0;
};

template <class T>
class IsStreamAble
{
    enum {Result=false};
};

class DataStream
{
public:
    DataStream();

    DataStream& operator <<(const StreamAble& element);
    int add(const StreamAble* ele);

protected:

};

#endif // DATASTREAM_H
