
typedef struct {
  unsigned char second;
  unsigned char minute;
  unsigned char hour;
  unsigned char weekday;
  unsigned char day;
  unsigned char month;
  unsigned int year;
} rtc_datetime_t;

void rtc_tick();
unsigned char rtc_get_halfstep();
unsigned char rtc_get_second();
unsigned char rtc_get_hour();
unsigned char rtc_get_minute();
bool rtc_update_by_ntp();



