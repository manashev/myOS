#include "multitasking.h"

#define TASKS_COUNT 10
#define TICKS_FOR_TASK 1000
registers_t tasks[TASKS_COUNT];
bool is_finished[TASKS_COUNT];
int current_task = -1;


void init_multitasking()
{
    init_multitasking_timer();

    

}

void switch_task(registers_t regs)
{

}

void multitasking_timer_callback(registers_t regs)
{
    static int tick = 0;
    ++tick;
    if (tick == TICKS_FOR_TASK || is_finished[current_task]) {
        tick = 0;
        switch_task(regs);
    }
}

void init_multitasking_timer()
{
    register_interrupt_handler(IRQ2, multitasking_timer_callback);

    // get the PIT value: hardware clock at 1193180 Hz
    u32 divisor = 1193180;

    // send the command
    send_byte_to_port(0x43, 0x36); // command port
    send_byte_to_port(0x40, low_8(divisor));
    send_byte_to_port(0x40, high_8(divisor));
}