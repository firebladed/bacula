/*
 *   edit.c  edit string to ascii, and ascii to internal 
 * 
 *    Kern Sibbald, December MMII
 *
 *   Version $Id$
 */

/*
   Copyright (C) 2000, 2001, 2002 Kern Sibbald and John Walker

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License as
   published by the Free Software Foundation; either version 2 of
   the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
   General Public License for more details.

   You should have received a copy of the GNU General Public
   License along with this program; if not, write to the Free
   Software Foundation, Inc., 59 Temple Place - Suite 330, Boston,
   MA 02111-1307, USA.

 */

#include "bacula.h"
#include <math.h>

/* We assume ASCII input and don't worry about overflow */
uint64_t str_to_uint64(char *str) 
{
   register char *p = str;
   register uint64_t value = 0;

   while (B_ISSPACE(*p)) {
      p++;
   }
   if (*p == '+') {
      p++;
   }
   while (B_ISDIGIT(*p)) {
      value = value * 10 + *p - '0';
      p++;
   }
   return value;
}

int64_t str_to_int64(char *str) 
{
   register char *p = str;
   register int64_t value;
   int negative = FALSE;

   while (B_ISSPACE(*p)) {
      p++;
   }
   if (*p == '+') {
      p++;
   } else if (*p == '-') {
      negative = TRUE;
      p++;
   }
   value = str_to_uint64(p);
   if (negative) {
      value = -value;
   }
   return value;
}



/*
 * Edit an integer number with commas, the supplied buffer
 * must be at least 27 bytes long.  The incoming number
 * is always widened to 64 bits.
 */
char *edit_uint64_with_commas(uint64_t val, char *buf)
{
   sprintf(buf, "%" lld, val);
   return add_commas(buf, buf);
}

/*
 * Edit an integer number, the supplied buffer
 * must be at least 27 bytes long.  The incoming number
 * is always widened to 64 bits.
 */
char *edit_uint64(uint64_t val, char *buf)
{
   sprintf(buf, "%" lld, val);
   return buf;
}


/*
 * Convert a string duration to utime_t (64 bit seconds)
 * Returns 0: if error
	   1: if OK, and value stored in value
 */
int duration_to_utime(char *str, utime_t *value)
{
   int i, len;
   double val;
   /*
    * The "n" = mins and months appears before minutes so that m maps
    *   to months. These "kludges" make it compatible with pre 1.31 
    *	Baculas.
    */
   static const char *mod[] = {"n", "seconds", "months", "minutes", 
                  "hours", "days", "weeks",   "quarters",   "years", NULL};
   static const int32_t mult[] = {60,	1, 60*60*24*30, 60, 
		  60*60, 60*60*24, 60*60*24*7, 60*60*24*91, 60*60*24*365};
   char mod_str[20];
   int mod_len;

   /*
    * Look for modifier by walking back looking for the first
    *	space or digit.
    */
   strip_trailing_junk(str);
   len = strlen(str);
   /* Strip trailing spaces */
   for (i=len; i>0; i--) {
      if (!B_ISSPACE(str[i-1])) {
	 break;
      }
      str[i-1] = 0;
   }
   /* Find beginning of the modifier */
   for ( ; i>0; i--) {
      if (!B_ISALPHA(str[i-1])) {
	 break;
      }
   }
   /* If not found, error */
   if (i == 0 || i == len) {
      Dmsg2(200, "error i=%d len=%d\n", i, len);
      return 0;
   }
   /* Move modifier to mod_str */
   bstrncpy(mod_str, &str[i], sizeof(mod_str));
   mod_len = strlen(mod_str);
   if (mod_len == 0) {		      /* Make sure we have a modifier */
      Dmsg0(200, "No modifier found\n");
      return 0;
   }
   Dmsg2(200, "in=%s  mod=%s:\n", str, mod_str);
   /* Backup over any spaces in front of modifier */
   for ( ; i>0; i--) {
      if (B_ISSPACE(str[i-1])) {
	 continue;
      }
      str[i] = 0;
      break;
   }
   /* The remainder (beginning) should be our number */
   if (!is_a_number(str)) {
      Dmsg0(200, "input not a number\n");
      return 0;
   }
   /* Now find the multiplier corresponding to the modifier */
   for (i=0; mod[i]; i++) {
      if (strncasecmp(mod_str, mod[i], mod_len) == 0) {
	 break;
      }
   }
   if (mod[i] == NULL) {
      Dmsg0(200, "Modifier not found\n");
      return 0; 		      /* modifer not found */
   }
   Dmsg2(200, "str=%s: mult=%d\n", str, mult[i]);
   errno = 0;
   val = strtod(str, NULL);
   if (errno != 0 || val < 0) {
      return 0;
   }
  *value = (utime_t)(val * mult[i]);
   return 1;

}

/*
 * Edit a utime "duration" into ASCII
 */
char *edit_utime(utime_t val, char *buf)
{
   char mybuf[30];
   static int mult[] = {60*60*24*365, 60*60*24*30, 60*60*24, 60*60, 60};
   static char *mod[]  = {"year",  "month",  "day", "hour", "min"};
   int i;
   uint32_t times;

   *buf = 0;
   for (i=0; i<5; i++) {
      times = val / mult[i];
      if (times > 0) {
	 val = val - (utime_t)times * mult[i];
         sprintf(mybuf, "%d %s%s ", times, mod[i], times>1?"s":"");
	 strcat(buf, mybuf);
      }
   }
   if (val == 0 && strlen(buf) == 0) {	   
      strcat(buf, "0 secs");
   } else if (val != 0) {
      sprintf(mybuf, "%d sec%s", (uint32_t)val, val>1?"s":"");
      strcat(buf, mybuf);
   }
   return buf;
}

