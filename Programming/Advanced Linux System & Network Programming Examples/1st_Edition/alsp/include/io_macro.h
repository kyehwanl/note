/* vim: set ts=4 sw=4: */
/* Filename    : io_macro.h
 * Description : useful macro for facilitate coding
 * Author      : SunYoung Kim <sunyzero@yahoo.com>
 * Notes       : 
 */
#ifndef IO_MACRO_H
#define IO_MACRO_H

#define print_msg(io, msgtype, arg...) \
	fprintf(io, "["#msgtype"] [%s/%s:%03d] ", __FILE__, __FUNCTION__, __LINE__); fprintf(io, arg); \
	fputc('\n', io);

#define pr_err(arg...) print_msg(stderr, ERR, arg)
#define pr_out(arg...) print_msg(stdout, REP, arg)
#endif
