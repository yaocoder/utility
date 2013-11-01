#ifndef PASSWORD_MD5CRYPT_H
#define PASSWORD_MD5CRYPT_H
#include <string>

#define	MD5_RESULTLEN (128/8)

struct md5_context {
	unsigned long lo, hi;
	unsigned long a, b, c, d;
	unsigned char buffer[64];
	unsigned long block[MD5_RESULTLEN];
};

void safe_memset(void *data, int c, size_t size);

void md5_init(struct md5_context *ctx);
void md5_update(struct md5_context *ctx, const void *data, size_t size);
void md5_final(struct md5_context *ctx, unsigned char result[MD5_RESULTLEN]);
void md5_get_digest(const void *data, size_t size,
		    unsigned char result[MD5_RESULTLEN]);

bool Md5_salt(const std::string& password, std::string& md5_pass);

/*****************************************************************
函数原型:bool password_generate_md5_crypt(const char *pw, const char *salt, char *md5pass)
函数功能:根据pw和salt生成MD5pw
参数说明:pw:用户输入的密码(未加密);salt:md5 salt;md5pass:(出参)加密后的密码
返回值	:生成成功返回true,否则返回false
*****************************************************************/
bool password_generate_md5_crypt(const char *pw, const char *salt, char *md5pass);

/*****************************************************************
函数原型:bool generate_salt(unsigned len, char *ret_salt)
函数功能:生成md5的salt
参数说明:len:salt的长度;ret_salt:(出参)生成的salt
返回值	:生成成功返回true,否则返回false
*****************************************************************/
bool generate_salt(unsigned len, char *ret_salt);

/*****************************************************************
函数原型:bool generate_md5_password(const char *pw, char *md5pass)
函数功能:密码验证
参数说明:pw:用户输入的密码(未加密);md5pass:(出参)生成的md5密码
返回值	:密码匹配返回true,否则返回false
*****************************************************************/
bool generate_md5_password(const char *pw, char *md5pass);

/*****************************************************************
函数原型:bool check_password(const char *inputpw, const char *dbpw)
函数功能:密码验证
参数说明:inputpw:用户输入的密码(未加密);dbpw:数据库中查询出来的密码(已加密)
返回值	:密码匹配返回true,否则返回false
*****************************************************************/
bool check_password(const char *inputpw, const char *dbpw);

#endif
