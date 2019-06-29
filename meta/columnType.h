#pragma once
#include "../util/itoaSse.h"
namespace META {
	static constexpr uint16_t numToStrMap[] = { 0x3030,0x3031,0x3032,0x3033,0x3034,0x3035,0x3036,0x3037,0x3038,0x3039,
												0x3130,0x3131,0x3132,0x3133,0x3134,0x3135,0x3136,0x3137,0x3138,0x3139,
												0x3230,0x3231,0x3232,0x3233,0x3234,0x3235,0x3236,0x3237,0x3238,0x3239,
												0x3330,0x3331,0x3332,0x3333,0x3334,0x3335,0x3336,0x3337,0x3338,0x3339,
												0x3430,0x3431,0x3432,0x3433,0x3434,0x3435,0x3436,0x3437,0x3438,0x3439,
												0x3530,0x3531,0x3532,0x3533,0x3534,0x3535,0x3536,0x3537,0x3538,0x3539 };
#define T_UNION 0
#define T_UINT8 1
#define T_INT8  2
#define T_UINT16 3
#define T_INT16 4
#define T_UINT32 5
#define T_INT32 6
#define T_UINT64 7
#define T_INT64 8
#define T_BIG_NUMBER 9
#define T_FLOAT 10
#define T_DOUBLE 11
#define T_DECIMAL 12
#define T_TIMESTAMP 13
#define T_DATETIME 14
#define T_DATE 15
#define T_YEAR 16
#define T_TIME 17
#define T_BLOB 18
#define T_STRING 19
#define T_JSON 20
#define T_XML 21
#define T_GEOMETRY 22
#define T_SET 23
#define T_ENUM 24
#define T_BYTE 25
#define T_MAX_TYPE 255
	struct columnTypeInfo
	{
		uint8_t type;
		uint8_t columnTypeSize;
		bool asIndex;
		bool fixed;
	};
	constexpr static columnTypeInfo columnInfos[] = {
	{T_UNION,4,true,false,},
	{T_UINT8, 1,true,true},
	{T_INT8,1 ,true,true},
	{T_UINT16,2,true,true},
	{T_INT16, 2,true,true},
	{T_UINT32,4,true,true},
	{T_INT32,4 ,true,true},
	{T_UINT64,8,true,true},
	{T_INT64,8,true,true},
	{T_BIG_NUMBER,4 ,false,false},
	{T_FLOAT,4,true,true},
	{T_DOUBLE,8,true,true},
	{T_DECIMAL,4,true,true},
	{T_TIMESTAMP,8 ,true,true},
	{T_DATETIME,8,false,true},
	{T_DATE,4,false,true},
	{T_YEAR,2,false,true},
	{T_TIME,8,false,true},
	{T_BLOB,4,false,true},
	{T_STRING,4,false,true},
	{T_JSON,4,false,true},
	{T_XML,4,false,true},
	{T_GEOMETRY,4,false,true},
	{T_SET,8,false,true},
	{T_ENUM,2,false,true},
	{T_BYTE,8,false,true}
	};
#pragma pack(1)
	struct timestamp
	{
		union
		{
			struct {
				uint32_t seconds;
				uint32_t nanoSeconds;
			};
			uint64_t time;
		};
		inline uint8_t toString(char* str)
		{
			uint8_t len = u32toa_sse2(seconds, str);
			str[len - 1] = '.';
			char usec[8];
			*(uint64_t*)(str+len) = 3458817497345568816ul;//{'0','0','0','0','0','0','\0','0'}
			uint8_t secLen = u32toa_sse2(nanoSeconds/1000, usec);
			memcpy(str + len + (6 - secLen + 1), usec, secLen);
			return len+6;
		}
	};
	/*[24byte usec][17 byte seconds of day][5 byte day][2 byte month][16 bit year]*/
	struct dateTime
	{
		int64_t time;
		inline int16_t year()
		{
			return time >> 48;
		}
		inline uint8_t month()
		{
			return ((uint64_t)time >> 46) & 0x3u;
		}
		inline uint8_t day()
		{
			return ((uint64_t)time >> 41) & 0x1fu;
		}
		inline uint8_t hour()
		{
			return (((uint64_t)time >> 24) & 0x1ffffu) / 3600;
		}
		inline uint8_t mi()
		{
			return ((((uint64_t)time >> 24) & 0x1ffffu) % 3600) / 60;
		}
		inline uint8_t sec()
		{
			return ((((uint64_t)time >> 24) & 0x1ffffu) % 60);
		}
		inline uint32_t usec()
		{
			return ((uint64_t)time) & 0xfffffu;
		}
		static inline int64_t createDate(int16_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t mi, uint8_t sec, uint32_t usec)
		{
			int64_t tm = year;
			tm <<= 2;
			tm |= month;
			tm <<= 17;
			tm |= hour * 3600 + mi * 60 + sec;
			tm <<= 24;
			tm |= usec;
			return tm;
		}
		inline void set(int16_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t mi, uint8_t sec, uint32_t usec)
		{
			this->time = createDate(year, month, day, hour, mi, sec, usec);
		}
		inline uint8_t toString(char* str)
		{
			uint8_t yearLength = i32toa_sse2(year(), str);
			str[yearLength - 1] = '-';
			*(uint16_t*)& str[yearLength] = numToStrMap[month()];
			str[yearLength +2] = '-';
			*(uint16_t*)& str[yearLength+3] = numToStrMap[day()];
			str[yearLength + 5] = ' ';
			*(uint16_t*)& str[yearLength + 6] = numToStrMap[hour()];
			str[yearLength + 8] = ':';
			*(uint16_t*)& str[yearLength + 9] = numToStrMap[mi()];
			str[yearLength + 11] = ':';
			*(uint16_t*)& str[yearLength + 12] = numToStrMap[sec()];
			if (usec() != 0)
			{
				str[yearLength +14] = '.';
				char usecBuffer[8];
				*(uint64_t*)(yearLength + 15) = 3458817497345568816ul;//{'0','0','0','0','0','0','\0','0'}
				uint8_t secLen = u32toa_sse2(usec(), usecBuffer);
				memcpy(str + yearLength + 15 + 6 - secLen + 1, usecBuffer, secLen);
				return yearLength + 15+6+1;
			}
			else
			{
				str[yearLength + 14] = '\0';
				return yearLength+15;
			}
		}
		inline uint8_t toDateString(char* str)
		{
			uint8_t yearLength = i32toa_sse2(year(), str);
			str[yearLength - 1] = '-';
			*(uint16_t*)& str[yearLength] = numToStrMap[month()];
			str[yearLength + 2] = '-';
			*(uint16_t*)& str[yearLength + 3] = numToStrMap[day()];
			str[yearLength + 5] = '\0';
			return yearLength + 6;
		}
		inline uint8_t toTimeString(char* str)
		{
			uint8_t hourLength = 0;
			if (year() < 0)
			{
				hourLength = 1;
				str[0] = '-';
			}
			hourLength += u32toa_sse2(hour(), str+ hourLength);
			str[hourLength - 1] = ':';
			*(uint16_t*)& str[hourLength] = numToStrMap[mi()];
			str[hourLength + 2] = ':';
			*(uint16_t*)& str[hourLength + 3] = numToStrMap[sec()];
			if (usec() != 0)
			{
				str[hourLength + 5] = '.';
				char usecBuffer[8];
				*(uint64_t*)(hourLength + 6) = 3458817497345568816ul;//{'0','0','0','0','0','0','\0','0'}
				uint8_t secLen = u32toa_sse2(usec(), usecBuffer);
				memcpy(str + hourLength + 6 + 6 - secLen + 1, usecBuffer, secLen);
				return hourLength + 6 + 6 + 1;
			}
			else
			{
				str[hourLength + 5] = '\0';
				return hourLength + 6;
			}
		}
	};
	/*[24 bit usc][8 bit second][8 bit min][24 bit hour]*/
	struct Time
	{
		int64_t time;
		static inline int64_t createTime(int16_t hour, uint8_t mi, uint8_t second, uint32_t usec)
		{
			int64_t t = hour;
			t <<= 8;
			t |= mi;
			t <<= 8;
			t |= second;
			t <<= 24;
			t |= usec;
			return t;
		}
		inline void  set(int16_t hour, uint8_t mi, uint8_t second, uint32_t usec)
		{
			this->time = createTime(hour, mi, second, usec);
		}
		inline int32_t hour()
		{
			return time >> 40;
		}
		inline uint8_t mi()
		{
			return (time >> 32) & 0xffu;
		}
		inline uint8_t sec()
		{
			return (time >> 24) & 0xffu;
		}
		inline uint32_t usec()
		{
			return time & 0xffffffu;
		}
		inline uint8_t toString(char* str)
		{
			uint8_t hourLength = u32toa_sse2(hour(), str);
			str[hourLength -1] = ':';
			*(uint16_t*)& str[hourLength] = numToStrMap[mi()];
			str[hourLength + 2] = ':';
			*(uint16_t*)& str[hourLength + 3] = numToStrMap[sec()];
			if (usec() != 0)
			{
				str[hourLength + 5] = '.';
				char usecBuffer[8];
				*(uint64_t*)(hourLength + 6) = 3458817497345568816ul;//{'0','0','0','0','0','0','\0','0'}
				uint8_t secLen = u32toa_sse2(usec(), usecBuffer);
				memcpy(str + hourLength + 6 + 6 - secLen + 1, usecBuffer, secLen);
				return hourLength + 6 + 6 + 1;
			}
			else
			{
				str[hourLength + 5] = '\0';
				return hourLength + 6;
			}
		}
	};
	/*[1 byte day][1 byte month][2 byte year]*/
	struct Date {
		int32_t time;
		static inline int32_t createDate(int16_t year, uint8_t month, uint8_t day)
		{
			int32_t tm = year;
			tm <<= 16;
			tm |= month;
			tm <<= 8;
			tm |= day;
			return tm;
		}
		inline void set(int16_t year, uint8_t month, uint8_t day)
		{
			time = createDate(year, month, day);
		}
		inline int16_t year()
		{
			return time >> 16;
		}
		inline uint8_t month()
		{
			return ((uint32_t)time >> 8) & 0xffu;
		}
		inline uint8_t day()
		{
			return ((uint32_t)time) & 0xffu;
		}
		uint8_t toString(char* str)
		{
			uint8_t yearLength = i32toa_sse2(year(), str);
			str[yearLength - 1] = '-';
			*(uint16_t*)& str[yearLength] = numToStrMap[month()];
			str[yearLength + 2] = '-';
			*(uint16_t*)& str[yearLength + 3] = numToStrMap[day()];
			str[yearLength + 5] = '\0';
			return yearLength + 6;
		}

	};
}