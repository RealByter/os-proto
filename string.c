#include "string.h"

void reverse_string(char str[], int length)
{
  int start = 0;
  int end = length - 1;
  while(start < end)
  {
    char temp = str[start];
    str[start] = str[end];
    str[end] = temp;
    end--;
    start++;
  }
}

char* int_to_string(int num, char* str, int base)
{
  int i = 0;
  int is_negative = 0;

  /* Handle 0 explicitly, otherwise empty string is
   * printed for 0 */
  if(num == 0)
  {
    str[i] = '0';
    str[i+1] = '\0';
    return str;
  }

  // In standard itoa(), negative numbers are handled
  // only with base 10. Otherwise numbers are
  // considered unsigned.
  if (num < 0 && base == 10) {
    is_negative = 1;
    num = -num;
  }

  // Process individual digits
  while (num != 0) 
  {
    int rem = num % base;
    str[i++] = (rem > 9) ? (rem - 10) + 'a' : rem + '0';
    num = num / base;
  }

  // If number is negative, append '-'
  if (is_negative)
    str[i++] = '-';
}