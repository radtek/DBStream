/*
 * mysqlTypes.h
 *
 *  Created on: 2018年12月5日
 *      Author: liwei
 */

#ifndef MYSQLTYPES_H_
#define MYSQLTYPES_H_
#include "columnType.h"
	constexpr uint8_t mysqlTypeMaps[] = { META::T_DECIMAL,META::T_INT8,META::T_INT16,META::T_INT32,META::T_FLOAT,META::T_DOUBLE,META::T_MAX_TYPE,META::T_TIMESTAMP,META::T_INT64,META::T_INT32,META::T_DATE,META::T_TIME,META::T_DATETIME,META::T_YEAR,META::T_DATE,META::T_STRING,META::T_BYTE,META::T_TIMESTAMP,META::T_DATETIME,META::T_TIME,
	META::T_MAX_TYPE,META::T_MAX_TYPE,META::T_MAX_TYPE,META::T_MAX_TYPE,META::T_MAX_TYPE,META::T_MAX_TYPE,META::T_MAX_TYPE,META::T_MAX_TYPE,META::T_MAX_TYPE,META::T_MAX_TYPE,META::T_MAX_TYPE,META::T_MAX_TYPE,META::T_MAX_TYPE,META::T_MAX_TYPE,META::T_MAX_TYPE,META::T_MAX_TYPE,META::T_MAX_TYPE,META::T_MAX_TYPE,META::T_MAX_TYPE,META::T_MAX_TYPE,
	META::T_MAX_TYPE,META::T_MAX_TYPE,META::T_MAX_TYPE,META::T_MAX_TYPE,META::T_MAX_TYPE,META::T_MAX_TYPE,META::T_MAX_TYPE,META::T_MAX_TYPE,META::T_MAX_TYPE,META::T_MAX_TYPE,META::T_MAX_TYPE,META::T_MAX_TYPE,META::T_MAX_TYPE,META::T_MAX_TYPE,META::T_MAX_TYPE,META::T_MAX_TYPE,META::T_MAX_TYPE,META::T_MAX_TYPE,META::T_MAX_TYPE,META::T_MAX_TYPE,
	META::T_MAX_TYPE,META::T_MAX_TYPE,META::T_MAX_TYPE,META::T_MAX_TYPE,META::T_MAX_TYPE,META::T_MAX_TYPE,META::T_MAX_TYPE,META::T_MAX_TYPE,META::T_MAX_TYPE,META::T_MAX_TYPE,META::T_MAX_TYPE,META::T_MAX_TYPE,META::T_MAX_TYPE,META::T_MAX_TYPE,META::T_MAX_TYPE,META::T_MAX_TYPE,META::T_MAX_TYPE,META::T_MAX_TYPE,META::T_MAX_TYPE,META::T_MAX_TYPE,
	META::T_MAX_TYPE,META::T_MAX_TYPE,META::T_MAX_TYPE,META::T_MAX_TYPE,META::T_MAX_TYPE,META::T_MAX_TYPE,META::T_MAX_TYPE,META::T_MAX_TYPE,META::T_MAX_TYPE,META::T_MAX_TYPE,META::T_MAX_TYPE,META::T_MAX_TYPE,META::T_MAX_TYPE,META::T_MAX_TYPE,META::T_MAX_TYPE,META::T_MAX_TYPE,META::T_MAX_TYPE,META::T_MAX_TYPE,META::T_MAX_TYPE,META::T_MAX_TYPE,
	META::T_MAX_TYPE,META::T_MAX_TYPE,META::T_MAX_TYPE,META::T_MAX_TYPE,META::T_MAX_TYPE,META::T_MAX_TYPE,META::T_MAX_TYPE,META::T_MAX_TYPE,META::T_MAX_TYPE,META::T_MAX_TYPE,META::T_MAX_TYPE,META::T_MAX_TYPE,META::T_MAX_TYPE,META::T_MAX_TYPE,META::T_MAX_TYPE,META::T_MAX_TYPE,META::T_MAX_TYPE,META::T_MAX_TYPE,META::T_MAX_TYPE,META::T_MAX_TYPE,
	META::T_MAX_TYPE,META::T_MAX_TYPE,META::T_MAX_TYPE,META::T_MAX_TYPE,META::T_MAX_TYPE,META::T_MAX_TYPE,META::T_MAX_TYPE,META::T_MAX_TYPE,META::T_MAX_TYPE,META::T_MAX_TYPE,META::T_MAX_TYPE,META::T_MAX_TYPE,META::T_MAX_TYPE,META::T_MAX_TYPE,META::T_MAX_TYPE,META::T_MAX_TYPE,META::T_MAX_TYPE,META::T_MAX_TYPE,META::T_MAX_TYPE,META::T_MAX_TYPE,
	META::T_MAX_TYPE,META::T_MAX_TYPE,META::T_MAX_TYPE,META::T_MAX_TYPE,META::T_MAX_TYPE,META::T_MAX_TYPE,META::T_MAX_TYPE,META::T_MAX_TYPE,META::T_MAX_TYPE,META::T_MAX_TYPE,META::T_MAX_TYPE,META::T_MAX_TYPE,META::T_MAX_TYPE,META::T_MAX_TYPE,META::T_MAX_TYPE,META::T_MAX_TYPE,META::T_MAX_TYPE,META::T_MAX_TYPE,META::T_MAX_TYPE,META::T_MAX_TYPE,
	META::T_MAX_TYPE,META::T_MAX_TYPE,META::T_MAX_TYPE,META::T_MAX_TYPE,META::T_MAX_TYPE,META::T_MAX_TYPE,META::T_MAX_TYPE,META::T_MAX_TYPE,META::T_MAX_TYPE,META::T_MAX_TYPE,META::T_MAX_TYPE,META::T_MAX_TYPE,META::T_MAX_TYPE,META::T_MAX_TYPE,META::T_MAX_TYPE,META::T_MAX_TYPE,META::T_MAX_TYPE,META::T_MAX_TYPE,META::T_MAX_TYPE,META::T_MAX_TYPE,
	META::T_MAX_TYPE,META::T_MAX_TYPE,META::T_MAX_TYPE,META::T_MAX_TYPE,META::T_MAX_TYPE,META::T_MAX_TYPE,META::T_MAX_TYPE,META::T_MAX_TYPE,META::T_MAX_TYPE,META::T_MAX_TYPE,META::T_MAX_TYPE,META::T_MAX_TYPE,META::T_MAX_TYPE,META::T_MAX_TYPE,META::T_MAX_TYPE,META::T_MAX_TYPE,META::T_MAX_TYPE,META::T_MAX_TYPE,META::T_MAX_TYPE,META::T_MAX_TYPE,
	META::T_MAX_TYPE,META::T_MAX_TYPE,META::T_MAX_TYPE,META::T_MAX_TYPE,META::T_MAX_TYPE,META::T_MAX_TYPE,META::T_MAX_TYPE,META::T_MAX_TYPE,META::T_MAX_TYPE,META::T_MAX_TYPE,META::T_MAX_TYPE,META::T_MAX_TYPE,META::T_MAX_TYPE,META::T_MAX_TYPE,META::T_MAX_TYPE,META::T_MAX_TYPE,META::T_MAX_TYPE,META::T_MAX_TYPE,META::T_MAX_TYPE,META::T_MAX_TYPE,
	META::T_MAX_TYPE,META::T_MAX_TYPE,META::T_MAX_TYPE,META::T_MAX_TYPE,META::T_MAX_TYPE,META::T_MAX_TYPE,META::T_MAX_TYPE,META::T_MAX_TYPE,META::T_MAX_TYPE,META::T_MAX_TYPE,META::T_MAX_TYPE,META::T_MAX_TYPE,META::T_MAX_TYPE,META::T_MAX_TYPE,META::T_MAX_TYPE,META::T_MAX_TYPE,META::T_MAX_TYPE,META::T_MAX_TYPE,META::T_MAX_TYPE,META::T_MAX_TYPE,
	META::T_MAX_TYPE,META::T_MAX_TYPE,META::T_MAX_TYPE,META::T_MAX_TYPE,META::T_MAX_TYPE,META::T_JSON,META::T_DECIMAL,META::T_ENUM,META::T_SET,META::T_BLOB,META::T_BLOB,META::T_BLOB,META::T_BLOB,META::T_STRING,META::T_STRING,META::T_GEOMETRY
	};


#endif /* MYSQLTYPES_H_ */
