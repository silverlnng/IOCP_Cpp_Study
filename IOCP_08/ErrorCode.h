#pragma once

enum class ERROR_CODE : unsigned short
{
	NONE = 0,

	LOGIN_USER_ALREADY=31,
	LOGIN_USER_USED_ALL_OBJ =32,
	LOGIN_USER_INVALID_PW =33
};