/*
 * Convert a size size in bytes to uint64_t
 * Returns 0: if error
	   1: if OK, and value stored in value
 */
int size_to_uint64(char *str, int str_len, uint64_t *rtn_value)
{
   int i, ch;
   double value;
   int mod[]  = {'*', 'k', 'm', 'g', 0}; /* first item * not used */
   uint64_t mult[] = {1,	     /* byte */
		      1024,	     /* kilobyte */
		      1048576,	     /* megabyte */
		      1073741824};   /* gigabyte */

#ifdef we_have_a_compiler_that_works
   int mod[]  = {'*', 'k', 'm', 'g', 't', 0};
   uint64_t mult[] = {1,	     /* byte */
		      1024,	     /* kilobyte */
		      1048576,	     /* megabyte */
		      1073741824,    /* gigabyte */
		      1099511627776};/* terabyte */
#endif

   Dmsg1(400, "Enter sized to uint64 str=%s\n", str);

   /* Look for modifier */
   ch = str[str_len - 1];
   i = 0;
   if (B_ISALPHA(ch)) {
      if (B_ISUPPER(ch)) {
	 ch = tolower(ch);
      }
      while (mod[++i] != 0) {
	 if (ch == mod[i]) {
	    str_len--;
	    str[str_len] = 0; /* strip modifier */
	    break;
	 }
      }
   }
   if (mod[i] == 0 || !is_a_number(str)) {
      return 0;
   }
   Dmsg3(400, "size str=:%s: %lf i=%d\n", str, strtod(str, NULL), i);

   errno = 0;
   value = strtod(str, NULL);
   if (errno != 0 || value < 0) {
      return 0;
   }
#if defined(HAVE_WIN32)
   /* work around microsofts non handling of uint64 to double cvt*/
   *rtn_value = (uint64_t)(value * (__int64)mult[i]);
   Dmsg2(400, "Full value = %lf %" lld "\n", value * (__int64)mult[i],  
	 (uint64_t)(value * (__int64)mult[i]));
#else
   *rtn_value = (uint64_t)(value * mult[i]);
   Dmsg2(400, "Full value = %lf %" lld "\n", value * mult[i],  
      (uint64_t)(value * mult[i]));
#endif
   return 1;
}

/*
 * Check if specified string is a number or not.
 *  Taken from SQLite, cool, thanks.
 */
int is_a_number(const char *n)
{
   bool digit_seen = false;

   if( *n == '-' || *n == '+' ) {
      n++;
   }
   while (B_ISDIGIT(*n)) {
      digit_seen = true;
      n++;
   }
   if (digit_seen && *n == '.') {
      n++;
      while (B_ISDIGIT(*n)) { n++; }
   }
   if (digit_seen && (*n == 'e' || *n == 'E')
       && (B_ISDIGIT(n[1]) || ((n[1]=='-' || n[1] == '+') && B_ISDIGIT(n[2])))) {
      n += 2;			      /* skip e- or e+ or e digit */
      while (B_ISDIGIT(*n)) { n++; }
   }
   return digit_seen && *n==0;
}

/*
 * Check if the specified string is an integer	 
 */
int is_an_integer(const char *n)
{
   bool digit_seen = false;
   while (B_ISDIGIT(*n)) {
      digit_seen = true;
      n++;
   }
   return digit_seen && *n==0;
}

/*
 * Check if Bacula Resoure Name is valid
 */
/* 
 * Check if the Volume name has legal characters
 * If ua is non-NULL send the message
 */
bool is_name_valid(char *name, POOLMEM **msg)
{
   int len;
   char *p;
   /* Special characters to accept */
   const char *accept = ":.-_ ";

   /* Restrict the characters permitted in the Volume name */
   for (p=name; *p; p++) {
      if (B_ISALPHA(*p) || B_ISDIGIT(*p) || strchr(accept, (int)(*p))) {
	 continue;
      }
      if (msg) {
         Mmsg(msg, _("Illegal character \"%c\" in name.\n"), *p);
      }
      return false;
   }
   len = strlen(name);
   if (len >= MAX_NAME_LENGTH) {
      if (msg) {
         Mmsg(msg, _("Name too long.\n"));
      }
      return false;
   }
   if (len == 0) {
      if (msg) {
         Mmsg(msg,  _("Volume name must be at least one character long.\n"));
      }
      return false;
   }
   return true;
}



/*
 * Add commas to a string, which is presumably
 * a number.  
 */
char *add_commas(char *val, char *buf)
{
   int len, nc;
   char *p, *q;
   int i;

   if (val != buf) {
      strcpy(buf, val);
   }
   len = strlen(buf);
   if (len < 1) {
      len = 1;
   }
   nc = (len - 1) / 3;
   p = buf+len;
   q = p + nc;
   *q-- = *p--;
   for ( ; nc; nc--) {
      for (i=0; i < 3; i++) {
	  *q-- = *p--;
      }
      *q-- = ',';
   }   
   return buf;
}

#ifdef TEST_PROGRAM
void d_msg(char*, int, int, char*, ...)
{}
int main(int argc, char *argv[])
{
   char *str[] = {"3", "3n", "3 hours", "3.5 day", "3 week", "3 m", "3 q", "3 years"};
   utime_t val;
   char buf[100];
   char outval[100];

   for (int i=0; i<8; i++) {
      strcpy(buf, str[i]);
      if (!duration_to_utime(buf, &val)) {
         printf("Error return from duration_to_utime for in=%s\n", str[i]);
	 continue;
      }
      edit_utime(val, outval);
      printf("in=%s val=%lld outval=%s\n", str[i], val, outval);
   }
}
#endif
