#include <LPC21xx.h>
#include <string.h>
#include "delay.h"
#include "lcd.h"
#include "uart0.h"
#include "i2c.h"
#include "kpm.h"
#include "R305.h"
#include "interrupts_pro.h"
#define IN1 (1 << 25)
#define IN2 (1 << 24)
#define EN (1 << 26)
#define MAX_USERS 3
typedef struct
{
    int password;
    int fingerprint_id;
}User;

User users[MAX_USERS] = {
        {1234, 1}, // User 1
    {5678, 2}, // User 2
    {1023, 3}  // User 3
};
#define IN1 (1 << 25)
#define IN2 (1 << 24)

void edit_fingerprint(void);
void save_password_to_eeprom(void)
{
        int i;
    u8 msb, lsb;
    u8 slaveAddr = 0xA0 >> 1; // AT24C02 or AT25C256 typically use 0xA0 (shifted)

    for (i = 0; i < MAX_USERS; i++)
        {
        int pass = users[i].password;
        msb = (pass >> 8) & 0xFF;
        lsb = pass & 0xFF;

        // Write MSB at address i*2
        i2c_eeprom_bytewrite(slaveAddr, i * 2, msb);
        delay_ms(5);  // Wait for EEPROM write cycle

        // Write LSB at address i*2 + 1
        i2c_eeprom_bytewrite(slaveAddr, i * 2 + 1, lsb);
        delay_ms(5);
    }
}

void load_password_from_eeprom(void)
{
    u8 msb, lsb;
    u8 slaveAddr = 0xA0 >> 1;
        int i;
    for (i = 0; i < MAX_USERS; i++)
        {
        msb = i2c_eeprom_randomread(slaveAddr, i * 2);
        lsb = i2c_eeprom_randomread(slaveAddr, i * 2 + 1);
        users[i].password = (msb << 8) | lsb;
    }
}


void motor_init(void) {
    IODIR1 |= IN1 | IN2 | EN;
        IOSET1 = EN;
}

void motor_open(void) {
    IOSET1 = IN1;
    IOCLR1 = IN2;
    delay_ms(500);
    IOCLR1 = IN1;
}

void motor_close(void) {
    IOSET1 = IN2;
    IOCLR1 = IN1;
    delay_ms(500);
    IOCLR1 = IN2;
}

int check_user(int user_id, int password, int fingerprint_id)
{
        user_id=user_id-1;
    if (user_id < 0 || user_id >= MAX_USERS)
        return 0;

    if ((password == users[user_id].password) && (fingerprint_id == users[user_id].fingerprint_id))
        {
        return 1;
    }
    return 0;
}
int main(void)

{

    int entered_pass;
    int fingerprint_id;
    char key;
    //int i;

    LCD_Init();
        init_i2c();
    InitUART0();
        Initkpm();
        motor_init();
        //Enable_EINT1();

    Write_CMD_LCD(0x01);
    Write_str_LCD("BIOMETRIC LOCKER");
    delay_ms(1000);

        // Save initial passwords to EEPROM
    //save_password_to_eeprom();
    //load_password_from_eeprom();

    while(1)
    {
        Write_CMD_LCD(0x01);
        Write_str_LCD("1:Unlock 3:Reset");
        do
                {
                        key = keyScan();
        }while (key != '1' && key != '3');
                if (key == '1')
                {
                int user_id;
                        Write_CMD_LCD(0x01);
                Write_str_LCD("Enter User ID:");

                do {
                        key = keyScan();
                   } while (key < '0' || key >= ('0' + MAX_USERS));
                user_id = key - '0';

                        Write_CMD_LCD(0x01);
                Write_str_LCD("Enter Pass:");
            entered_pass =Password_scan();

                        Write_CMD_LCD(0x01);
                Write_str_LCD("Place Finger...");
                fingerprint_id = search_fp() - 1;
                        Write_int_LCD(fingerprint_id);
                delay_ms(500);

                if (check_user(user_id, entered_pass, fingerprint_id))
                        {
                        Write_CMD_LCD(0x01);
                        Write_str_LCD("Access Granted");
                        motor_open();
                        delay_ms(1000);
                        motor_close();
                }
                        else
                        {
                        Write_CMD_LCD(0x01);
                        Write_str_LCD("Access Denied");
                                delay_ms(1500);
                }
                }
                else if(key == '3')
                {
                        edit_fingerprint();
                }
        }
}
void edit_password(void);
void edit_fingerprint(void);
void eint0_isr(void)__irq
{
    char key;

    Write_CMD_LCD(0x01);
    Write_str_LCD("1:PWD 2:FP");

    do
        {
        key = keyScan();
    } while (key != '1' && key != '2');

    if (key == '1')
        {
        edit_password();
    }
        else if (key == '2')
        {
        edit_fingerprint();
    }
    EXTINT |= 0x01;
    VICVectAddr = 0;
}
void edit_password(void)
{
    int user_id;
    int current = 0, new_pass = 0, confirm = 0;
    char key;

    Write_CMD_LCD(0x01);
    Write_str_LCD("Enter ID:");

    do
        {
        key = keyScan();
    }while (key < '0' || key >= ('0' + MAX_USERS));
    user_id = key - '0';

    Write_CMD_LCD(0x01);
    Write_str_LCD("CurPass:");

    current = Password_scan();

    if (current == users[user_id].password)
        {
        Write_CMD_LCD(0x01);
        Write_str_LCD("NewPass:");

        new_pass = Password_scan();

        Write_CMD_LCD(0x01);
        Write_str_LCD("Confirm:");

                confirm = Password_scan();

        if (new_pass == confirm)
                {
            users[user_id].password = new_pass;
            //save_password_to_eeprom();
                        save_password_to_eeprom();// Save new to EEPROM
            Write_CMD_LCD(0x01);
            Write_str_LCD("Password Set");
        }
                else
                {
            Write_CMD_LCD(0x01);
            Write_str_LCD("Mismatch");
        }
        }
                else
                {
                Write_CMD_LCD(0x01);
                Write_str_LCD("Wrong Password");
        }
                delay_ms(1500);
}
void edit_fingerprint(void)
{
    char key;
    int id;

    Write_CMD_LCD(0x01);
    Write_str_LCD("1:ENR 2:DEL");
    Write_CMD_LCD(0xC0);
    Write_str_LCD("3:DEL ALL");

    do
        {
        key = keyScan();
    } while (key != '1' && key != '2' && key != '3');

    if (key == '1')
        {
                Write_str_LCD("Enrolled");
                        Write_CMD_LCD(0x01);
                        Write_CMD_LCD(0x80);
                        Write_str_LCD("Enter ID: ");
                        key = keyScan();
                        key = key - '0';
                        Write_CMD_LCD(0xC0);
                        Write_int_LCD(key);
                delay_ms(100);
                        if(key>0 && key<10)
                        {
                                enroll(key);
                        }
    }
    else if (key == '2')
        {
        Write_CMD_LCD(0x01);
        Write_str_LCD("Enter ID:");
        key = keyScan();
        id = key - '0';
        if (delete_fp(id) == 0x00)
            Write_str_LCD("Deleted");
        else
                {
            Write_str_LCD("Fail");

                }
    }
    else if (key == '3')
        {
        if (deleteall_fp() == 0x00)
            Write_str_LCD("All Del");
        else
            Write_str_LCD("Del Fail");
    }
    delay_ms(1500);
}                       