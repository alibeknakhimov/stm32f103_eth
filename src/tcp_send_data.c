#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#define RCC_APB2ENR     (*(volatile uint32_t*)(0x40021000UL + 0x18UL))
#define GPIOA_CRH       (*(volatile uint32_t*)(0x40010800UL + 0x04UL))
#define GPIOA_ODR       (*(volatile uint32_t*)(0x40010800UL + 0x0CUL))
#define USART_SR        (*(volatile uint32_t*)(0x40013800UL + 0x00UL))
#define USART_DR        (*(volatile uint32_t*)(0x40013800UL + 0x04UL))
#define USART_BRR       (*(volatile uint32_t*)(0x40013800UL + 0x08UL))
#define USART_CR1       (*(volatile uint32_t*)(0x40013800UL + 0x0CUL))
#define GPIOC_CRH       (*(volatile uint32_t*)(0x40011000UL + 0x04UL))
#define GPIOx_ODR       (*(volatile uint32_t*)(0x40011000UL + 0x0CUL))
bool gh = true;
void gpioa_init(void);
void gpioc_init(void);
void uart_init(void);
void uart_send(uint8_t data);
char* uart_receive_string(void);
void delay(void);

void mode(uint8_t mode);
void localip(uint8_t ip1, uint8_t ip2, uint8_t ip3, uint8_t ip4);
void submask(uint8_t ip1, uint8_t ip2, uint8_t ip3, uint8_t ip4);
void gateway(uint8_t ip1, uint8_t ip2, uint8_t ip3, uint8_t ip4);
void localport(uint16_t port);
void destinationip(uint8_t ip1, uint8_t ip2, uint8_t ip3, uint8_t ip4);
void destinationport(uint16_t port);
void baudrate(uint32_t rate);
void safechanges();


int main(){
    gpioc_init();
    GPIOx_ODR |= (1 << 13);
    gpioa_init();
    uart_init();
    GPIOA_ODR |= (1<<11);     //rst
    GPIOA_ODR |= (1<<12);    //cfg
    char received_string[10] = "";
    while (1) {
        strcpy(received_string, uart_receive_string());
        char* div = strchr(received_string, ':');
        *div = '\0';
        uint8_t value = atoi(div+1);
        if (strcmp(received_string, "B2") == 0 && value == 1) {
            GPIOx_ODR &= ~(1 << 13);       
        }
        if (strcmp(received_string, "B2") == 0 && value == 0) {
            GPIOx_ODR |= (1 << 13);       
        }
    }
    
    
}

char uart_receive(){
    while(!(USART_SR & (1<<5)));
    return USART_DR;
}

char* uart_receive_string() {
    static char buffer[100]; 
    int i = 0;  

    while (i < sizeof(buffer) - 1) {
        char c = uart_receive();
        buffer[i] = c;  
        if (c == '\n' || c == '\r') break;  
        i++;  
    } 

    buffer[i] = '\0';  
    return buffer; 
}


void gpioa_init(){
    RCC_APB2ENR |= (1 << 2) | (1 << 14);
    GPIOA_CRH &= ~(15 << 4);
    GPIOA_CRH &= ~(15 << 8);
    GPIOA_CRH |= (11 << 4);
    GPIOA_CRH |= (1 << 10);

    GPIOA_CRH &= ~(15 << 12);
    GPIOA_CRH |= (1 << 12);

    GPIOA_CRH &= ~(15 << 16);
    GPIOA_CRH |= (1 << 16);
}
void uart_init(){
    USART_CR1 |= (1 << 13);
    USART_BRR = 8000000/9600;
    USART_CR1 |= (1 << 2);
    USART_CR1 |= (1 << 3);
}


void uart_send(uint8_t data){
    while(!(USART_SR & (1<<7)));
    USART_DR = data;
}

void mode(uint8_t mode){
    uart_send(0x57);
    uart_send(0xab);
    uart_send(0x10);
    uart_send(mode);
}
void localip(uint8_t ip1, uint8_t ip2, uint8_t ip3, uint8_t ip4){
    uart_send(0x57);
    uart_send(0xab);
    uart_send(0x11);
    uart_send(ip1);
    uart_send(ip2);
    uart_send(ip3);
    uart_send(ip4);
}
void submask(uint8_t ip1, uint8_t ip2, uint8_t ip3, uint8_t ip4){
    uart_send(0x57);
    uart_send(0xab);
    uart_send(0x12);
    uart_send(ip1);
    uart_send(ip2);
    uart_send(ip3);
    uart_send(ip4);

}
void gateway(uint8_t ip1, uint8_t ip2, uint8_t ip3, uint8_t ip4){
    uart_send(0x57);
    uart_send(0xab);
    uart_send(0x13);
    uart_send(ip1);
    uart_send(ip2);
    uart_send(ip3);
    uart_send(ip4);
}
void localport(uint16_t port){
    uart_send(0x57);
    uart_send(0xab);
    uart_send(0x14);
    uart_send(port & 0xff);
    uart_send((port >> 8) & 0xff);
}
void destinationip(uint8_t ip1, uint8_t ip2, uint8_t ip3, uint8_t ip4){
    uart_send(0x57);
    uart_send(0xab);
    uart_send(0x15);
    uart_send(ip1);
    uart_send(ip2);
    uart_send(ip3);
    uart_send(ip4);
}
void destinationport(uint16_t port){
    uart_send(0x57);
    uart_send(0xab);
    uart_send(0x16);
    uart_send(port & 0xff);
    uart_send((port >> 8) & 0xff);
}
void baudrate(uint32_t rate){
    uart_send(0x57);
    uart_send(0xab);
    uart_send(0x21);
    uart_send(rate & 0xff);
    uart_send((rate >> 8) & 0xff);
    uart_send((rate >> 16) & 0xff);
    uart_send((rate >> 24) & 0xff);
}
void safechanges(){
    uart_send(0x57);
    uart_send(0xab);
    uart_send(0x0d);
    delay();
    uart_send(0x57);
    uart_send(0xab);
    uart_send(0x0e);
    delay();
    uart_send(0x57);
    uart_send(0xab);
    uart_send(0x5e);
    delay();
}

void delay(){
    for(volatile int i = 0; i<=10000; i++);
}
void gpioc_init(void){
    RCC_APB2ENR |= (1 << 4);

    GPIOC_CRH &= ~(15 << 20);
    GPIOC_CRH |= (1 << 20);
}