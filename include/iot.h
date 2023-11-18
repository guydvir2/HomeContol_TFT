#include <myIOT2.h>

myIOT2 iot;
struct MQTTmsg
{
    char topic[40];
    char msg[10];
};

uint8_t msginQue_counter = 0;
const uint8_t MaxMessagesInQue = 5;
MQTTmsg msgQue[MaxMessagesInQue];

void add_msg2que(const char *topic, const char *msg)
{
    if (msginQue_counter < MaxMessagesInQue - 1)
    {
        strcpy(msgQue[msginQue_counter].topic, topic);
        strcpy(msgQue[msginQue_counter].msg, msg);
        msginQue_counter++;
    }
}
void delete_msg_inQue(uint8_t n)
{
    for (uint8_t i = n; i < msginQue_counter; i++)
    {
        strcpy(msgQue[i].msg, msgQue[i + 1].msg);
        strcpy(msgQue[i].topic, msgQue[i + 1].topic);
    }
    msginQue_counter--;
}
void print_msgQue()
{
    char a[60];
    for (uint8_t i = 0; i < msginQue_counter; i++)
    {
        sprintf(a, "#%d; TOP:%s; MSG: %s", i, msgQue[i].topic, msgQue[i].msg);
        Serial.println(a);
    }
    Serial.println("#######\n");
}
void print_singleMSG(MQTTmsg &msg, uint8_t i)
{
    Serial.print("ID#: ");
    Serial.print(i);
    Serial.print("; TOT: ");
    Serial.print(msginQue_counter);
    Serial.print("; TOP: ");
    Serial.print(msgQue[i].topic);
    Serial.print("; MSG: ");
    Serial.println(msgQue[i].msg);
}
void send_msgsInQue()
{
    const uint8_t send_interval = 100;
    static unsigned long last_msg_sent_time = 0;

    if (msginQue_counter > 0 && millis() - last_msg_sent_time > send_interval * 5)
    {
        last_msg_sent_time = millis();
        print_singleMSG(msgQue[msginQue_counter], 0);
        delete_msg_inQue(0);
    }
}
void gen_dummyMsg(uint8_t n)
{
    for (int8_t i = 0; i < n; i++)
    {
        sprintf(msgQue[i].topic, "Topic_%s", String(millis()));
        sprintf(msgQue[i].msg, "msg%d", i);
        msginQue_counter++;
        delay(5);
    }
}
void extMQTT(char *incoming_msg, char *_topic)
{
    Serial.println(incoming_msg);
    char msg[150];
    if (strcmp(incoming_msg, "status") == 0)
    {
        sprintf(msg, "BOO");
        iot.pub_msg(msg);
    }
    else if (strcmp(incoming_msg, "help2") == 0)
    {
        sprintf(msg, "help #2:No other functions");
        iot.pub_msg(msg);
    }
    else if (strcmp(incoming_msg, "ver2") == 0)
    {
        sprintf(msg, "ver #2:");
        iot.pub_msg(msg);
    }
}
void update_clock()
{
    char a[22];
    iot.get_timeStamp(a);
}
void start_iot2()
{
    iot.useSerial = true;
    iot.useFlashP = false;
    iot.noNetwork_reset = 2;
    iot.ignore_boot_msg = false;

    iot.add_gen_pubTopic("myHome/Messages");
    iot.add_gen_pubTopic("myHome/log");
    iot.add_gen_pubTopic("myHome/debug");
    iot.add_subTopic("myHome/test");
    iot.add_pubTopic("myHome/test/Avail");
    iot.add_subTopic("myHome/alarmMonitor/State");

    iot.start_services(extMQTT);
}