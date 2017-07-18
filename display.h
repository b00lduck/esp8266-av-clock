
extern unsigned char display_ram_second_dots[60];
extern unsigned char display_ram_hour_dots[12];
extern unsigned char display_ram_quarter_dots[4];
extern unsigned char display_ram_digits[6];
extern unsigned char display_ram_digit_dots[2];
extern unsigned char display_ram_digit_colon;

void display_init();
void display_update_decimal(unsigned char *digit, unsigned char value);
void display_update();
void display_render();
void display_send();



