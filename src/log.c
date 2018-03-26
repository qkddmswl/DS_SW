#include <stdio.h>
#include <stdarg.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <app_common.h>
#include "log.h"

#define MAX_LOG_SIZE 4096
#define PATH_MAX 4096

static FILE *log_fp = NULL;
static log_type ltype = LOG_TYPE_DLOG;

static const char log_prio_name[][DLOG_PRIO_MAX-1] = {
	"UNKNOWN",
	"DEFAULT", /**< Default */
	"VERBOSE", /**< Verbose */
	"DEBUG", /**< Debug */
	"INFO", /**< Info */
	"WARN", /**< Warning */
	"ERROR", /**< Error */
	"FATAL", /**< Fatal */
	"SILENT" /**< Silent */
};

static inline char* getFormattedTime(void)
{
	struct timeval val;
	struct tm *ptm;
	static char res_time[40] = {0, };

	gettimeofday(&val, NULL);
	ptm = localtime(&val.tv_sec);

	// format : YYMMDDhhmmssuuuuuu
	snprintf(res_time, sizeof(res_time), "%04d-%02d-%02d %02d:%02d:%02d:%06ld"
		, ptm->tm_year + 1900, ptm->tm_mon + 1, ptm->tm_mday
		, ptm->tm_hour, ptm->tm_min, ptm->tm_sec
		, val.tv_usec);

	return res_time;
}

static int __open_log_file(void)
{
	char buf[PATH_MAX] = {0,};
	char *prefix = NULL;

	prefix = app_get_data_path();

	if (!prefix)
		goto error;

	snprintf(buf, sizeof(buf)-1, "%s%s", prefix, "log.txt");
	free(prefix);

	log_fp = fopen(buf, "a"); /* append or create new ??? */
	if (log_fp == NULL) {
		dlog_print(DLOG_WARN, LOG_TAG, "%s\n", strerror(errno));
		goto error;
	}

	return 0;

error:
	dlog_print(DLOG_WARN, LOG_TAG, "error to use log file, so use dlog as log system\n");
	ltype = LOG_TYPE_DLOG;
	return -1;
}

int log_type_set(log_type type)
{
	ltype = type;
	dlog_print(DLOG_DEBUG, LOG_TAG, "setting log type : [%d]\n", type);
	switch (type) {
	case LOG_TYPE_FILE:
	case LOG_TYPE_ALL:
		__open_log_file();
		break;
	case LOG_TYPE_DLOG: /* nothing to do */
	default:
		ltype = LOG_TYPE_DLOG;
		break;
	}
	return 0;
}

void log_file_close(void)
{
	if (log_fp) {
		fclose(log_fp);
		log_fp = NULL;
		dlog_print(DLOG_DEBUG, LOG_TAG, "close log file\n");
	}

	log_type_set(LOG_TYPE_DLOG);

	return;
}

int log_print(log_priority prio, const char *tag, const char *fmt, ...)
{
	va_list ap;

	switch (ltype) {
	case LOG_TYPE_FILE:
		if (log_fp) {
			va_start(ap, fmt);
			fprintf(log_fp, "[%s] [%s]", getFormattedTime(), log_prio_name[prio]);
			vfprintf(log_fp, fmt, ap);
			va_end(ap);
			fflush(log_fp);
		}
		break;
	case LOG_TYPE_ALL:
		va_start(ap, fmt);
		if (log_fp) {
			fprintf(log_fp, "[%s] [%s]", getFormattedTime(), log_prio_name[prio]);
			vfprintf(log_fp, fmt, ap);
		}
		dlog_vprint(prio, tag, fmt, ap);
		va_end(ap);

		if (log_fp)
			fflush(log_fp);
		break;
	case LOG_TYPE_DLOG:
	default:
		va_start(ap, fmt);
		dlog_vprint(prio, tag, fmt, ap);
		va_end(ap);
		break;
	}
	return 0;
}
