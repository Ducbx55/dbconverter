#include "DbRecordConvertor.h"
#include <chrono>
#include <thread>

DbRecordConvertor::DbRecordConvertor(RecordProducer* producer, StatementConsumer* consumer, StatementQueue* queue)
{
	this->m_consumer = consumer;
	this->m_producer = producer;
	m_queue = queue;
}

bool DbRecordConvertor::start(std::vector<Table*> tblList)
{
	// start consumer on other thread
	std::thread consumerThread(&DbRecordConvertor::startConsumer);

	// start producer on this main thread
	startProducer(tblList);

	// wait consumer consums all statements in the queue
	while (!m_queue->isEmpty())
	{
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}

	// stop queue
	m_queue->stop();

	// stop consumer
	m_consumer->stop();

	consumerThread.join();
}

void DbRecordConvertor::startConsumer()
{
	m_consumer->startConsum();
}

void DbRecordConvertor::startProducer(std::vector<Table*> tblList)
{
	for (int i = 0; i < tblList.size(); i++)
	{
		m_producer->startGetRecord(tblList.at(i));
	}
}