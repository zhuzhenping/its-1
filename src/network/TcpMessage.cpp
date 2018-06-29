#include "TcpMessage.h"
#include <cstdlib>
#include <cstring>
#include <cstdio>
#include "common/Thread.h"

namespace network {

const char *CHECK_CODE = "Zhongan"; // 校验码.


TcpMessage::TcpMessage(void) 
	: body_length_(0)
	, body_msg_(NULL)
	, data_is_leagle_(false)
{
	memset(header_,0, sizeof(header_));
}

TcpMessage::~TcpMessage(void) {
	//clear_data();
}

void TcpMessage::clear_data() { 
	memset(header_,0, sizeof(header_));
	if (body_msg_) {
		delete[] body_msg_;
		body_msg_ = NULL;
	}
	body_length_ = 0;
	data_is_leagle_ = false;
}

void TcpMessage::decode_header()
{
	if (memcmp(CHECK_CODE, header_, check_code_length) != 0) {
		data_is_leagle_ = false;
	}
	else {
		char len[body_length_value + 1] = {0};
		memcpy(len, header_+check_code_length, body_length_value);
		body_length_ = atoi(len);
		if (body_length_ <= 0 || body_length_ > max_body_length)
			data_is_leagle_ = false;
		else
			data_is_leagle_ = true;
	}

	if (false == data_is_leagle_)
		body_length_ = max_body_length;

	body_msg_ = new char[body_length_];
	memset(body_msg_, 0, body_length_);
}

void TcpMessage::encode_header(const char *buf, int len) {
	while (body_msg_) { // 还没发完.
		zhongan::common::Thread::Sleep(1);
	}

	body_length_ = head_length()+len;
	body_msg_ = new char[body_length_];
	memset(body_msg_, 0, body_length_);
	memcpy(body_msg_, CHECK_CODE, check_code_length);
	sprintf(body_msg_ + check_code_length, "%12d", len);
	memcpy(body_msg_ + head_length(), buf, len);
}

bool TcpMessage::is_heart_beat() const{
	return !strcmp(body_msg_, CHECK_CODE);
}

}