#include "mysqlDataSource.h"
#include "mysqlConnector.h"
#include "mysqlBinlogReader.h"
#include "BinlogEventParser.h"
#include "glog/logging.h"
#include "memory/ringBuffer.h"
#include "store/store.h"
#include "mysqlRecordOffset.h"
#include "initMetaData.h"
#include <thread>
#include "util/valgrindTestUtil.h"
namespace DATA_SOURCE {
#ifdef OS_WIN
#define mysqlFuncLib "mysqlParserFuncs.dll"
#define mysqlParserTree "..\\..\\..\\..\\sqlParser\\ParseTree"
#endif
#ifdef OS_LINUX
	#define mysqlFuncLib "lib/libmysqlParserFuncs.so"
	#define mysqlParserTree "sqlParser/ParseTree"
#endif

	static constexpr uint64_t BEGIN_RECORD  = 0x1ffffffffffffffful;
	static constexpr uint64_t COMMIT_RECORD = 0x2ffffffffffffffful;
	static constexpr auto ASYNC = "async";
	#define asyncPushRecord(r)	while (unlikely(!m_outputQueue.push((r), 100)))\
	{\
		if (unlikely(!m_running))\
			break;\
	}
	void mysqlDataSource::initByConf()
	{
		std::string rtv = m_connector->initByConf();
		if (!rtv.empty())
		{
			LOG(ERROR) << rtv;
		}
	}
	void mysqlDataSource::updateConf(const char* key, const char* value)
	{

	}
	mysqlDataSource::mysqlDataSource(config* conf, META::metaDataCollection* metaDataCollection, STORE::store* store):dataSource(conf,metaDataCollection,store),m_async(false),m_running(false)
	{
		m_recordBufferPool = new ringBuffer();
		m_connector = new mysqlConnector(conf);
		m_metaDataCollection = metaDataCollection;
		m_store = store;
		m_reader = new mysqlBinlogReader(m_connector);
		m_parser = new BinlogEventParser(m_metaDataCollection, m_recordBufferPool);
		m_currentRecord = nullptr;
	}
	mysqlDataSource::~mysqlDataSource()
	{
		delete m_reader;
		delete m_parser;
		delete m_connector;
		delete m_recordBufferPool;
	}
	void mysqlDataSource::readThread()
	{
		BinlogEventParser::ParseStatus parseStatus;
		while (likely(m_running))
		{
			const char* logEvent;
			size_t size;
			if(unlikely(m_reader->readBinlog(logEvent, size)!= mysqlBinlogReader::READ_OK|| logEvent == nullptr))
				goto FAILED;
			parseStatus = m_parser->parser(logEvent, size);
			switch (parseStatus)
			{
			case BinlogEventParser::OK:
			{
				DATABASE_INCREASE::record* record;
				do {
					if (nullptr != (record = m_parser->getRecord()))
					{
						asyncPushRecord(record);
					}
					else
						break;
				} while (likely(m_running));
				break;
			}
			case BinlogEventParser::FILTER:
				break;
			case BinlogEventParser::BEGIN:
				asyncPushRecord(static_cast<DATABASE_INCREASE::record*>((void*)BEGIN_RECORD));
				break;
			case BinlogEventParser::COMMIT:
				asyncPushRecord(static_cast<DATABASE_INCREASE::record*>((void*)COMMIT_RECORD));
				break;
			default:
				m_lastError = m_parser->getError();
				goto FAILED;
			}
		}
	FAILED:
		m_running = false;
		return;
	}
	DATABASE_INCREASE::record* mysqlDataSource::asyncRead()
	{
		static int i = 0;
		DATABASE_INCREASE::record* record = nullptr;
		do {
			if (m_outputQueue.pop(record, 10))
			{
				if (((uint64_t)record) == BEGIN_RECORD)
					m_store->begin();
				else if (((uint64_t)record) == COMMIT_RECORD)
					m_store->commit();
				else
				{
					if((++i&0xffff) == 0)
						LOG(ERROR)<<record->head->logOffset;
					return record;
				}
			}
		} while (likely(m_running));
		return nullptr;
	}
	DATABASE_INCREASE::record* mysqlDataSource::syncRead()
	{
		static int i = 0;
		DATABASE_INCREASE::record* record = nullptr;
		if (nullptr != (record = m_parser->getRecord()))
			return record;
		const char* logEvent = nullptr;
		size_t size = 0;
		BinlogEventParser::ParseStatus parseStatus;
		do {
			if (unlikely(m_reader->readBinlog(logEvent, size) != mysqlBinlogReader::READ_OK || logEvent == nullptr))
				return nullptr;
			parseStatus = m_parser->parser(logEvent, size);
			switch (parseStatus)
			{
			case BinlogEventParser::OK:
			{
				if (nullptr != (record = m_parser->getRecord()))
				{
					if((++i&0xffff) == 0)
						LOG(ERROR)<<record->head->logOffset;
					return record;
				}
				break;
			}
			case BinlogEventParser::FILTER:
				break;
			case BinlogEventParser::BEGIN:
				m_store->begin();
				break;
			case BinlogEventParser::COMMIT:
				m_store->commit();
				break;
			default:
				m_lastError = m_parser->getError();
				return nullptr;
			}
		} while (likely(m_running));
		return nullptr;
	}
	DATABASE_INCREASE::record* mysqlDataSource::read()
	{
		if (likely(m_currentRecord != nullptr))
			m_recordBufferPool->freeMem(m_currentRecord);
		m_currentRecord = m_async ? asyncRead() : syncRead();
		if (m_currentRecord == nullptr)
			return nullptr;
#if 0
		if(m_currentRecord->head->minHead.type>=4)
			vSave(m_currentRecord,m_currentRecord->head->minHead.size+sizeof(DATABASE_INCREASE::DMLRecord));
		else
			vSave(m_currentRecord,m_currentRecord->head->minHead.size+sizeof(DATABASE_INCREASE::DDLRecord));
#endif
		return m_currentRecord;
	}
	const char* mysqlDataSource::dataSourceName() const
	{
		return NAME;
	}
	bool mysqlDataSource::start()
	{
		std::string rtv = m_connector->initByConf();
		if (!rtv.empty())
		{
			LOG(ERROR) << rtv;
			m_lastError = std::string("init connect failed for").append(rtv);
			return false;
		}
		uint64_t timestamp = 0, logOffset = 0;
		if (!m_store->checkpoint(timestamp, logOffset))
		{
			timestamp = m_conf->getLong(SECTION, std::string(CHECKPOINT_SECTION).append(START_TIMESTAMP).c_str(), 0, 0, 0x0fffffffffffffffull);
			logOffset = m_conf->getLong(SECTION, std::string(CHECKPOINT_SECTION).append(START_LOGPOSITION).c_str(), 0, 0, 0x0fffffffffffffffull);
		}
		else
		{
			META::timestamp t;
			t.time = timestamp;
			timestamp = t.seconds;
		}
		if (logOffset > 0)
		{
			if (mysqlBinlogReader::READ_OK != m_reader->seekBinlogByCheckpoint(fileId(logOffset), offsetInFile(logOffset)))
			{
				LOG(ERROR) << "mysql datasource init from binlog position:" << offsetInFile(logOffset) << "@" << fileId(logOffset) << " failed";
				return false;
			}
		}
		else if (timestamp > 0)
		{
			if (mysqlBinlogReader::READ_OK != m_reader->seekBinlogByTimestamp(timestamp))
			{
				LOG(ERROR) << "mysql datasource init from timestamp:" << timestamp << " failed";
				return false;
			}
		}
		else
		{
			LOG(ERROR) << "can not find nether "<< START_TIMESTAMP<<" or "<< START_LOGPOSITION<<" in config";
			return false;
		}
		initMetaData imd(m_connector);
		std::vector<std::string> databases;
		if (0 != imd.getAllUserDatabases(databases))
		{
			LOG(ERROR) << "get database list from mysql server failed";
			return false;
		}
		if (0 != imd.loadMeta(m_metaDataCollection, databases))
		{
			LOG(ERROR) << "load meta from mysql server failed";
			return false;
		}
		m_metaDataCollection->print();
		m_running = true;
		if (m_conf->get(SECTION, ASYNC, "false").compare("true") == 0)
		{
			m_async = true;
			m_thread = std::thread(asyncReadThread, this);
		}
		else
			m_async = false;
		if(0 != m_parser->init(mysqlFuncLib,mysqlParserTree))
		{
			LOG(ERROR) << "init binlog parser failed";
			return false;
		}
		return true;
	}
	bool mysqlDataSource::stop()
	{
		m_running = false;
		if (m_async)
			m_thread.join();
		return true;
	}
	bool mysqlDataSource::running()const
	{
		return m_running;
	}
	extern "C" DLL_EXPORT dataSource* instance(config* conf, META::metaDataCollection* metaDataCollection, STORE::store* store)
	{
		return new mysqlDataSource(conf,metaDataCollection,store);
	}

}
