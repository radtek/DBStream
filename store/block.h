/*
 * block.h
 *
 *  Created on: 2019年1月18日
 *      Author: liwei
 */

#ifndef BLOCK_H_
#define BLOCK_H_
#include <stdint.h>
#include "../util/ref.h"
#include "page.h"
namespace STORE{
#define BLOCK_FLAG_APPENDING    0x01
#define BLOCK_FLAG_FINISHED     0x02
#define BLOCK_FLAG_FLUSHED      0x04
#define BLOCK_FLAG_HAS_REDO     0x08
#define BLOCK_FLAG_COMPRESS     0x10
#define BLOCK_FLAG_MULTI_TABLE  0x11
#pragma pack(1)
struct recordID{
    uint32_t id;
    uint32_t pos;
};
struct dataPartHead {
	uint64_t dataPartSize;
	union {
		uint64_t rawSizeBeforeCompress;
		struct {
			uint32_t resever;
			uint32_t crc;
		};
	};
};
struct  tableDataInfo
{
	uint64_t tableId;
	uint32_t recordCount;
	uint32_t recordIdsOffset;
	uint16_t pageCount;
	uint16_t startPageId;
	uint32_t recordDataSize;
	uint32_t indexsOffset;
};
#define setRecordPosition(p,pageId,offsetInPage) (p) = (pageId);(p) <<= 20;(p)+= (offsetInPage);
#define pageId(p) ((p)>>20)
#define offsetInPage(p) ((p)&0x000fffff)
struct recordGeneralInfo
{
	uint8_t recordType;
	uint32_t tableIndex;
	uint32_t offset;
	uint64_t timestamp;
	uint64_t logOffset;
};
struct tableFullData {
	uint16_t recordPageCount;
	uint16_t firstRecordPageId;
	uint16_t indexCount;
	uint16_t firstIndexPageId;
	META::tableMeta *meta;
};
class block{
public:
	ref m_ref;
	uint32_t m_version;
    uint32_t m_flag;
    uint64_t m_blockID;
    uint64_t m_tableID;// if !(flag&BLOCK_FLAG_MULTI_TABLE)
	uint64_t m_minTime;
    uint64_t m_maxTime;
	uint64_t m_minLogOffset;
	uint64_t m_maxLogOffset;
	uint64_t m_minRecordId;
	uint32_t m_recordCount;
	uint32_t m_tableCount;
	uint16_t m_pageCount;
	block()
	{
		memset(&m_version, 0, sizeof(block) - offsetof(block,m_flag));
	}
    inline bool use()
    {
		return m_ref.use;
    }
    inline void unuse()
    {
		m_ref.unuse();
    }
    virtual int flush() =0;
};
}



#endif /* BLOCK_H_ */
