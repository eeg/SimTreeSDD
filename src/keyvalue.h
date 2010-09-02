/*******************************************************************************
 * these functions make it easy to read user input from file or command line
 * they were written by Walter Brisken
 ******************************************************************************/

#ifndef __KEYVALUE__
#define __KEYVALUE__

#define KV_MAXPARMS	1024 /* Max number of parameters for a KeyValue */
#define KV_INTERR	-999999
#define KV_FLOATERR	-999999.0

struct KeyValue
{
	char *key[KV_MAXPARMS];
	char *value[KV_MAXPARMS];
	int nparms;
};

struct KeyValue *newKeyValue();
void deleteKeyValue(struct KeyValue *p);
void KeyValueaddparm(struct KeyValue *p, const char *key, const char *value);
struct KeyValue *loadKeyValue(const char *filename);
int KeyValuekeyindex(const struct KeyValue *p, const char *key);
int getKeyValueint(const struct KeyValue *p, const char *key);
double getKeyValuedouble(const struct KeyValue *p, const char *key);
const char *getKeyValuestring(const struct KeyValue *p, const char *key);
void printKeyValue(const struct KeyValue *p);

#endif
