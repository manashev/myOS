#include "isr.h"
#include "idt.h"
#include "print.h"
#include "ports.h"

namespace {

isr_t interrupt_handlers[IDT_ENTRIES];

const char *exception_messages[] = {
    "Division By Zero",
    "Debug",
    "Non Maskable Interrupt",
    "Breakpoint",
    "Into Detected Overflow",
    "Out of Bounds",
    "Invalid Opcode",
    "No Coprocessor",

    "Double Fault",
    "Coprocessor Segment Overrun",
    "Bad TSS",
    "Segment Not Present",
    "Stack Fault",
    "General Protection Fault",
    "Page Fault",
    "Unknown Interrupt",

    "Coprocessor Fault",
    "Alignment Check",
    "Machine Check",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",

    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved"
};
} // namespace

extern "C" 
{
    #define ISR(n) void isr ## n();
    #define IRQ(n) void irq ## n();
    #include "idt_gates.h"
    #undef IRQ
    #undef ISR
}

void init_isr() 
{
    #define ISR(n) set_idt_gate(n, (u32)isr ## n);
    #define IRQ(n) set_idt_gate(n, (u32)irq ## n);
    #include "idt_gates.h"
    #undef IRQ
    #undef ISR

    set_idt(); // Load with ASM
}

void isr_handler(registers_t r) 
{
    Screen::print("\nRecieved interrupt: ", color::red);
    Screen::print((int)r.int_no, color::yellow);

    Screen::print("\nMessage: '", color::red);
    Screen::print(exception_messages[r.int_no], color::yellow);

    Screen::print("'\nError code: ", color::red);
    Screen::print((int)r.err_code, color::yellow);
    Screen::print("\n\n");
}

void irq_handler(registers_t r)
{
    // send EOI to the PICs
    bool is_slave = r.int_no >= 40;
    if (is_slave) {
        send_byte_to_port(0xA0, 0x20);
    }
    send_byte_to_port(0x20, 0x20); // master

    if (interrupt_handlers[r.int_no] != 0) {
        isr_t handler = interrupt_handlers[r.int_no];
        handler(r);
    }
}

void register_interrupt_handler(u8 n, isr_t handler)
{
    interrupt_handlers[n] = handler;
}
