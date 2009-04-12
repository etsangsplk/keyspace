#include "System/Config.h"
#include "System/Events/EventLoop.h"
#include "System/IO/IOProcessor.h"
#include "Framework/ReplicatedLog/ReplicatedLog.h"
#include "Framework/Database/Database.h"
#include "Application/TestDB/TestDB.h"
#include "Application/Keyspace/Database/KeyspaceDB.h"
#include "Application/Keyspace/Protocol/HttpServer.h"
#include "Application/Keyspace/Protocol/Keyspace/KeyspaceServer.h"
#include "Framework/Transport/TransportTCPReader.h"
#include "Framework/Transport/TransportTCPWriter.h"
/*
void f();
void PrintRead();

Endpoint me;
TransportTCPWriter writer;
TransportTCPReader reader;
CFunc cfunc(&f);
CFunc onReaderRead(&PrintRead);
CdownTimer timer(1000, &cfunc);
ByteArray<8> bs;

void f()
{
	bs.Set("hello");

	writer.Write(bs);
	
	EventLoop::Get()->Add(&timer);
}

void PrintRead()
{
	ByteString bs;
	
	reader.GetMessage(bs);
	printf("\nPrintRead: %.*s\n", bs.length, bs.buffer);
}
*/
int main(int argc, char* argv[])
{
	if (argc != 2)
	{
		printf("usage: %s <config-file>\n", argv[0]);
		return 1;
	}
	
	//Log_SetTimestamping(true);

	if (!Config::Init("keyspace.conf"))
		ASSERT_FAIL();

	IOProcessor::Init();
	database.Init(Config::GetValue("database.dir", "."));	
	
	if (!PaxosConfig::Get()->Init(argv[1]))
		ASSERT_FAIL();

	ReplicatedLog::Get()->Init();
	
	KeyspaceDB kdb;
	kdb.Init();
	
//	TestDB testdb;
//	testdb.Init(ioproc, eventloop, &rl);

	HttpServer protoHttp;
	protoHttp.Init(&kdb, Config::GetIntValue("http.port", 8080));

	KeyspaceServer protoKeyspace;
	protoKeyspace.Init(&kdb, Config::GetIntValue("keyspace.port", 7080));

	EventLoop::Run();

/*
	IOProcessor::Get()->Init();
	
	reader.Init(5000);
	reader.SetOnRead(&onReaderRead);
	
	me.Set("127.0.0.1", 5000);
	writer.Init(me);
	
	EventLoop::Get()->Add(&timer);
	
	EventLoop::Get()->Run();
*/
}
