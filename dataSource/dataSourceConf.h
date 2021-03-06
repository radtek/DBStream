#pragma once
namespace DATA_SOURCE
{
	static constexpr auto SECTION = "dataSource";
	static constexpr auto DATASOURCE_TYPE = "dataSourceType";
	static constexpr auto DATASOURCE_LIBDIR_PATH = "dataSourceLibDirPath";
#ifdef OS_WIN
	static constexpr auto DATASOURCE_LIBDIR_DEFAULT_PATH = ".\\";
#endif
#ifdef OS_LINUX
	static constexpr auto DATASOURCE_LIBDIR_DEFAULT_PATH = "./lib/dataSource/";
#endif

	static constexpr auto HOST = "host";
	static constexpr auto PORT = "port";
	static constexpr auto USER = "user";
	static constexpr auto PASSWORD = "password";
	static constexpr auto CONNNECT_TIMEOUT = "connectTimeout";
	static constexpr auto READ_TIMEOUT = "readTimeout";
	static constexpr auto WRITE_TIMEOUT = "writeTimeout";

	static constexpr auto SERVICE_NAME = "serviceName";
	static constexpr auto SID = "sid";


	static constexpr auto CHECKPOINT_SECTION = "checkpoint.";

	static constexpr auto START_TIMESTAMP = "startTimestamp";
	static constexpr auto START_LOGPOSITION = "startLogPosition";
}


